use std::io::{self, Write};
use std::path::Path;

const WEIGHTINESS: u32 = 32;
const MAX_NODES: usize = 256;
const SYSFS_PATH: &str = "/sys/kernel/mm/mempolicy/weighted_interleave";

fn gcd(a: u32, b: u32) -> u32 {
    let mut a = a;
    let mut b = b;
    while b != 0 {
        let t = a % b;
        a = b;
        b = t;
    }
    a
}

fn gcd_array(arr: &[u32]) -> u32 {
    let mut g = arr[0];
    for &v in &arr[1..] {
        g = gcd(g, v);
    }
    g
}

fn write_weight(node: usize, weight: u32) -> io::Result<()> {
    let path = format!("{}/node{}", SYSFS_PATH, node);
    std::fs::write(&path, format!("{}\n", weight))?;
    Ok(())
}

fn detect_nodes() -> usize {
    for i in 0..MAX_NODES {
        let path = format!("{}/node{}", SYSFS_PATH, i);
        if !Path::new(&path).exists() {
            return i;
        }
    }
    MAX_NODES
}

fn main() {
    if !Path::new(SYSFS_PATH).exists() {
        println!("Error: {} not found", SYSFS_PATH);
        println!("Please enable CONFIG_NUMA and weighted interleave in kernel.");
        return;
    }

    let nodes = detect_nodes();

    if nodes == 0 {
        println!("No nodeX entries found under {}", SYSFS_PATH);
        println!("Kernel may not support weighted interleave.");
        return;
    }

    println!("Detected {} nodes\n", nodes);

    let mut bw: Vec<u64> = Vec::with_capacity(nodes);
    let mut total: u64 = 0;

    for i in 0..nodes {
        print!("Enter bandwidth for node{} (MB/s): ", i);
        io::stdout().flush().unwrap();

        let mut input = String::new();
        if io::stdin().read_line(&mut input).is_err() {
            println!("Invalid input");
            return;
        }

        match input.trim().parse::<u64>() {
            Ok(v) => {
                bw.push(v);
                total += v;
            }
            Err(_) => {
                println!("Invalid input");
                return;
            }
        }
    }

    if total == 0 {
        println!("Total bandwidth is zero");
        return;
    }

    let mut weight: Vec<u32> = Vec::with_capacity(nodes);

    for i in 0..nodes {
        let w = ((WEIGHTINESS as u64 * bw[i]) / total) as u32;
        weight.push(if w == 0 { 1 } else { w });
    }

    let g = gcd_array(&weight);
    for w in &mut weight {
        *w /= g;
    }

    println!("\nCalculated weights:");

    for i in 0..nodes {
        println!("node{}: bw={} -> weight={}", i, bw[i], weight[i]);
    }

    print!("\nWrite these weights to sysfs? [y/N]: ");
    io::stdout().flush().unwrap();

    let mut response = String::new();
    io::stdin().read_line(&mut response).ok();

    let c = response.trim().chars().next().unwrap_or_default();

    if c != 'y' && c != 'Y' {
        println!("Aborted.");
        return;
    }

    println!("\nWriting sysfs:");

    for i in 0..nodes {
        match write_weight(i, weight[i]) {
            Ok(_) => println!("node{}: weight={} written successfully", i, weight[i]),
            Err(e) => {
                eprintln!("node{}: failed to write: {}", i, e);
            }
        }
    }
}

# wi_bw_tool

NUMA weighted interleave memory policy management tool. Reads bandwidth values for each node, calculates proportional weights, and writes them to the kernel sysfs interface.

## Build

### Using Makefile (Recommended)

```bash
make          # Build release version
make debug    # Build with debug symbols
make run      # Build and run
make clean    # Clean build artifacts
```

### Manual Build

```bash
gcc -o wi_bw_tool wi_bw_tool.c -Wall -Wextra
```

### Rust Version

```bash
cd rust_wi_bw_tool
cargo build --release
./target/release/wi_bw_tool
```

## Usage

```bash
./wi_bw_tool
```

### Permission Requirements

Writing to sysfs requires root privileges:

```bash
sudo ./wi_bw_tool
```

### Example

```bash
$ sudo ./wi_bw_tool
Detected 3 nodes

Enter bandwidth for node0 (MB/s): 200
Enter bandwidth for node1 (MB/s): 20
Enter bandwidth for node2 (MB/s): 10

Calculated weights:
node0: bw=200 -> weight=27
node1: bw=20 -> weight=2
node2: bw=10 -> weight=1

Write these weights to sysfs? [y/N]: y

Writing sysfs:
node0: weight=27 written successfully
node1: weight=2 written successfully
node2: weight=1 written successfully
```

### Prerequisites

- Kernel with NUMA support enabled
- Kernel with weighted interleave memory policy enabled
- `/sys/kernel/mm/mempolicy/weighted_interleave/node*` path exists

## Notes

- Requires a Linux system with NUMA support
- May need root privileges to write to sysfs
- If sysfs path does not exist, the kernel does not support weighted interleave policy

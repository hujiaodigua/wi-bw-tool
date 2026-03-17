# wi_bw_tool

NUMA 加权交错内存策略管理工具。读取每个节点的带宽值，计算比例权重，并将其写入内核 sysfs 接口。

## 版本

本项目共有 3 个版本：

1. **用户空间程序（C 语言版）** - `wi_bw_tool.c`
2. **用户空间程序（Rust 语言版）** - `rust_wi_bw_tool/`
3. **Linux 内核修改版** - `linux-patch/`

> Linux 内核 patch 来自 [Jonathan Cameron](https://lore.kernel.org/linux-cxl/abLl84pc7tavuieW@gourry-fedora-PF4VCD3F/) 的提交，并经过其本人勘误。

## 编译

### 使用 Makefile（推荐）

```bash
make          # 编译发布版本
make debug    # 编译调试版本（带调试符号）
make run      # 编译并运行
make clean    # 清理构建产物
```

### 手动编译

```bash
gcc -o wi_bw_tool wi_bw_tool.c -Wall -Wextra
```

### Rust 版本

```bash
cd rust_wi_bw_tool
cargo build --release
./target/release/wi_bw_tool
```

## 使用

```bash
./wi_bw_tool
```

### 权限要求

写入 sysfs 需要 root 权限：

```bash
sudo ./wi_bw_tool
```

### 示例

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

### 前置条件

- 内核已启用 NUMA 支持
- 内核已启用加权交错（weighted interleave）内存策略
- `/sys/kernel/mm/mempolicy/weighted_interleave/node*` 路径存在

## 注意事项

- 需要具有 NUMA 支持的 Linux 系统
- 可能需要 root 权限才能写入 sysfs
- 如果 sysfs 路径不存在，说明内核不支持加权交错策略

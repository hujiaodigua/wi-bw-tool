# AGENTS.md - wi_bw_tool

## Project Overview

This is a minimal C utility for managing NUMA weighted interleave memory policy. It reads bandwidth values for each node, calculates proportional weights, and writes them to the kernel sysfs interface.

**Single source file:** `wi_bw_tool.c`

---

## Build Commands

### Compile
```bash
gcc -o wi_bw_tool wi_bw_tool.c -Wall -Wextra
```

### Compile with debug symbols
```bash
gcc -g -O0 -o wi_bw_tool wi_bw_tool.c -Wall -Wextra
```

### Run
```bash
./wi_bw_tool
```

### Using Makefile
```bash
make          # Build release
make debug    # Build with debug symbols
make run      # Build and run
make clean    # Remove build artifacts
```

### Clean build artifacts
```bash
rm -f wi_bw_tool
```

---

## Code Style Guidelines

### Formatting

- **Indentation:** Use tabs (not spaces)
- **Line length:** Keep under 100 characters when practical
- **Braces:** Opening brace on same line for functions, separate line for control statements
  ```c
  int main(void)
  {
      if (condition) {
          do_something();
      }
  }
  ```

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Functions | snake_case | `detect_nodes`, `write_weight` |
| Variables | snake_case | `total`, `weight[i]` |
| Constants/Macros | UPPER_SNAKE_CASE | `WEIGHTINESS`, `MAX_NODES`, `SYSFS_PATH` |
| File names | snake_case | `wi_bw_tool.c` |

### Functions

- Place return type on separate line from function name
- Opening brace on its own line
- Use static for file-local functions
- Brief comment for non-trivial logic
  ```c
  static unsigned int gcd(unsigned int a, unsigned int b)
  {
      while (b) {
          unsigned int t = a % b;
          a = b;
          b = t;
      }
      return a;
  }
  ```

### Includes

- Standard library headers first, then custom
- Alphabetical order within groups
- No redundant includes

### Error Handling

- Return -1 or non-zero for errors
- Use `perror()` for system errors with errno context
- Print user-friendly messages to stdout/stderr
- Always check return values of `fopen()`, `scanf()`, etc.

### Types

- Use `int` for general integers, `unsigned int` for non-negative values
- Use `unsigned long` for values that may exceed INT_MAX (e.g., bandwidth in MB/s)
- Use `char` for strings and single characters
- Avoid `long` (platform-dependent size)

### Control Flow

- Always use braces for blocks, even single-line statements
  ```c
  // Wrong
  if (condition)
      do_something();

  // Correct
  if (condition) {
      do_something();
  }
  ```

- Put the opening brace on the same line as the condition

### Memory and Resources

- Always close files after opening (`fclose()`)
- No dynamic memory allocation in this project (avoid `malloc`/`free`)

### Comments

- Use English for all comments
- Brief inline comments for tricky logic
- Chinese comments acceptable for user-facing output strings

---

## Testing

There are no automated tests for this project. Manual testing:

1. Compile with debug flags
2. Run on a system with NUMA support and weighted interleave enabled
3. Verify sysfs paths exist before running
4. Check that weights are written correctly to `/sys/kernel/mm/mempolicy/weighted_interleave/node*`

---

## Common Issues

- **Sysfs not found:** Ensure kernel has `CONFIG_NUMA` and weighted interleave enabled
- **Permission denied:** May need root privileges to write to sysfs
- **Node detection fails:** Kernel may not support weighted interleave policy

---

## Dependencies

- Standard C library (libc)
- Linux kernel with NUMA support
- GCC or compatible C compiler

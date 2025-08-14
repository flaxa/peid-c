
# PEiD-C

PEiD-C is a lightweight tool for identifying packers, cryptors, and protectors used in Windows executables. It is a C version of the Python version [Python PEiD](https://github.com/packing-box/peid).


## Building

### Build on Linux

```bash
make

```

### Build on Windows (native)

```cmd
# Open Command Prompt or PowerShell
make
```

The compiled binary will appear in the `bin/` directory.

## Usage

```bash
# Linux
./bin/peid-c -i /path/to/binary.exe

# Windows
peid-c.exe -i C:\path\to\binary.exe
```
```bash
$ ./peid-c_x86_64 -h
Usage: ./peid-c [OPTIONS] -i FILE
Defaults: search from entry point, print all matches.

Options:
  -f, --full       Search the whole file instead of from entry point
  -1, --first      Stop after first match instead of printing all matches
  -i, --input FILE Input file to scan (required)
  -h, --help       Show this help message
```
## Release Files

- `peid-c_x86_64` → 64-bit Linux  
- `peid-c_i686` → 32-bit Linux  
- `peid-c_x86_64.exe` → 64-bit Windows  
- `peid-c_i686.exe` → 32-bit Windows  



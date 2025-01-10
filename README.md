# CHIP-8 Emulator

This project is a CHIP-8 emulator written in C. The CHIP-8 is a simple, interpreted programming language used in the 1970s to aid in creating games on early computers.

## Features

- Emulates the CHIP-8 instruction set
- Supports loading and executing CHIP-8 programs
- Provides a simple SDL2 display for interacting with the programs

### Prerequisites

Ensure you have the following installed on your system:

- **C Compiler**: `gcc` or any standard C compiler
- **Make**: For building the project

### Installation

1. Clone the repository:

```bash
git clone https://github.com/Nathan5563/chip-8.git
```

2. Navigate to the source directory:

```bash
cd chip-8
```

3. Build the project:

```bash
make
```

### Usage
After building the project, you can run the emulator with a CHIP-8 program:

```bash
./chip8 path
```
Replace `path` with the actual path to the CHIP-8 program you wish to run. The `ROMs/` directory contains a list of sample programs.

### License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.
# Operating Systems Programming Assignment 02

## Project Title
Implementation of Linux `ls` Utility in C

## Student Information
- **Roll Number:** BSDSF24A043
- **Operating System:** Kali Linux
- **Programming Language:** C
- **Compiler:** GCC

## Project Description
This project implements a simplified version of the Linux `ls` command using the C programming language. It displays directory contents and supports multiple listing options.

## Implemented Features

### Version 1.1.0 — Long Listing (`-l`)
Displays file permissions, link count, owner, group, file size, modification time, and filename.

### Version 1.2.0 — Column Display
Displays filenames in columns by default. The program uses terminal width to arrange the output.

### Version 1.3.0 — Horizontal Display (`-x`)
Displays filenames horizontally, from left to right.

### Version 1.4.0 — Alphabetical Sorting
Sorts filenames alphabetically in the normal and horizontal display modes.

### Version 1.5.0 — Colorized Output
Uses ANSI escape codes to display directories in blue, executable files in green, and symbolic links in cyan.

### Version 1.6.0 — Recursive Listing (`-R`)
Displays directory contents recursively, including subdirectories.

## Project Structure

```text
BSDSF24A043-OS-A02/
├── src/
│   └── lsv1.0.0.c
├── bin/
│   └── ls
├── obj/
├── man/
├── Makefile
├── README.md
└── REPORT.md
```

## Compilation

Run the following command from the project directory:

```bash
make clean && make
```

## Usage

### Default Listing

```bash
./bin/ls
```

### Long Listing

```bash
./bin/ls -l
```

### Horizontal Display

```bash
./bin/ls -x
```

### Recursive Listing

```bash
./bin/ls -R
```

### Long Recursive Listing

```bash
./bin/ls -lR
```

### Horizontal Recursive Listing

```bash
./bin/ls -xR
```

### List a Specific Directory

```bash
./bin/ls src
```

## Git Branches and Version History

The project uses separate Git branches for feature development.

| Version | Feature | Branch |
|---|---|---|
| v1.1.0 | Long Listing | `feature-long-listing-v1.1.0` |
| v1.2.0 | Column Display | `feature-column-display-v1.2.0` |
| v1.3.0 | Horizontal Display | `feature-horizontal-display-v1.3.0` |
| v1.4.0 | Alphabetical Sorting | `feature-alphabetical-sorting-v1.4.0` |
| v1.5.0 | Colorized Output | `feature-colorized-output-v1.5.0` |
| v1.6.0 | Recursive Listing | `feature-recursive-listing-v1.6.0` |

## Testing

The application should be compiled and tested using the commands listed in the Usage section. Verify each feature before submission.

## Conclusion

This project demonstrates directory handling, file metadata retrieval, dynamic memory allocation, alphabetical sorting, terminal output formatting, colorized output, and recursive directory traversal using C.

## Student Information

- **Roll Number:** BSDSF24A043
- **Assignment:** Operating Systems Programming Assignment 02

# Operating Systems Programming Assignment 02

## Implementation of Linux ls Utility in C

### Student Information
- **Roll Number:** BSDSF24A043
- **Operating System:** Kali Linux
- **Programming Language:** C
- **Compiler:** GCC

## 1. Introduction

The purpose of this assignment is to implement a simplified version
of the Linux `ls` utility using the C programming language.

The program reads directory contents and displays filenames and
file information using different command-line options.

## 2. Objectives

The main objectives of this assignment are:

- Read directory contents using C.
- Display detailed file information.
- Arrange filenames in columns.
- Display filenames horizontally.
- Sort filenames alphabetically.
- Apply colors according to file types.
- List directories recursively.

## 3. Implemented Features

### 3.1 Long Listing — v1.1.0

The `-l` option displays file permissions, link count, owner, group,
file size, modification time, and filename.

### 3.2 Column Display — v1.2.0

The default display arranges filenames in columns based on terminal
width.

### 3.3 Horizontal Display — v1.3.0

The `-x` option displays filenames horizontally, from left to right.

### 3.4 Alphabetical Sorting — v1.4.0

The `qsort()` function and a comparison function sort filenames
alphabetically in the normal and horizontal display modes.

### 3.5 Colorized Output — v1.5.0

ANSI escape codes provide different colors for file types:
- Directories: Blue
- Executable files: Green
- Symbolic links: Cyan

### 3.6 Recursive Listing — v1.6.0

The `-R` option lists the contents of a directory and its
subdirectories recursively.

## 4. Implementation Details

The program uses the following C and Linux facilities:

- `opendir()` and `readdir()` to read directory entries.
- `stat()` / `lstat()` to retrieve file metadata.
- `getpwuid()` and `getgrgid()` to retrieve owner and group names.
- `qsort()` to sort filenames.
- Dynamic memory allocation to store filenames.
- `ioctl()` to obtain terminal width.
- Recursive function calls to traverse subdirectories.
- ANSI escape sequences to colorize output.

## 5. Compilation

The program is compiled using GCC through the Makefile.

Command:

```bash
make clean && make
```

## 6. Test Commands

The following commands can be used to test the application:

```bash
./bin/ls
./bin/ls -l
./bin/ls -x
./bin/ls -R
./bin/ls -lR
./bin/ls -xR
```

Each command should be executed and its output checked before
submission.

## 7. Version History

| Version | Feature |
|---|---|
| v1.1.0 | Long Listing |
| v1.2.0 | Column Display |
| v1.3.0 | Horizontal Display |
| v1.4.0 | Alphabetical Sorting |
| v1.5.0 | Colorized Output |
| v1.6.0 | Recursive Listing |

## 8. Conclusion

This assignment demonstrates directory handling, file metadata
retrieval, dynamic memory allocation, alphabetical sorting,
terminal output formatting, colorized output, and recursive
directory traversal in C.

The project also provides practice with GCC, Makefiles, Git
branches, commits, and version tags.

# PandorOS

Another attempt to create a small operating system. This time, more features, less fancy code.

## Concept

The whole OS is created with the OS of the [TI-83 Plus](https://en.wikipedia.org/wiki/TI-83_series#TI-83_Plus) in mind.

The feature list is simple: - **PandorOS Basic** Command Shell - Flat File System - `.PRG` Program Files - `.TXT` Text Files - `.DAT` Data Files (similar to a binary file) - File Editor for `.PRG` and `.TXT` files. - Command Catalog with in-system-help - Character Map for printing every possible character

## File System

PandorOS uses an In-RAM file system that allows the user to create programs and data files.

See [filesystem.md](filesystem.md) for more details.

## PandorOS Basic

The basic dialect of PandorOS. Quite restricted, quite simple.

### Expressions

PandorOS Basic supports all basic expression types:

| Expression      | Example    |
| --------------- | ---------- |
| Number          | `10`       |
| String          | `"Name"`   |
| Variable Access | `A`        |
| Negation        | `-A`       |
| Parenthesis     | `(A)`      |
| Addition        | `A + B`    |
| Subtraction     | `A - B`    |
| Multiplication  | `A * B`    |
| Division        | `A / B`    |
| Logic And       | `A And B`  |
| Logic Or        | `A Or B`   |
| Logic Not       | `Not A`    |
| Function Call   | `Mod(A,B)` |

### Variables

Pandoros Basic only supports 26 distinct numeric variables and 10 string variables:

| Variable        | Type   |
| --------------- | ------ |
| `A` - `Z`       | Number |
| `Str0` - `Str9` | String |

### Assignments

Each expression returns a result that can be stored in a variable. An
assignment is a postfix operation where an arrow (`->`) is followed by
the variable name:

    10 -> A
    20 - A -> B
    "Hello World!" -> Str0

### Functions

A function is a command that takes a list of arguments and returns a value.
It may be part of an expression.

    Product(10, B, 20)

### Orders

An order is a special type of function that must be the first component of a
statement. It does not return any value and is separated from its argument list
by a whitespace character instead of brackets.

    Disp 10, 20

### Example

> Current status allows no multiline program execution, only single-line statements.
> Also, no control structures are implemented.

    Input "Enter Variable 1: ", A
    Input "Enter Variable 2: ", B
    A - B -> D
    A / B -> Q
    Print "Sum        = ", A + B
    Print "Difference = ", D
    Print "Product    = ", Product(A, B, 1)
    Print "Quotient   = ", Q

## Building

Requirements:

- re2c (https://re2c.org/)
- lemon (https://github.com/sqlite/sqlite/blob/master/tool/lemon.c + https://github.com/sqlite/sqlite/blob/master/tool/lempar.c)
- lua5
- mono + mcs
- clang
- syslinux

```sh-session
[user@host pandoros]$ make -C kernel
[user@host pandoros]$ qemu-system-x86_64 -kernel kernel/kernel -hda kernel/root.pfs
```

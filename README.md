# Minishell

A Unix shell implementation written in C as part of the 42 Common Core.

Minishell recreates essential behavior of a command-line shell, including command parsing, environment-variable expansion, pipelines, redirections, built-in commands, process management, and signal handling.

The project focuses on understanding how a shell coordinates parsing with low-level Unix primitives such as processes, file descriptors, pipes, and signals.

## Key Features

- Interactive command prompt using GNU Readline
- Command tokenization and parsing
- Environment-variable expansion
- Single and multi-command pipelines
- Input and output redirections
  - `<`
  - `>`
  - `>>`
  - `<<` heredoc
- PATH-based executable resolution
- Environment management
- Shell-compatible exit status handling
- Interactive signal handling
- Built-in commands:
  - `echo`
  - `cd`
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`

## Architecture

The implementation separates the shell into parsing and execution layers:

```text
Mini-SHELL/
├── parsing/
│   ├── tokenizer
│   ├── variable expansion
│   ├── redirection parsing
│   ├── heredoc handling
│   └── command construction
│
├── execution/
│   ├── command execution
│   ├── pipelines
│   ├── redirections
│   ├── built-ins
│   ├── environment handling
│   └── error handling
│
├── includes/
│   ├── shared utilities
│   ├── memory management
│   ├── signal handling
│   └── environment utilities
│
└── minishell.c
```

This separation allows parsed commands to be transformed into execution structures before the process-management layer handles pipes, file descriptors, built-ins, and external programs.

## Execution Model

External commands are executed using the standard Unix process model:

```text
command
   │
   ├── resolve executable
   │
   ├── fork()
   │
   └── child
        ├── configure redirections / pipes
        └── execve()
```

For pipelines:

```text
cmd1 | cmd2 | cmd3

cmd1 stdout
     │
     ▼
   pipe
     │
     ▼
cmd2 stdin → cmd2 stdout
                    │
                    ▼
                  pipe
                    │
                    ▼
                 cmd3 stdin
```

The shell creates the required pipes and child processes, redirects their standard input/output with `dup2()`, closes unused file descriptors, and waits for process completion.

## Built-in Commands

The shell implements the standard project built-ins:

| Command | Purpose |
|---|---|
| `echo` | Print arguments |
| `cd` | Change working directory |
| `pwd` | Display current directory |
| `export` | Add or update environment variables |
| `unset` | Remove environment variables |
| `env` | Display the environment |
| `exit` | Exit the shell |

Commands that must modify the shell's own state are handled accordingly rather than always being delegated to a child process.

## Build

### Requirements

- C compiler
- GNU Make
- GNU Readline

Compile with:

```bash
make
```

The project is compiled with:

```text
-Wall -Wextra -Werror
```

## Usage

Start the shell:

```bash
./minishell
```

Examples:

```bash
minishell$ echo "Hello, world"
Hello, world

minishell$ export PROJECT=minishell
minishell$ echo $PROJECT
minishell

minishell$ cat file.txt | grep hello

minishell$ cat < input.txt | grep error > output.txt
```

Clean object files:

```bash
make clean
```

Remove the executable and object files:

```bash
make fclean
```

Rebuild:

```bash
make re
```

## Technical Concepts

This project provided hands-on experience with:

- Unix process creation
- `fork()` / `execve()` execution
- Pipes and inter-process communication
- File descriptors
- `dup2()` and descriptor redirection
- Process synchronization with `waitpid()`
- Environment variables
- PATH resolution
- Unix signals
- Shell exit statuses
- Command parsing and tokenization
- Dynamic memory and resource management

## Project Context

Minishell is a team project from the 42 Common Core designed to reproduce a subset of Bash behavior while developing a deeper understanding of Unix process management and shell architecture.

Rather than relying on high-level process-management libraries, the project builds the execution model directly using Unix system calls.

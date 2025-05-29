# Minishell Testing Guide

This guide provides a comprehensive overview of critical and special test cases for the 42 Minishell project. Passing these tests, especially the mandatory ones, is crucial for successful validation. This document draws heavily from common 42 evaluation criteria and publicly available test suites.

## Table of Contents

1.  [Introduction](#introduction)
2.  [General Requirements & Setup](#general-requirements--setup)
    * [Compilation](#compilation)
    * [Memory Management](#memory-management)
    * [Global Variables](#global-variables)
    * [Error Handling](#error-handling)
    * [History Navigation](#history-navigation)
3.  [Core Functionality Tests](#core-functionality-tests)
    * [Simple Command Execution](#simple-command-execution)
    * [Arguments](#arguments)
    * [Return Value (`$?`)](#return-value--)
    * [Environment Variables (`$`)](#environment-variables--)
    * [Path Resolution (`$PATH`)](#path-resolution-path)
4.  [Quoting Tests](#quoting-tests)
    * [Double Quotes (`"`)](#double-quotes--)
    * [Single Quotes (`'`)](#single-quotes--)
    * [Mixed Quoting](#mixed-quoting)
5.  [Built-in Command Tests](#built-in-command-tests)
    * [`echo`](#echo)
    * [`cd`](#cd)
    * [`pwd`](#pwd)
    * [`env`](#env)
    * [`export`](#export)
    * [`unset`](#unset)
    * [`exit`](#exit)
6.  [Redirection Tests](#redirection-tests)
    * [Input Redirection (`<`)](#input-redirection--)
    * [Output Redirection (`>`)](#output-redirection--)
    * [Append Redirection (`>>`)](#append-redirection--)
    * [Heredoc Redirection (`<<`)](#heredoc-redirection--)
    * [Multiple Redirections](#multiple-redirections)
    * [Redirections with Permissions/Non-existent Files](#redirections-with-permissionsnon-existent-files)
7.  [Pipe Tests](#pipe-tests)
    * [Single Pipes](#single-pipes)
    * [Multiple Pipes](#multiple-pipes)
    * [Pipes with Built-ins](#pipes-with-built-ins)
    * [Pipes with Redirections](#pipes-with-redirections)
    * [Pipe Error Handling](#pipe-error-handling)
8.  [Signal Handling Tests](#signal-handling-tests)
    * [`Ctrl+C` (SIGINT)](#ctrlc-sigint)
    * [`Ctrl+D` (EOF)](#ctrld-eof)
    * [`Ctrl+\\` (SIGQUIT)](#ctrl-sigquit)
9.  [Special & Edge Case Tests](#special--edge-case-tests)
    * [Empty/Whitespace Commands](#emptywhitespace-commands)
    * [Long Command Lines](#long-command-lines)
    * [Non-existent Commands](#non-existent-commands)
    * [Complex Combinations](#complex-combinations)
    * [Torture Tests (`testsdesenfer` inspiration)](#torture-tests-testsdesenfer-inspiration)
10. [Bonus Feature Tests (If Implemented)](#bonus-feature-tests-if-implemented)
    * [Logical Operators (`&&`, `||`)](#logical-operators--)
    * [Parentheses](#parentheses)
    * [Wildcards (`*`)](#wildcards--)
11. [Testing Tools](#testing-tools)
    * [Valgrind](#valgrind)
    * [Automated Testers](#automated-testers)

---

## 1. Introduction

The Minishell project at 42 aims to build a basic command-line interpreter similar to `bash`. This document serves as a guide to rigorously test your Minishell implementation, covering mandatory features, edge cases, and potential bonus functionalities.

## 2. General Requirements & Setup

### Compilation

* **Flags:** Your `Makefile` must compile with `-Wall -Wextra -Werror`.
* **No Relinking:** Ensure your `Makefile` does not re-link unnecessarily.
* **No Compilation Errors:** The project must compile cleanly without any warnings or errors.

### Memory Management

* **Zero Leaks:** Your Minishell must have **zero memory leaks**. This is a critical point.
    * Use `valgrind` extensively. Example command for general leak checking:
        ```bash
        valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=readline.supp ./minishell
        ```
        (You may need a `readline.supp` file for known `readline` leaks, often provided by other students or found in shared resources.)
    * Test long sessions, complex pipelines, repeated commands, and error scenarios for leaks.

### Global Variables

* **Strict Limitation:** You are generally allowed only **one global variable**, and its sole purpose should be to store information about received signals (e.g., a flag for `SIGINT`). Be prepared to justify its existence.

### Error Handling

* **Clear Messages:** For invalid inputs, syntax errors, or command failures, your shell should print clear and informative error messages (e.g., `command not found`, `permission denied`, `syntax error`).
* **Non-Crashing:** Under no circumstances should your shell crash (segmentation fault, bus error, etc.). Any crash leads to an immediate 0 on evaluation.

### History Navigation

* **Arrow Keys:** Up and Down arrow keys should allow navigation through previously entered commands.

## 3. Core Functionality Tests

### Simple Command Execution

* Execute commands with **absolute paths**:
    ```bash
    /bin/ls
    /usr/bin/wc
    /bin/cat /etc/hosts
    ```
* Execute commands with **relative paths**:
    ```bash
    ./my_script.sh
    ../test_dir/my_other_script.sh
    ```
* Execute commands found in `$PATH` (no explicit path):
    ```bash
    ls
    cat /etc/hosts
    grep root /etc/passwd
    ```

### Arguments

* Simple commands with arguments:
    ```bash
    ls -l
    echo hello world
    cat file1 file2
    ```
* Commands with special characters as arguments (handled by quotes):
    ```bash
    echo "hello   world"
    ls "-l"
    ```

### Return Value (`$?`)

* Check the exit status of successful commands:
    ```bash
    /bin/ls
    echo $?  # Should be 0
    ```
* Check the exit status of failed commands (e.g., command not found, permission denied, non-existent files):
    ```bash
    non_existent_command
    echo $? # Should be 127
    /bin/ls non_existent_file
    echo $? # Should be 1 or 2 (depending on `ls` exit code for no such file)
    ```
* Complex scenarios with `$?`:
    ```bash
    ls > /dev/null; echo $?
    non_existent_cmd; echo $? + $?
    ```

### Environment Variables (`$`)

* **Expansion:** Test basic variable expansion:
    ```bash
    echo $USER
    echo $HOME
    echo $PATH
    echo $PWD
    ```
* **Unset Variables:**
    ```bash
    echo $NON_EXISTENT_VAR  # Should print nothing or an empty line
    ```
* **Partial Expansion:**
    ```bash
    echo hello$USERworld
    echo $U$SER
    ```

### Path Resolution (`$PATH`)

* **Default `PATH`:** Ensure commands like `ls`, `cat`, `grep` work without full paths.
* **Unsetting `PATH`:**
    ```bash
    unset PATH
    ls # Should now fail: "command not found"
    /bin/ls # Should still work (absolute path)
    ```
* **Modifying `PATH`:**
    ```bash
    export PATH=/bin:/usr/bin
    ls # Works
    export PATH=/tmp
    ls # Should fail if /tmp doesn't contain ls
    export PATH=/bin:/usr/local/bin:/usr/bin
    # Test precedence: if 'ls' exists in both /bin and /usr/local/bin, which one is executed?
    ```
* **Empty `PATH` component:** `export PATH=::/bin` (should still find commands in `/bin`).

## 4. Quoting Tests

### Double Quotes (`"`)

* **Protect spaces/special chars:**
    ```bash
    echo "hello   world"
    ls -l "file with spaces" # If such a file exists
    ```
* **Allow variable expansion:**
    ```bash
    echo "User is $USER"
    echo "Current dir: $PWD"
    echo "Exit status: $?"
    ```
* **Escape `$` (not usually required for minishell, but good to check):** `echo "this is \$ not a var"` (should print `this is $ not a var`)
* **Unclosed quotes:** `echo "hello` (should prompt for more input or handle as syntax error)

### Single Quotes (`'`)

* **Literal interpretation:** Everything inside single quotes should be treated literally. No variable expansion, no special character interpretation.
    ```bash
    echo '$USER'       # Should print $USER
    echo 'hello world' # Should print hello world
    echo 'Path: $PATH' # Should print Path: $PATH
    echo 'hello | cat > file' # Should print hello | cat > file
    ```
* **Unclosed quotes:** `echo 'hello` (should prompt for more input or handle as syntax error)

### Mixed Quoting

* Combinations of single and double quotes:
    ```bash
    echo "'$USER'"      # Expected: '$USER' (single quotes protect `$` but are themselves literal)
    echo '"$USER"'      # Expected: "username" (double quotes allow `$` expansion)
    echo "hello '$USER' world"
    echo 'hello "$USER" world'
    echo "a'b\"c'd"
    ```

## 5. Built-in Command Tests

### `echo`

* Basic `echo`:
    ```bash
    echo
    echo hello
    echo "hello world"
    echo hello   world # Multiple spaces should collapse to one
    ```
* `echo -n` option:
    ```bash
    echo -n hello # No newline
    echo -n hello world
    echo hello -n world # -n should be treated as argument if not first or after specific options
    echo -n -n -n hello
    ```
* `echo` with variable expansion:
    ```bash
    echo $USER
    echo "$USER"
    echo '$USER'
    echo "Hello $USER, your PWD is $PWD"
    ```

### `cd`

* **Absolute paths:**
    ```bash
    cd /
    pwd # Should be /
    cd /usr/bin
    pwd # Should be /usr/bin
    ```
* **Relative paths:**
    ```bash
    cd ..
    cd .
    cd ../../
    ```
* **No arguments:** `cd` (should go to `$HOME`). Verify by `pwd`.
* **Non-existent directory:**
    ```bash
    cd non_existent_dir # Should print an error and not change directory
    ```
* **Permissions issues:** `cd /root` (should fail with permission denied).
* **Symbolic links:** `cd /var/log` (often a symlink, check behavior).

### `pwd`

* Basic `pwd`: `pwd`
* After `cd` commands: Always verify `pwd` after `cd` operations.

### `env`

* **Display:** `env` (should display current environment variables).
* **Check variables after `export`/`unset`:**
    ```bash
    export MY_VAR=hello
    env | grep MY_VAR # Should show MY_VAR=hello
    unset MY_VAR
    env | grep MY_VAR # Should show nothing
    ```
* **Empty environment:** If launched with `env -i ./minishell`, `env` should show only new exports.

### `export`

* **New variable:**
    ```bash
    export MY_VAR=value
    echo $MY_VAR # Should print value
    env | grep MY_VAR # Should find it
    ```
* **Overwrite variable:**
    ```bash
    export MY_VAR=value1
    export MY_VAR=value2
    echo $MY_VAR # Should print value2
    ```
* **Variable without value:** `export MY_VAR` (should add `MY_VAR` to `env` without a value).
* **Invalid identifier:** `export 1VAR=value` (should print error).
* **Multiple variables:** `export VAR1=val1 VAR2=val2`
* **`export` without arguments:** Should print all exported variables (like `declare -x` in bash).
* **Quotes in value:** `export VAR="hello world"`

### `unset`

* **Existing variable:**
    ```bash
    export MY_VAR=hello
    unset MY_VAR
    echo $MY_VAR # Should print nothing
    env | grep MY_VAR # Should not find it
    ```
* **Non-existent variable:** `unset NON_EXISTENT_VAR` (should not cause error).
* **Invalid identifier:** `unset 1VAR` (should print error).
* **Multiple variables:** `unset VAR1 VAR2`

### `exit`

* **Basic exit:** `exit` (should exit the shell with status 0).
* **Exit with status:** `exit 42` (should exit with status 42).
* **Multiple arguments:** `exit 1 2` (should print error and not exit).
* **Non-numeric argument:** `exit hello` (should print error and exit).
* **Exit in a pipe:** `echo hello | exit` (behaviour might vary, often `exit` in a child process doesn't affect parent).
* **Exit with maximum/minimum values:** `exit 255`, `exit 256` (should be 0), `exit -1` (should be 255).

## 6. Redirection Tests

Create temporary files (e.g., `test_file.txt`, `output.txt`) for these tests.

### Input Redirection (`<`)

* **Basic:** `cat < test_file.txt`
* **Non-existent file:** `cat < non_existent.txt` (should print error).
* **Permissions:** `cat < /dev/null` (works), `cat < /etc/shadow` (should fail with permission denied).

### Output Redirection (`>`)

* **Basic:** `echo hello > output.txt` (creates/overwrites).
* **Check content:** `cat output.txt`
* **Overwrite existing file:** `echo new content > output.txt` (check content).
* **Permissions:** `echo hello > /root/protected.txt` (should fail).

### Append Redirection (`>>`)

* **Basic:** `echo hello >> append.txt` (creates/appends).
* **Append to existing:** `echo world >> append.txt` (check `cat append.txt`).
* **Permissions:** `echo hello >> /root/protected.txt` (should fail).

### Heredoc Redirection (`<<`)

* **Basic heredoc:**
    ```bash
    cat << EOF
    hello
    world
    EOF
    ```
* **Multiline input.**
* **Empty delimiter:** `cat << ""`
* **Heredoc with spaces in delimiter:** `cat << "    EOF   "`
* **No final delimiter:** Minishell should wait for input indefinitely until EOF (Ctrl+D).
* **Variable expansion in heredoc (usually not required):** `cat << EOF` then `$USER`
* **Heredoc after command:** `ls << EOF` (should ignore heredoc and run `ls`).

### Multiple Redirections

* **Multiple inputs (last one applies):** `cat < file1 < file2` (should read from `file2`).
* **Multiple outputs (last one applies):** `echo hello > file1 > file2` (only `file2` should contain "hello").
* **Mixed types:**
    ```bash
    cat < file1 > file2
    grep "a" < file1 >> file2
    ls > file1 < file2 # Order should not matter for file opening
    ```

### Redirections with Permissions/Non-existent Files

* Test combinations of invalid file paths or permissions with different redirection types. Ensure appropriate error messages and `$?` values.
    ```bash
    cat < non_existent.txt > output.txt # Should fail at input, no output created
    echo hello > /dev/full # Test full disk or similar error
    ```

## 7. Pipe Tests

Create `file1.txt` with content for `cat` etc.

### Single Pipes

* **Basic:** `ls | cat`
* **Common usage:** `cat file1.txt | grep "keyword"`
* **Pipe to built-in:** `ls | grep "d"`
* **Built-in to pipe:** `echo hello | wc -c`
* **Long commands in pipe:** `ls -lR / | grep "minishell" | head -n 5`

### Multiple Pipes

* **Three or more commands:**
    ```bash
    ls -l | grep "file" | wc -l
    cat file1.txt | head -n 1 | cut -d' ' -f1
    ```

### Pipes with Built-ins

* **Built-in as first, middle, or last command:**
    ```bash
    echo hello | cat
    cat | echo hello | cat # Not practical but tests pipe flow
    cat file.txt | grep "a" | wc -l
    ```
* **`export` / `unset` in pipe (should not affect parent shell):**
    ```bash
    export VAR=ORIGINAL
    echo hello | export VAR=CHANGED_IN_CHILD # VAR should remain ORIGINAL in parent
    echo $VAR # Check in parent shell
    ```
    Similarly for `cd` in a pipe.

### Pipes with Redirections

* **Input/Output on first command:** `cat < file1.txt | grep "a"`
* **Input/Output on last command:** `grep "a" file1.txt | cat > output.txt`
* **Input/Output on middle command (less common but possible):**
    ```bash
    cat file1.txt | grep "a" > temp.txt | wc -l # grep output goes to temp.txt, wc gets nothing
    cat file1.txt | (grep "a" > temp.txt) | wc -l # This involves subshells, not typical for Minishell
    ```
    Focus on redirections at the ends of pipes for typical Minishell scope.
* **Combinations:**
    ```bash
    < infile grep pattern | cat > outfile
    cmd1 | cmd2 >> outfile < infile
    echo "test" | cat << EOF > output.txt
    hello
    EOF
    ```

### Pipe Error Handling

* **Non-existent command in pipe:** `ls | non_existent_cmd | cat` (minishell should continue with next valid command if possible, otherwise exit pipe).
* **Permission denied in pipe:** `cat /root/secret.txt | grep "a"`
* **Broken pipes:** What happens if a command in the middle exits early? `yes | head -n 5 | tail -n 1`

## 8. Signal Handling Tests

### `Ctrl+C` (SIGINT)

* **Empty prompt:** Type `Ctrl+C`.
    * Expected: New line, new prompt, no previous input.
* **Prompt with typed content:** Type `some command`, then `Ctrl+C`.
    * Expected: New line, new prompt, buffer cleared (pressing Enter afterwards should do nothing).
* **During a blocking command:** Run `cat` (no arguments, waiting for input) or `sleep 100`, then `Ctrl+C`.
    * Expected: Blocking command terminates, new line, new prompt.
* **During a non-blocking command:** Run `ls`, then `Ctrl+C` quickly.
    * Expected: `ls` completes or is interrupted, new line, new prompt.
* **In a pipe:** `cat | cat` then `Ctrl+C`.
    * Expected: Both `cat` processes should terminate.

### `Ctrl+D` (EOF)

* **Empty prompt:** Type `Ctrl+D`.
    * Expected: Minishell exits.
* **Prompt with typed content:** Type `some command`, then `Ctrl+D`.
    * Expected: Do nothing. Continue prompting.
* **During a blocking command:** Run `cat` (no arguments, waiting for input), then `Ctrl+D`.
    * Expected: `cat` receives EOF and terminates normally.
* **In a heredoc:** Provide input to a heredoc, then `Ctrl+D` instead of delimiter.
    * Expected: Heredoc terminates.

### `Ctrl+\\` (SIGQUIT)

* **All scenarios (empty prompt, typed content, blocking command):** Type `Ctrl+\\`.
    * Expected: Do nothing (should not print "Quit: 3" like bash).
    * If a child process is running, `Ctrl+\\` might terminate it and show "Quit: 3". Your shell should prevent this for its own process but typically allows child processes to dump core if they are not explicitly catching `SIGQUIT`. The 42 subject often implies your *shell itself* shouldn't print anything.

## 9. Special & Edge Case Tests

### Empty/Whitespace Commands

* Just press Enter.
* Type spaces then Enter: `      `
* Type tabs then Enter: `\t\t\t`
* Type mixed spaces/tabs then Enter.

### Long Command Lines

* Type a very long command with many arguments.
* Pipe a very long string from `echo` to `cat`.

### Non-existent Commands

* `gibberish`
* `ls non_existent_file` (check error message and `$?`)

### Complex Combinations

* Mixing everything: `echo "hello $USER" | grep "l" > output.txt << EOF && cat output.txt` (if bonus is implemented)
* **Permissions:** Try executing files without execute permissions, or writing to directories without write permissions.
    ```bash
    chmod -x my_script.sh
    ./my_script.sh # Should fail with permission denied
    mkdir no_write_dir
    chmod -w no_write_dir
    echo hello > no_write_dir/file.txt # Should fail
    ```

### Torture Tests (`testsdesenfer` inspiration)

* **Random input:**
    ```bash
    cat /dev/urandom | head -c 10000 | ./minishell
    # OR
    valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=readline.supp ./minishell 2> outerror < /dev/urandom
    ```
    This can expose issues with unexpected character handling, parsing, or memory. Be ready for "weird files" or errors.
* **Many empty pipes:** `cat ||| cat` or `| | |` (syntax errors).
* **Many redirections:** `ls > a > b > c > d`
* **Unclosed quotes/pipes/redirections:** What happens if the user presses enter on `echo "` or `ls |` or `cat <`? Your shell should either prompt for more input or return a syntax error.

## 10. Bonus Feature Tests (If Implemented)

### Logical Operators (`&&`, `||`)

* **`cmd1 && cmd2`:** `ls && echo "success"` (cmd2 runs only if cmd1 succeeds)
* **`cmd1 || cmd2`:** `false || echo "fallback"` (cmd2 runs only if cmd1 fails)
* **Combinations:** `cmd1 && cmd2 || cmd3`, `(cmd1 || cmd2) && cmd3` (requires parentheses).
* **Nested:** `ls && (echo hello || false)`

### Parentheses

* **Grouping commands:** `(ls -l && pwd)`
* **Pipes with parentheses:** `(ls -l | grep "a") && echo "Found"`
* **Redirections with parentheses:** `(echo hello) > file.txt`

### Wildcards (`*`)

* **File matching:** `ls *.txt`, `cat *.c`
* **No matches:** `ls *.nonexistent` (should display `*.nonexistent` literally, not error).
* **Complex patterns:** `ls ??.txt`, `ls [abc]*.c` (depending on complexity required).
* **Wildcard in arguments:** `echo *`, `grep "pattern" *`

## 11. Testing Tools

### Valgrind

* Essential for memory leak detection and other memory errors.
* Run your Minishell with Valgrind during various test scenarios, especially those involving `fork`, `execve`, pipes, and redirections, as these are common sources of leaks.

### Automated Testers

While manual testing is crucial for understanding behavior and finding subtle bugs, several automated testers exist to speed up the process and cover common scenarios. Some popular ones include:

* **`zstenger93/42_minishell_tester`**: A comprehensive tester with mandatory, bonus, Valgrind, and specific category tests.
    * [https://github.com/zstenger93/42_minishell_tester](https://github.com/zstenger93/42_minishell_tester)
* **`Liammmmmmmm/42-Minishell` (specifically `testsdesenfer`):** Contains "torture tests" for extreme edge cases.
    * [https://github.com/Liammmmmmmm/42-Minishell](https://github.com/Liammmmmmmm/42-Minishell) (Look for `testsdesenfer` file)

Remember that automated testers are a good starting point, but they cannot replace thorough manual testing and understanding your code's behavior under various conditions. Always try to "break" your own shell!

---
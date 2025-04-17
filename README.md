# C Shell

A modular Unix-like shell implemented in C, replicating and extending core Bash functionalities. This project is designed for maintainability, clarity, and extensibility, following strict modular coding practices.

---

## Features

- **Dynamic Prompt:**  
  Displays `<Username@SystemName:~>` with the current directory. The shell's home directory is shown as `~`; absolute paths are shown when outside home.

- **Command Parsing:**  
  Supports multiple commands separated by `;` or `&`, with robust handling of spaces and tabs. Background execution (`&`) supported for system commands.

- **System Command Execution:**  
  Run any Bash system command (e.g., `vim`, `sleep`) in foreground or background.

- **I/O Redirection and Piping:**  
  Full support for `<`, `>`, `>>` redirection and any number of pipes (`|`). Redirection and pipes can be combined.

- **Session Management:**  
  Handles `Ctrl-C`, `Ctrl-D`, `Ctrl-Z` for process/job control and shell exit.

- **Custom Scripting:**  
  Supports `.myshrc` for aliases and simple functions.

- **Color Coding:**  
  Prompts, errors, and file listings use color coding for clarity (e.g., green for executables, blue for directories, red for errors).

---

## Built-in Commands

| Command      | Description                                                                                                     |
|--------------|-----------------------------------------------------------------------------------------------------------------|
| `hop`        | Change directory. Supports `.`, `..`, `~`, `-` flags; prints new path.                                          |
| `reveal`     | List files/directories. Flags: `-a` (all), `-l` (long), supports path symbols, color-coded output.              |
| `log`        | View command history (max 15), purge history, or execute by index.                                              |
| `proclore`   | Show process info for given PID or current shell.                                                               |
| `seek`       | Search recursively for files/dirs with filtering flags `-d`, `-f`, `-e`.                                        |
| `activities` | List all shell-spawned processes (PID, command, state).                                                         |
| `ping`       | Send signals to processes by PID and signal number.                                                             |
| `fg`/`bg`    | Foreground/background process control.                                                                          |
| `neonate`    | Print most recently created process PID every N seconds until 'x' is pressed.                           |
| `iMan`       | Fetch online man pages for commands using sockets from http://man.he.net/.                                      |

---

## System Commands

- Run any system command (e.g., `vim`, `sleep`, `ls`) in foreground or background.
- Foreground commands display elapsed time if >2 seconds.
- Background commands print PID; completion is notified autonomously with process name and exit status.

---

## I/O Redirection and Pipes

- **Redirection:**  
- `>`: Output to file (overwrite)
- `>>`: Output to file (append)
- `<`: Input from file  
- Error if input file not found; output files created with 0644 permissions.

- **Pipes:**  
- Chain commands using `|`; supports any number of pipes.
- Error if pipe used incorrectly (e.g., missing command on either side).

- **Redirection + Pipes:**  
- Combine redirection and pipes in any order (but not multiple inputs/outputs).

## Aliases:  
Define single-word aliases for commands.


---

## Signals, Job Control, and Activities

- **activities:**  
List all shell-spawned processes with PID, command name, and state (Running/Stopped).

- **ping <pid> <signal_number>:**  
Send signal to process (signal number modulo 32). Error if process not found.

- **fg <pid>:**  
Bring background/stopped process to foreground.

- **bg <pid>:**  
Resume stopped background process.

- **Keyboard Shortcuts:**  
- `Ctrl-C`: Interrupt foreground process (SIGINT)
- `Ctrl-D`: Exit shell after killing all processes
- `Ctrl-Z`: Push foreground process to background (Stopped)

---

## Networking (iMan)

- **iMan <command_name>:**  
Fetches man page for the command from http://man.he.net/ using sockets. Prints HTML response if page not found.

---


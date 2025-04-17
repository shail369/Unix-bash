# Part A

## Display Requirements

### In case there are multiple commands whose execution time is more than 2 seconds than I will display the latest of those commands in prompt.

- Displays the username and system name.
- Shows the current directory, replacing the home directory with `~`.
- Includes the last executed command and its execution time (if applicable).
- Supports handling directories within and outside the home directory.

#### `void get_details();`

- This function is used to get the current directory as well as systemname and username

#### `void get_prompt();`

- This function in used to print the prompt similar to original terminal, by also checking if the path to current directory is through home directory or not.
- Also it checks if the last command tooks more than 2 seconds to execute, if yes also mention that command and its duration in the prompt.

## Input Requirement

### Any command consisting of two successive `&` will give error and not perform any further commands.
### If a `&` is the first character of command it will give an error and not perform any further command.

- The input command string is first split into commands based on semicolons (`;`).
- Each semicolon-separated command is stored in the `semicolon_commands` array.
- Spaces are appended to each semicolon-separated command to facilitate further parsing.
- These commands are stored in the `semicolon_commands_with_spaces` array.
- Each semicolon-separated command is further divided based on ampersands (`&`).
- The resulting ampersand-separated commands are stored in the `and_commands` array.
- The `counter_and` array tracks the number of ampersand-separated commands within each semicolon-separated command.
- Each ampersand-separated command is split into individual commands based on spaces.
- The individual commands are stored in the `individual_commands` array.
- The `counter_individual` array tracks the number of space-separated commands within each ampersand-separated command.
- The function checks for and skips any commands that consist solely of spaces.

#### `int multiple();`

- This function completes all these steps.

## Hop

### If the first hop command is `hop -`, it will not work as previous directory is not defined.
### An absolute path begins with `/`.
### A relative path begins with `.` or just normally.
### A path relative from home directory begins with `~`.

- `~`: Switch to the home directory.
- `..`: Move to the parent directory.
- `-`: Switch back to the previous directory.
- `.`: Remain in the current directory.
- Custom paths: Handle absolute and relative paths, expanding `~` for the home directory.
- The function checks if the target directory is accessible (`X_OK` permission).

#### `int hop();`

- This function is used to detect the directory and to change current directory.

## Reveal

### If we provide with a file as the path then it will only give the details of that file.

**Directory Listing**: Lists all files and directories within a specified path.
- **Hidden Files**: With the `-a` flag, hidden files (starting with `.`) are included in the listing.
- **Detailed Information**: The `-l` flag displays detailed information for each file, including permissions, links, owner, group, size, and last modification time.
- **Color Coding**: Directories are shown in blue, executable files in green, and regular files in white.
- **Custom Path Handling**: Supports custom path handling for special paths. It also supportss all the path similar to hop.

#### `void print_permissions();` 

- It is used to print all the permissions of the file.

#### `char *colour();`

- It is used to find suitable colour for the file/directory.

#### `int get_path();`

- It is used to find the location of the direction where we have to call the reveal function

#### `int all_details();`
- It is used to print all the details of that file. Called when flag l is true.

#### `int reveal();`

- It is used to find all the files in that directory aand print them.

## Log

### If I give command log execute than the command will again get added in the history unless it is log execute 1.

- Automatically stores valid and unique commands in a file named `history` located in the home directory. The history can store up to 15 commands.
- L0g: Lists all the commands stored in the `history` file.
- Log Purge: Clears all the commands from the `history` file.
- Log Execute: Executes a command from the history based on its index.

#### `int existing();`

- Returns the number of commands read from the file.

#### `int store();`

- Stores a command in the `history` file if it is not a duplicate of the last command.
- Manages the command history to ensure only the last 15 commands are stored.

#### `int history();`

- Prints the list of commands stored in the `history` file.

#### `int clear();`

- Clears the `history` file by writing an empty file.

#### `int execute();`

- Executes a command from the history based on the provided index.

## ForeGround BackGround

- Create a fork to make the child run in the background while parent runs in foreground.
- Create another fork in child, where grandchild executes the command and child waits for grandchild to finish denoting whether process ender normally or abnormally.
- The group process ID of grandchild is changed to make it run independently of the parent.

## Proclore

 - Reads `/proc/<pid>/status` to extract the process state (e.g., Running, Sleeping).
- Checks if the process is in the foreground using `getpgid()` and `tcgetpgrp()`.
- Retrieves the process group ID using `getpgid()`.
- Reads `/proc/<pid>/statm` to get the virtual memory usage, which is then converted to kilobytes.
- Reads the symbolic link `/proc/<pid>/exe` to find the path of the executable. If the executable path starts with the user's home directory, it shows the path relative to the home directory.

#### `int proclore();`

- IT is used to print all the details relaated to that process.

## Seek

- Can search for directories, files, or both based on the flags provided.
- Supports searching within a specified directory or the current directory.
- `-d`: Search only for directories.
- `-f`: Search only for files.
- `-e`: Execute the file or change to the directory if exactly one match is found.
- Directories are printed in blue.
- If exactly one file is found and the `-e` flag is used, the file will be executed if it has execute permissions. If it is a script, it will be displayed.
- If exactly one directory is found and the `-e` flag is used, the current directory will be changed to the found directory.- Files are printed in green.

#### `int seek();`

- It is used find all the files and directories used prexic match the given input in the given and all the subsequent files.

#### `int special();`

- It is used to handle the case for e flag, i.e either change current directory or print the contentss of that file.

# Part B

## Myshrc

### We can't add any new alias while the code is running.
### The alias are defined as `reveala = reveal -l` not as ` alias reveala = reveal -l`
### At max 100 alias can be used (MAX_PROCESSES)

- Matches the current commmand with given aliases, if it matches with any one of them then it replaces current command with the alias it mentions.

#### `char *trim_whitespace()`

- Trims leading and trailing whitespace from a given string.

#### `void aliases()`

- Reads alias-command pairs from the configuration file (`myshrc.txt`) and stores them in the `alias` and `alias_function` arrays.

#### `int alias_replacement()`

- Replaces a command with its alias expansion.

## I/O Redirection

### There can only be one input and/or output file.

- This project implements input and output redirection handling for a custom shell, ensuring proper syntax and validation of redirection commands (`>`, `>>`, and `<`).

#### `int proper()`

- Check if the given input is correct if yes then redirects the standard input/output to the given filename.

## Pipes

### It there is back to back `|` without spaces in between then we will treat them as single pipe.
### It we are redirectings input/ output to a file wi will not consider the pipes and only use those files.
### If there is a `|` immediately followed by `&` then it will give error and not perform any further commands.
### If there is a `|` as first character or last last character then it will give error and perform anyy further commmand.

- We will create an array of pipe and redirect the output to the write end pipe and or read from thee read end of the pipe.

## Activities

#### At max we can display the status of 100 processes. (MAX_PROCESSES)

- It implements a feature to monitor and display the status of active processes in a custom shell.

#### `int activities_main()`

- Monitors and displays the status of active processes and sorted order.
- It will check the state of that process then give `Running` or `Stopped` according.

#### `void add_process()`

- Adds a new process to the list of managed processes.

#### `void remove_process()`

- Removes a given process from the list of managed processes.

#### `void stop_process()`

- Changes the state of given new process in the list of managed processes.

## Signals

- It sends signals to a process based on the process ID (PID) and signal number provided by the user.
- It implements signal handling functions to manage how the custom shell reacts to user inputs like `Ctrl+C`, `Ctrl+D`, and `Ctrl+Z`.

#### `int ping_main()`

- Sends a signal to a process using the provided PID and signal number.

#### `void ctrl_c()`

- Handles the `Ctrl+C` (SIGINT) signal by sending it to the currently running foreground process. Making sure the foreground process is not our terminal.

#### `void ctrl_z()`

- Handles the `Ctrl+Z` (SIGTSTP) signal by suspending the currently running foreground process and marking it as "Stopped".

#### `void ctrl_d()`

- Handles the `Ctrl+D` (EOF) input by terminating all running processes and exiting the shell.

## FG BG

#### `int fg_command()`

- Brings the background process with the specified PID to the foreground.

#### `int bg_command()`

- Sends the specified stopped process to the background.

## Neonate

- This module provides functionality for tracking and displaying the most recent process IDs in a Linux environment.

#### `char get_keypress()`

- Captures a single keypress from the user.

#### `pid_t find_pid()`

- Finds the most recent process ID by checking the creation time of processes in the /proc directory.

#### `void neonate()`

- Tracks and displays the most recent process ID at regular intervals until a stop condition is met.

## iMan

- This module provides functionality to retrieve and print the content of man pages from an online source.

#### `void printer()`

- Extracts and prints content from HTML text while ignoring tags and based on the specified command.

#### `int iman()`

- Retrievesthe man page for a given command from an online man page service.


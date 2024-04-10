# my-shell
# Description 
This project contains a simple shell implementation (mysh) written in C. The shell provides basic functionality such as executing commands, built-in commands like cd, pwd, which, and exit, as well as handling I/O redirection. It supports both interactive mode and batch mode execution.

# Test Plan

## Batch Plan 
1. Test Case: Execute a script file containing a single command.
    Input: Create a script file (myscript.sh) containing the command echo hello.
    Execution: Run ./mysh myscript.sh.
    Expected Output: The shell should print hello.
    Output: hello

## Interactive Mode
1. Test Case: Navigate to a subdirectory, execute a command, then navigate deeper into another subdirectory.

    Input: Run ./mysh and enter the following commands:
    cd subdir
    echo hello
    cd subsubdir
    pwd
 
    Expected Output: The shell should change directory to subdir, print hello, change directory to subsubdir, and print the current working directory, which should be /current/path/subdir/subsubdir

2. Test Case: Attempt to change to a directory that does not exist.
    Input: Run ./mysh and enter cd directory_that_does_not_exist.
    Expected Output: The shell should print an error message indicating that the directory does not exist.
    

3. Test Case: Navigate up multiple directories.
    Input: Run ./mysh and enter cd ..
    Expected Output: The shell should navigate up  in the directory structure.
    

4. Test Case: Exit the shell.
    Input: Run ./mysh and enter exit.
    Expected Output: The shell should print mysh: exiting and terminate.




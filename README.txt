 CS3013_Project1
 
=== Compiling the Program ===

To compile all files, use the command `make` or `make all`. To compile just boring.c, run `make boring`. To compile just custom.c, run `make custom`. To compile just multi.c, run `make multi`. These compile to executables called `boring`, `custom`, and `multi`, respectively.


=== Basic Commander ===

This program was tested by comparing the output given in the Project document to that outputted in the file. This program runs only three commands. First, it runs `whoami`, then `last`, then `ls -al /home`. It does this by using fork(), then using waitpid() so that the parent waits for the child before continuing.
 
=== Custom Commander ===

This program can only accept commands under 1000 character. We tested this program by trying different combinations of input. This program reads a .txt file to get the commands it needs to run. Each line of the file is a command.

The program also has two new commands: `ccd` and `cpwd`. The `ccd` command changes the working directory and the `cpwd` command prints the working directory. These functions use a variable called workingDirectory to keep track of the working directory.


=== Multi Commander ===

Our program was made to only allow 100 commands and each command must be under 1000 characters. 

Our program was tested by trying different combinations of commands and sending different tasks to be background tasks.

In a sense, we treated foreground and background jobs essentially the same. The only difference is that background process did not trigger the execution of the next command while the foreground would execute the next task once it will finish. This algorithm simplifed a lot of management for backgroumd tasks. Our code featured no blocking waitpid() calls to improve speed and simplicity.

For tracking outstanding processes in the Multi process, we created a struct called Job which contains a variable called running. While that variable is set to 1, the process is outstanding. Once the process has finished, the variable is changed to 0. The `cproclist` command will list all unfinished commands.

Our program will not finish executing until all of the processes are no longer outstanding.

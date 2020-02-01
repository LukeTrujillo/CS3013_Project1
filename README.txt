# CS3013_Project1


Multi Commander

Our program was made to only allow 100 commands and each command must be under 1000 characters. 

Our program was tested by trying different combinations of commands and sending different tasks to be background tasks.

In a sense, we treated foreground and background jobs essentially the same. The only difference is that background process did not trigger the execution of the next command while the foreground would execute the next task once it will finish. This algorithm simplifed a lot of management for backgroumd tasks. Our code featured no blocking waitpid() calls to improve speed and simplicity.

For tracking outstanding processes in the Multi process, we created a struct called Job which contains a variable called running. While that variable is set to 1, the process is outstanding. Once the process has finished, the variable is changed to 0. 

Our program will not finish executing until all of the process are no longer outstanding.

# SHELL #

## Introduction ##

The goal of this project was to understand the mechanics of a shell by implementing one. The created shell takes commands from the user, sends them to an operating system then executes them. 

## Implementation ##

Implementation of this project consisted of 5 steps:

1.  Executing simple commands on shell using fork(), exec() and wait().
2.  Installing the built in commands to the shell.
3.  Implementing redirection.
4.  Implementing piping.
5.  Error management.
6.  Implementing extra features.


#### Executing commands on shell using fork(), exec() and wait()
Starting with the prof’s skeleton command, we replaced the function system() with fork(), exec() and wait(). By forking the argument, we’ll obtain two processes (a child and a parent) where the child execute the execvp function and the parent waits for its execution. If the process succeeded, the return status would be 0 and if it fails it would return 1. This portion of the code was put in the function execution(parsed command). This function would have been more efficient if we were able to use it for piping as well (we created another execution function for piping)
#### Installing the built in commands to the shell
#### Implementing redirection
#### Implementing piping

#### Error management.
To manage errors, we decided that it was best to output the error once detected so that order wouldn’t get messed up. We used stderr as advised by the professor instead of a simple printf so that the code knows that it’s an error. Error management would have been more efficient if were able to combine all the errors into one or two functions so it would be easier to control them and change them later on. 
#### Implementing extra features
Unfortunately, we were unable to implement this part of the assignment since we couldn’t implement the variable spaces correctly. Our approach to this part was to first check whether the implemented command intends to specify one of the environment variables. Subsequently, we would create a function called [set_funtion(parsed arguments)] that accepts all the arguments then set the third argument equal to the second. The third argument would be passed to the second depending on its type where if it was a string it would be passed right away. On the other hand, if it were a $var, we would access it’s value and pass it on to the second argument.

## Conclusion ##
Undergoing this project gave us a better understanding of how the shell is implemented. We learned that we could always add more features into our shell creating a whole system. 


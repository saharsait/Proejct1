# Generating an executable from an object file
sshell: sshell.o
	gcc -Wall -Wextra -Werror -o sshell sshell.o

# Generating an object file from a C file
sshell.o: sshell.c
	gcc -Wall -Wextra -Werror -c -o sshell.o sshell.c

# Cleaning all the generated files
clean:
	rm -f sshell sshell.o

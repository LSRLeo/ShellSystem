# ShellSystem
This project is mini Linux shell in C that can execute commands, handle built-in operations like cd and exit, and support input/output redirection using &lt; and >. We also fork child processes to run external programs and support background execution with &amp;, using signal handling to clean up finished processes.


# Compile 
gcc -Wall -Wextra -o minishell minishell.c
./minishell


# BASIC TEST CASES

Basic Shell:
pwd
cd ..
cd /
exit

Program Execution:
ls -l
ps

Input & Output Redirection:
ls -l > out.txt
cat < out.txt

Background Process:
sleep 10 &
ps

history:
!3

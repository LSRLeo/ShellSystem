# ShellSystem
This project is mini Linux shell in C that can execute commands, handle built-in operations like cd and exit, and support input/output redirection using &lt; and >. We also fork child processes to run external programs and support background execution with &amp;, using signal handling to clean up finished processes.




## Compile & Run

```bash
gcc -Wall -Wextra -o minishell minishell.c
./minishell
```

---

## Test Cases

### Basic Shell
```bash
pwd
cd ..
cd /
exit
```

### Program Execution
```bash
ls -l
ps
```

### Input & Output Redirection
```bash
ls -l > out.txt
cat < out.txt
```

### Background Process
```bash
sleep 10 &
ps
```

### History
```bash
!3
```

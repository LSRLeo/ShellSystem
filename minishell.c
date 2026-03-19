#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_LINE 1024
#define MAX_ARGS 100
#define MAX_HISTORY 100

/*Collaborators: Anh Tran & Shirui Liu */

//HISTORY

char *history[MAX_HISTORY];
int history_count = 0;

void add_history(const char *cmd)
{
  if (history_count < MAX_HISTORY)
  {
    history[history_count++] = strdup(cmd);
  }
}

void print_history(void)
{
  for (int i = 0; i < history_count; i++)
  {
    printf("%d %s\n", i + 1, history[i]);
  }
}

//SIGNAL HANDLING 

void reap_children(int sig)
{
  int saved_errno = errno;
  (void)sig;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

//PARSING

int parse_line(char *line, char **args)
{
  int count = 0;
  char *token = strtok(line, " \t");

  while (token && count < MAX_ARGS - 1)
  {
    args[count++] = token;
    token = strtok(NULL, " \t");
  }

  args[count] = NULL;
  return count;
}

//IO REDIRECTION

void handle_redirection(char **args)
{
  int i = 0, j = 0;

  while (args[i] != NULL)
  {
    if (strcmp(args[i], ">") == 0)
    {
      if (args[i + 1] == NULL)
      {
        fprintf(stderr, "Missing output file\n");
        exit(EXIT_FAILURE);
      }

      if (!freopen(args[i + 1], "w", stdout))
      {
        perror("freopen");
        exit(EXIT_FAILURE);
      }

      i += 2; /* Skip both '>' and filename */
    }
    else if (strcmp(args[i], "<") == 0)
    {
      if (args[i + 1] == NULL)
      {
        fprintf(stderr, "Missing input file\n");
        exit(EXIT_FAILURE);
      }

      if (!freopen(args[i + 1], "r", stdin))
      {
        perror("freopen");
        exit(EXIT_FAILURE);
      }

      i += 2; /* Skip both '<' and filename */
    }
    else
    {
      args[j++] = args[i++]; /* Copy non-redirection args */
    }
  }

  args[j] = NULL; /* Terminate the cleaned array */
}

//MAIN LOOP

int main(void)
{
  char line[MAX_LINE];
  char *args[MAX_ARGS];
  char cwd[1024];

  signal(SIGCHLD, reap_children);

  while (1)
  {
    /* Display prompt */
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      printf("%s> ", cwd);
    }
    else
    {
      perror("getcwd");
      printf("> ");
    }
    fflush(stdout);

    /* Read input */
    if (!fgets(line, MAX_LINE, stdin))
    {
      printf("\n");
      break;
    }

    /* Remove newline */
    line[strcspn(line, "\n")] = '\0';

    if (strlen(line) == 0)
      continue;

 
    if (line[0] == '!' && history_count > 0)
    {
      int idx = atoi(&line[1]) - 1;

      if (idx < 0 || idx >= history_count)
      {
        fprintf(stderr, "Invalid history reference\n");
        continue;
      }

      strcpy(line, history[idx]);
      printf("%s\n", line);
    }
    else
    {
      
      add_history(line);/* Only add to history if it's not a history reference */
    }

    int argc = parse_line(line, args);
    if (argc == 0)
      continue;

    /* Built-in: exit */
    if (strcmp(args[0], "exit") == 0)
    {
      break;
    }

    /* Built-in: cd */
    if (strcmp(args[0], "cd") == 0)
    {
      if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
      } else if (chdir(args[1]) != 0) {
        perror("cd");
      }
      continue;
    }


    /* Built-in: history */
    if (strcmp(args[0], "history") == 0)
    {
      print_history();
      continue;
    }

    /* Background process */
    int background = 0;
    if (argc > 0 && strcmp(args[argc - 1], "&") == 0)
    {
      background = 1;
      args[argc - 1] = NULL;
    }

    pid_t pid = fork();

    if (pid == 0)
    {
      /* Child process */
      handle_redirection(args);

      execvp(args[0], args);

      fprintf(stderr, "%s: command not found\n", args[0]);
      exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
      /* Parent process */
      if (!background)
      {
        int status;
        int done = 0;

        while (!done) {
          pid_t w = waitpid(pid, &status, 0);

          if (w == pid) {
            done = 1;                 // successfully waited for that child
          } else if (w == -1) {
            if (errno == EINTR) {
              // Interrupted by signal,leave the if statemnt and just try again
            } else if (errno == ECHILD) {
              done = 1;
            } else {
              perror("waitpid");
              done = 1;
            }
          } else {
            
          }
        }

      }
    }
    else
    {
      perror("fork");
    }
  }

  /* Cleanup history */
  for (int i = 0; i < history_count; i++)
  {
    free(history[i]);
  }

  return 0;
}
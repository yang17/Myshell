#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
//#include "wshell.h"

int Mysh_cd(char **args,char **hist, int current);
int Mysh_help(char **args,char **hist, int current);
int Mysh_exit(char **args,char **hist, int current);
int Mysh_history(char **args,char **hist, int current);
int clear_history(char **args,char **hist, int current);
int lastcommand(char **args,char **hist, int current);
/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "history",
  "hc",
  "!!"
};

int (*builtin_func[]) (char **,char **, int) = {
  &Mysh_cd,
  &Mysh_help,
  &Mysh_exit,
  &Mysh_history,
  &clear_history,
  &lastcommand

};


int Mysh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int Mysh_cd(char **args,char *hist[], int current)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "Mysh: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    //chdir() changes the current working directory to that specified in path
    perror("Mysh");}
    return 1;
}
int Mysh_help(char **args,char *hist[], int current)
{
  int i;
  printf("YangMing's SHELL\n");
  printf("The following are built in:\n");

  for (i = 0; i < Mysh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  return 1;
}
int Mysh_exit(char **args,char *hist[], int current)
{
  return 0;
}

#define HISTORY_COUNT 20
int Mysh_history(char **args,char *hist[], int current)
{
        int i = current;
        int hist_num = 1;

        do {
                if (hist[i]) {
                        printf("%4d  %s\n", hist_num, hist[i]);
                        hist_num++;
                }

                i = (i + 1) % HISTORY_COUNT;

        } while (i != current);

        return 1;
}

int clear_history(char **args,char *hist[], int current)
{
        int i;
        for (i = 0; i < HISTORY_COUNT; i++) {
                free(hist[i]);
                hist[i] = NULL;
              }

        return 1;
}


int Mysh_execute(char **args,char *hist[], int current);
char **Mysh_split_line(char *line);
int lastcommand(char **args,char *hist[], int current)
{

  free(hist[current-1]);
  hist[current-1] = NULL;
  char *newline;
  newline=strdup(hist[current-2]);
  //  printf("newline:%s\n", newline);
  args = Mysh_split_line(newline);
  return Mysh_execute(args,hist,current);
}


char *Mysh_read_line(void)
 {
   char *line = NULL;
   ssize_t bufsize = 0; // have getline allocate a buffer for us
   getline(&line, &bufsize, stdin);

   //if (line[strlen(line) - 1] == '\n')
    //line[strlen(line)]-1 = '\0';

   return line;
 }

#define Mysh_TOK_BUFSIZE 64
#define Mysh_TOK_DELIM " \t\r\n\a"
char **Mysh_split_line(char *line)
{
  int bufsize = Mysh_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "Mysh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, Mysh_TOK_DELIM);
  //breaks string str into a series of tokens using the delimiter delim
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += Mysh_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "Mysh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, Mysh_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int Mysh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  //fork() creates a child process that differs from the parent process only in its PID and PPID
  if (pid == 0) {// Child process
    if (execvp(args[0], args) == -1)
    //execvp: executes the program pointed to by filename
      {
      perror("Mysh");
      //prints a descriptive error message to stderr.
      // First the string str is printed, followed by a colon then a space.
      }
      exit(EXIT_FAILURE);
  }
  else if (pid < 0) {
    // Error forking
    perror("Mysh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    // WIFEXITED:Process Completion Status
  }

  return 1;
}

int Mysh_execute(char **args, char **hist, int current)
{
  int i;
  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < Mysh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0)
    //strcmp:compare str1 with str2,这里str2等于str1，才是0.即Match到了
     {
      return (*builtin_func[i])(args,hist,current);
    }
  }

  char *str;
  str=args[0];
  char *p;
  int n;
  p = strtok(str, "!");
  n = atoi(p);
  if ( n > 0 && n < 20) {
    free(hist[current-1]);
    hist[current-1] = NULL;
    char *newline2;
    newline2=strdup(hist[n-1]);
    printf("command:%s\n",newline2);
    args = Mysh_split_line(newline2);
    return Mysh_execute(args,hist,current);
  }

  return Mysh_launch(args);
}

void Mysh_loop(void)
{
  char *line;
  char **args;
  int status;

  //char prompt[MAX_PROMPT];

  char *hist[HISTORY_COUNT];
  int i, current = 0;
  for ( i = 0; i < HISTORY_COUNT; i++) {
   hist[i]= NULL;
  }

  char buff[PATH_MAX + 1];

  do {
    printf("[YangMing]>%s  $ ",getcwd (buff,PATH_MAX + 1 ));

    //type_prompt(prompt);

    line = Mysh_read_line();

    free(hist[current]);
    hist[current]=strdup(line);
    current = (current + 1) % HISTORY_COUNT;

    args = Mysh_split_line(line);
    status = Mysh_execute(args, hist, current);

    free(line);
    free(args);
  } while (status);
}






int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  Mysh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

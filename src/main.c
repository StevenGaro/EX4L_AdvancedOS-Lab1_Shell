/***************************************************************************//**

  @file         main.c

  @author       SG, CC, MC, CK, JRN
  
  @inspiration  Stephen Brennan LSH (Libstephen SHell)

  @year			2017/2018

  @brief        EX4l-Shell

*******************************************************************************/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "openlib/openlib.h"

/*
  Function Declarations for builtin shell commands:
 */
int help(char **args);
int gcckn_exit(char **args);
char **lsh_split_line(char *line);
char *lsh_read_line(void);
int lsh_execute(char **args);
int lsh_launch(char **args);


/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cr",
  "help",
  "exit",
  "op"
};

int (*builtin_func[]) (char **) = {
  &cr,
  &help,
  &gcckn_exit,
  &op
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
	char cwd[1024];
  char *line;
  char **args;
  int status;
  // Load config files, if any.

  // Run command loop.
  do {
	//Command prompt
	if(getcwd(cwd, sizeof(cwd)) != NULL)  //get current directory
	{
		printf("%-s ~ ",cwd);
	}
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
  
  /*if(argc == 3){
    return(cr(args));
  }*/
  //printf("\n [%d]\n", argc);
  
  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
//refaire ca XX
char *lsh_read_line(void)
{
  char *buffer = malloc(200); // alouer de l espace mémoire au buffer
  int position = 0; // buffer position
  int c;
  while (1) {
    // Read a character
    c = getchar(); // get the first character of the line

    if (c == EOF) {
      exit(EXIT_SUCCESS); // exit if End Of File is reached
    } 
    else if (c == '\n') {
      buffer[position] = '\0'; // return the buffer once we are at the end of the line (toute la ligne se trouve lettre par lettre dans le buffer
      return buffer;
    } 
    else {
      buffer[position] = c; //ajouter une lettrre au buffer
    }
    position++;
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a'"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));  // malloc sert à demander de l espace mémoire au systeme d exploitation
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int help(char **args)
{
  printf("\n*******************************************\n");
  printf("\nGCCKN SHell based on Stephen Brennan's LSH\n");
  printf("\n___________________________________________\n");
  printf("\nProtoype shell used to simulate the command open and its extensions.\n");
  printf("The following commands are built in:\n");
  printf("\n\t1. help\n");
  printf("\n\t2. op (/url)\n");
  printf("\n\t3. cr (/url/name or name)\n");
  printf("\nThe following are Open's options:\n");
  printf("\n\t1. op -s\n");
  printf("\n\t2. op -m\n");
  printf("\n\t3. op -p\n");
  printf("\nType 'op -man' or 'cr -man' for details on options.\n\n");
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int gcckn_exit(char **args)
{
  return 0;
}

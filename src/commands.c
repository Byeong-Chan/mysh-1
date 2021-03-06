#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "commands.h"
#include "built_in.h"

static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
};

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; // Not found
}

/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */
int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    } else {
      int possible = access((com->argv)[0], 0);
      if(possible == 0 && strcmp((com->argv)[com->argc - 1], "&") != 0) {
        int status;
        pid_t pid = fork();
        if(pid == 0) {
          printf("Excution : %s\n",(com->argv)[0]);
          execv((com->argv)[0], com->argv);
        }
        else {
          wait(&status);
        }
      }
      else if(possible == 0 && strcmp((com->argv)[com->argc - 1], "&") == 0) {
        if(bg_pid != 0) {
          int status;
          int p = waitpid(bg_pid, &status, WNOHANG);
          if(p == 0) {
            fprintf(stderr,"another process is excuting.\n");
            return 0;
          }
        }

        pid_t pid = fork();
        if(pid == 0) {
          printf("%d\n",getpid());

          free((com->argv)[com->argc - 1]);
          (com->argv)[com->argc - 1] = NULL;
          
          execv((com->argv)[0], com->argv);
        }
        else {
          bg_argc = com->argc - 1;
          bg_argv = (char **)malloc(sizeof(char *) * (com->argc - 1));
          for(int i = 0; i < com->argc - 1; i++) {
            int len = strlen((com->argv)[i]);
            bg_argv[i] = (char *)malloc(sizeof(char) * (len + 1));
            strcpy(bg_argv[i], (com->argv)[i]);
          }
          bg_pid = pid;
        } 
      }
      else
        fprintf(stderr, "%s: command not found\n", com->argv[0]);
      return -1;
    }
  }

  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      if(argv[j] == NULL) continue;
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}

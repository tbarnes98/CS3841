/*************************************
 *
 * Class: CS3840 - 021
 *
 * Name(s): Trevor Barnes
 *
 * Lab: Teeny Tiny Shell
 *
 *************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define INPUT_MAX 256
#define CMD_MAX 5
#define ARG_MAX 10

/* read_cmd_string()
 *     Reads a line of text from the user
 *         Parameters:  dest - target location for the input
 *         Returns: int - 0 on success or
 *                        -1 on error
 */
int read_cmd_string(char dest[INPUT_MAX])
{
    // Read user input
    if (fgets(dest, INPUT_MAX, stdin) == NULL)
    {
        fprintf(stderr, "Unable to read user input\n");
        return -1;
    }

    // Remove trailing return character
    int len = strlen(dest);
    if (dest[len - 1] == '\n')
    {
        dest[len - 1] = '\0';
    }

    return 0;
}

/* parse_commands()
 *     Reads a line of text from the user
 *         Parameters:  input - string containing user input
 *                      cmd_strs - the target array for command strings
 *         Returns: int - The number of commands found or
 *                        -1 on error
 */
int parse_commands(char input[INPUT_MAX], char cmd_strs[CMD_MAX][INPUT_MAX])
{
    // Chop the input into command strings
    int cmd_count = 0;
    char *cmd_ptr = strtok(input, ";");
    while (cmd_ptr)
    {
        if (cmd_count >= CMD_MAX)
        {
            fprintf(stderr, "\nToo many commands\n");
            return -1;
        }
        strncpy(cmd_strs[cmd_count], cmd_ptr, INPUT_MAX);
        cmd_count++;
        cmd_ptr = strtok(NULL, ";");
    }

    return cmd_count;
}

int main()
{
    char user_input[INPUT_MAX];
    char cmd_strs[CMD_MAX][INPUT_MAX];

    int quit_flag = 0;

    // Get user input in a loop
    while(quit_flag == 0) {

        // Print the input prompt
        printf("$> ");

        // Read user input
        if (read_cmd_string(user_input) == -1)
        {
            return 1;
        }

        // Set quit flag
        if (strcmp(user_input, "quit") == 0){
            printf("Quitting...\n");
            quit_flag = 1;
        }


        // Chop the input into command strings
        int cmd_count = parse_commands(user_input, cmd_strs);
        if (cmd_count == -1)
        {
            return 1;
        }

        // Chop the commands into arguments and execute one at a time
        if(!quit_flag) {
            for (int i = 0; i < cmd_count; i++) {
                char **args = malloc(ARG_MAX*sizeof(char *));
                char* str = malloc(10*sizeof(char));
                strncpy(str, strtok(cmd_strs[i], " "), 10);
                int j = 0;
                while(str != NULL) {
                    args[j] = malloc(10*sizeof(char));
                    strcpy(args[j], str);
                    str = strtok(NULL, " ");
                    j++;
                }
                pid_t pid = fork();
                if(pid == 0) {
                    execvp(args[0], args);
                    printf("Command not found: %s\n", args[0]);
                    perror("Exec failed");
                    exit(1);
                } else if(pid > 0) {
                    waitpid(pid, NULL, 0);
                } else {
                    printf("Fork failed\n");
                    exit(1);
                }
            }
        }
    }
    return 0;
}

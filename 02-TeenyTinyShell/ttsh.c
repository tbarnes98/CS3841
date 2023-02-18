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

#define INPUT_MAX 256
#define CMD_MAX 5

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
            fprintf(stderr, "Too many commands\n");
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

    // TODO need to be able to get input from
    //    the user in a loop

    // Print the input prompt
    printf("$> ");

    // Read user input
    if (read_cmd_string(user_input) == -1)
    {
        return 1;
    }

    // TODO: Figure out how to handle the 'quit' command
    if (strcmp(user_input, "quit") == 0){
        printf("Quitting...");
        exit(0);
    }


    // Chop the input into command strings
    int cmd_count = parse_commands(user_input, cmd_strs);
    if (cmd_count == -1)
    {
        return 1;
    }

    // Chop the commands into arguments and execute one at a time
    for (int i = 0; i < cmd_count; i++)
    {
        // TODO:
        //    1) Chop the command into command line arguments
        //        need to handle up to 10 arguments
        //        NOTE: the command name is always the first argument
        //    2) fork a process
        //    3) execute the command with execvp
        
    }

    return 0;
}

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#define PATH_DELIMETER ';'
#define PATH_SEP '\\'
#else
#define PATH_DELIMETER ':'
#define PATH_SEP '/'
#endif

static int builtinCommandCount = 1;
static int inputBufferLength = 1000;
static int commandArgumentLength = 100;
static int argumentCapacity = 20;

int getPathDirectories(char pathDirectories[][1024]);
int getUserInput(char *inputBuffer, int inputBufferLength);
int parseUserInput(char *inputBuffer, char *command, char arguments[][commandArgumentLength], int argumentCapacity);
int searchInPath(char *fullFilePath, char pathDirectories[][1024], char *command, int pathDirectoryCount);

int main(int argc, char *argv[])
{
    char shellPrompt[] = "Hamza Boyraz";
    char builtinCommands[builtinCommandCount][commandArgumentLength];
    strncpy(builtinCommands[0], "exit", commandArgumentLength);

    // Get the path variable
    char pathDirectories[100][1024];
    int pathDirectoryCount = getPathDirectories(pathDirectories);

    char inputBuffer[inputBufferLength];
    int argumentCount = 0;
    char command[commandArgumentLength];
    char arguments[argumentCapacity][commandArgumentLength];

    // REPL
    while (1)
    {
        // Print the shellPrompt
        printf("%s> ", shellPrompt);

        // Get the input from user
        if (getUserInput(inputBuffer, inputBufferLength) != 0)
        {
            break;
        }

        // Parse the user input
        argumentCount = parseUserInput(inputBuffer, command, arguments, argumentCapacity);

        if (strcmp(command, builtinCommands[0]) == 0) // EXIT
        {
            break;
        }
        else // NOT BUILTIN
        {
            char fullFilePath[1024];
            if (searchInPath(fullFilePath, pathDirectories, command, pathDirectoryCount))
            {
                // printf("command %s is found in %s\n", command, fullFilePath);

                char fullCommand[1024] = "";
                strncpy(fullCommand, command, 1024);
                for (int i = 0; i < argumentCount; i++)
                {
                    sprintf(fullCommand, "%s %s", fullCommand, arguments[i]);
                }
                system(fullCommand);
            }
            else
            {
                printf("%s: command not found\n", command);
            }
        }

        /*
        printf("command read: %s\n", command);
        printf("Argument count: %d\n", argumentCount);
        for (int i = 0; i < argumentCount; i++)
        {
            printf("Argument %d: %s\n", i + 1, arguments[i]);
        }
        */
    }

    return 0;
}

int getPathDirectories(char pathDirectories[][1024])
{
    // GET PATH ENVIRONMENT VARIABLE
    char *path = getenv("PATH");
    // SEPERATE PATH ENVIRONMENT VARIABLE
    char *iterPath = path;
    int pathDirectoryCount = 0;
    int pathDirectoryIndex = 0;
    while (*iterPath != '\0')
    {
        if (*iterPath == PATH_DELIMETER)
        {
            if (pathDirectories[pathDirectoryCount][pathDirectoryIndex - 1] == PATH_SEP)
            {
                pathDirectories[pathDirectoryCount][pathDirectoryIndex - 1] = '\0';
            }
            else
            {
                pathDirectories[pathDirectoryCount][pathDirectoryIndex] = '\0';
            }
            pathDirectoryCount++;
            pathDirectoryIndex = 0;
        }
        else
        {
            pathDirectories[pathDirectoryCount][pathDirectoryIndex++] = *iterPath;
        }

        iterPath++;
    }
    pathDirectories[pathDirectoryCount++][pathDirectoryIndex] = '\0';

    return pathDirectoryCount;
}

int getUserInput(char *inputBuffer, int inputBufferLength)
{
    if (NULL == fgets(inputBuffer, inputBufferLength, stdin))
    {
        return -1;
    }

    for (int i = 0; i < inputBufferLength; i++)
    {
        if (inputBuffer[i] == '\n')
        {
            inputBuffer[i] = '\0';
            break;
        }
    }

    return 0;
}

int parseUserInput(char *inputBuffer, char *command, char arguments[][commandArgumentLength], int argumentCapacity)
{
    int argumentCount = 0;
    int argumentCurrentIndex = 0;

    char *ptrBuffer = inputBuffer;
    char *ptrCommand = command;

    // Skip spaces
    while (*ptrBuffer == ' ')
    {
        ptrBuffer++;
    }

    // Parse command
    while (*ptrBuffer != ' ' && *ptrBuffer != '\0')
    {
        if ((ptrCommand - command) >= commandArgumentLength - 1)
        {
            while (*ptrBuffer != ' ' && *ptrBuffer != '\0')
            {
                ptrBuffer++;
            }
            break;
        }
        *ptrCommand = *ptrBuffer;
        ptrBuffer++;
        ptrCommand++;
    }
    *ptrCommand = '\0';

    // Parse arguments
    while (*ptrBuffer != '\0')
    {
        if (*ptrBuffer != ' ')
        {
            if (argumentCurrentIndex >= commandArgumentLength - 1)
            {
                arguments[argumentCount++][argumentCurrentIndex] = '\0';
                argumentCurrentIndex = 0;
                while (*ptrBuffer != ' ' && *ptrBuffer != '\0')
                {
                    ptrBuffer++;
                }
                continue;
            }

            arguments[argumentCount][argumentCurrentIndex++] = *ptrBuffer;
            if (*(ptrBuffer + 1) == ' ' || *(ptrBuffer + 1) == '\0')
            {
                arguments[argumentCount++][argumentCurrentIndex] = '\0';
                argumentCurrentIndex = 0;
                if (argumentCount >= argumentCapacity)
                {
                    break;
                }
            }
        }

        ptrBuffer++;
    }

    return argumentCount;
}

int searchInPath(char *fullFilePath, char pathDirectories[][1024], char *command, int pathDirectoryCount)
{
    // CHECK PATH
    for (int i = 0; i < pathDirectoryCount; i++)
    {
        sprintf(fullFilePath, "%s%c%s", pathDirectories[i], PATH_SEP, command);
        return 1;
    }
    return 0;
}
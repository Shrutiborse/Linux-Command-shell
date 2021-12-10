

/********************************************************************************************

This is a template for assignment on writing a custom Shell. 


Students may change the return types and arguments of the functions given in this template,

but do not change the names of these functions.


Though use of any extra functions is not recommended, students may use new functions if they need to, 

but that should not make code unnecessorily complex to read.


Students should keep names of declared variable (and any new functions) self explanatory,

and add proper comments for every logical step.


Students need to be careful while forking a new process (no unnecessory process creations) 

or while inserting the single handler code (should be added at the correct places).


Finally, keep your filename as myshell.c, do not change this name (not even myshell.cpp, 

as you not need to use any features for this assignment that are supported by C++ but not by C).

*********************************************************************************************/

#include <stdio.h>

#include <string.h>

#include <stdlib.h> // exit()

#include <unistd.h> // fork(), getpid(), exec()

#include <sys/wait.h> // wait()

#include <signal.h> // signal()

#include <fcntl.h> // close(), open()

int parseInput(char *str)

{

    // This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).

    int NoOfcmd = 3;

    char listcmd[3][20] = {"&&", "##", ">"};

    //char listcmd[NoOfcmd];

    //listcmd[0]='&&';

    //listcmd[1]='##';

    //istcmd[2]='>';

    char *cpstr = strdup(str);

    char *token = strtok(cpstr, " ");

    //if(strcmp(token,"exit")==0||strcmp(token,"EXIT")==0){

    // return 0;

    while (token != NULL)

    {

        for (int i = 0; i < 3; i++)

        {

            if (strcmp(token, listcmd[i]) == 0)

            {

                return i + 1;
            }
        }

        token = strtok(NULL, " ");
    }

    return 4;
}

char *trim_white_spaces(char *str)

{

    //printf("inside trim -->%s\n", str);

    str = strsep(&str, "\n");

    while (*str == ' ')

    {

        str++;
    }

    char *endpoint = str + strlen(str) - 1;

    while (*endpoint == ' ' && str < endpoint)

    {

        endpoint--;
    }

    endpoint[1] = '\0';

    return str;
}

void executeCommand(char *str)

{

    // This function will fork a new process to execute a command

    str = trim_white_spaces(str);

    char *cp = strdup(str);

    char *first_token = strsep(&cp, " ");

    if (strcmp(first_token, "cd") == 0)

    {

        int r = chdir(cp);
        if (r == -1)
        {

            printf("Shell: Incorrect command\n");
            exit(1);
        }
    }

    else

    {

        char **arrstr;

        arrstr = (char **)malloc(sizeof(char *) * 50);

        char *tempstr;

        for (int i = 0; i < 50; i++)

        {

            arrstr[i] = (char *)malloc(sizeof(char) * 50);
        }

        int i = 0;

        //printf("sep str\n");

        while ((tempstr = strsep(&str, " ")) != NULL)

        {

            //strcpy(arrstr[i],tempstr);

            arrstr[i] = tempstr;

            i++;
        }

        arrstr[i] = NULL;

        //printf("%s-->printing arrstr[0]", arrstr[0]);

        if (strcmp(arrstr[0], "cd") != 0)

        {

            int rc = fork();

            if (rc < 0)

            { // fork failed; exit

                exit(0);
            }

            else if (rc == 0)

            { // child (new) process

                //printf("%d is child PID\n", getpid());

                signal(SIGINT, SIG_DFL);

                signal(SIGTSTP, SIG_DFL);

                // -------- EXEC system call ---------

                //char *execCommand[] = {arrstr[0], NULL};

                int r = execvp(arrstr[0], arrstr);

                if (r == -1)

                {

                    printf("Shell: Incorrect command\n");
                    exit(1);
                }

                // -----------------------------------
            }

            else

            { // parent process (rc holds child PID)

                int rc_wait = wait(NULL);

                //printf("%d is parent PID, %d is child pid\n", getpid(), rc);
            }
        }
    }
}

void executeParallelCommands(char *str)

{

    // This function will run multiple commands in parallel

    printf("inside parallel\n");

    str = trim_white_spaces(str);
    char *tempstr = strsep(&str, "&&");

    //count=count/2;
    while (tempstr != NULL)
    {
        char *temp = strsep(&tempstr, " ");
        if (strcmp(temp, "cd") == 0)
        {
            char *temp = strsep(&tempstr, " ");
            if (temp == NULL)
            {
                return;
            }
            else
            {
                int r = chdir(temp);
                if (r == -1)
                {

                    printf("Shell: Incorrect command\n");
                    exit(1);
                }
            }
        }
        else
        {
            int rc1 = fork();

            if (rc1 < 0)
            { // fork failed; exit
                exit(0);
            }
            else if (rc1 == 0)
            {
                //child (new) process 1
                char *execCommand[] = {temp, NULL};
                int r = execvp(execCommand[0], execCommand);
                if (r < 0)

                {

                    printf("Shell: Incorrect command\n");
                    exit(0);
                }
            }
        }
        tempstr = strsep(&str, " ");
    }
    while (wait(NULL) > 0)
        ;
}

void executeSequentialCommands(char *str)

{
    //  printf("inside serial\n");
    // This function will run multiple commands in parallel

    str = trim_white_spaces(str);
    char *tempstr = strsep(&str, "##");

    //count=count/2;
    while (tempstr != NULL)
    {
        char *temp = strsep(&tempstr, " ");
        if (strcmp(temp, "cd") == 0)
        {
            temp = strsep(&tempstr, " ");
            if (temp == NULL)
            {
                return;
            }
            else
            {
                int r = chdir(temp);
                if (r == -1)

                {

                    printf("Shell: Incorrect command\n");
                    exit(0);
                }
            }
        }
        else
        {
            int rc1 = fork();

            if (rc1 < 0)
            { // fork failed; exit
                exit(0);
            }
            else if (rc1 == 0)
            {
                //child (new) process 1
                char *execCommand[] = {temp, NULL};
                int r = execvp(execCommand[0], execCommand);
                if (r == -1)
                {

                    printf("Shell: Incorrect command\n");
                    exit(1);
                }
            }

            else
            {                              // parent process (rc holds child PID)
                int rc_wait1 = wait(NULL); // COMMENTING THIS WAIT WILL CHANGE THE EXECUTION FROM SERIAL TO PARALLEL
                //printf("Parent PID:%d, Child1 pid:%d\n\n", getpid(), rc1);
            }
        }
        tempstr = strsep(&str, " ");
    }
}

void executeCommandRedirection(char *str)

{

    // This function will run a single command with output redirected to an output file specificed by user

    // This function will run a single command with output redirected to an output file specificed by user
    //printf("inside redirection\n");
    //ls > abc.txt
    str = trim_white_spaces(str);
    //printf("%s",str);
    char *tempstr = strsep(&str, ">");
    //printf("%s sep str 1 \n",tempstr);
    char *file = str;
    file = trim_white_spaces(file);
    char **arrstr;
    arrstr = (char **)malloc(sizeof(char *) * 50);
    char *astr;
    for (int i = 0; i < 50; i++)
    {
        arrstr[i] = (char *)malloc(sizeof(char) * 50);
    }
    int i = 0;
    //printf("sep str\n");
    while ((astr = strsep(&tempstr, " ")) != NULL)
    {
        //strcpy(arrstr[i],tempstr);
        arrstr[i] = astr;
        i++;
    }
    arrstr[i] = NULL;

    int rc = fork();

    if (rc < 0)
    { // fork failed; exit
        exit(0);
    }
    else if (rc == 0)
    { // child (new) process
        //printf("%d is child PID\n", getpid());

        // ------- Redirecting STDOUT --------

        close(STDOUT_FILENO);
        open(strsep(&file, "\n"), O_CREAT | O_WRONLY | O_APPEND);

        // -----------------------------------

        //char **execCommand = executeCommand(arrstr);
        //executeCommand(arrstr);
        //int r=execvp(execCommand[0], execCommand);
        int r = execvp(arrstr[0], arrstr);

        if (r == -1)
        {
            printf("Shell: Incorrect command\n");
            exit(1);
        }
    }
    else
    { // parent process (rc holds child PID)
        int rc_wait = wait(NULL);
        //printf("%d is parent PID, %d is child pid\n", getpid(), rc);
    }
}

int main()

{

    signal(SIGINT, SIG_IGN);

    signal(SIGTSTP, SIG_IGN);

    //int n=500;

    char *inputstr = NULL; // Initial declarations

    char **arrstr;

    arrstr = malloc(sizeof(char *) * 50);

    char currentWorkingDirectory[50];

    size_t length = 50;

    while (1) // This loop will keep your shell running until user exits.

    {

        // Print the prompt in format - currentWorkingDirectory$char cmd[1024];

        getcwd(currentWorkingDirectory, length);

        printf("%s$", currentWorkingDirectory);

        size_t b = 50;

        //inputstr=(char *)malloc(b*sizeof(char));

        // accept input with 'getline()'

        getline(&inputstr, &b, stdin);

        //printf("%s", inputstr);

        if (strcmp(inputstr, "exit\n") == 0 || strcmp(inputstr, "EXIT\n") == 0) // When user uses exit command.

        {

            printf("Exiting shell...\n");

            break;
        }

        // Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.

        int flagset = parseInput(inputstr);

        //printf("\n------------->After parsing--%s", inputstr);

        if (flagset == 1)

        {

            executeParallelCommands(inputstr);

        } // This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)

        else if (flagset == 2)

        {

            executeSequentialCommands(inputstr);

        } // This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)

        else if (flagset == 3)

        {

            executeCommandRedirection(inputstr);

        } // This function is invoked when user wants redirect output of a single command to and output file specificed by user

        else

        {

            executeCommand(inputstr);

        } // This function is invoked when user wants to run a single commands
    }

    return 0;
}
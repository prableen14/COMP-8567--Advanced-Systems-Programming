/*Prableen Kaur Sachdeva
110100670
Section:4
*/

//for pid_t, size_t

#include <sys/types.h>

//here this is for handling any true/false flags which are there for checking conditions below
#include<stdbool.h>

//this here is used for access to in-builtr methds like malloc(), exit()
#include <stdlib.h>

//this is for doing any manipulatin with string, like funct like strstr(), strcpy()
#include <string.h>

//for general use of system calls like read(),open(), write()
#include <unistd.h>

// define macros and functions that allow a parent process to wait for its child processes to finish their execution, WIFEXITED(), waitpid()
#include <sys/wait.h>

//O_RDWR, O_RDONLY, O_WRONLY
#include <sys/fcntl.h>

//for taking inpt from usr /displaying otp and it is used for fgets(), printf() funcs
#include <stdio.h>

#define PERMIT_PIPES 8

#define PERMITARG 5

#define LENCMD_ST 560

//Global variables which are declared for storing commands from console, length of the inputted cmd, an array pipe is defined for communication between child and parnt processes 

char twoDarrayForStoringCmmds[8][200];

int cnsleCmdLgth;
int pipeForCommunicationPC[2];
int inputtedCmdsonShellCnt;

//STRING MANIPULATION MODULES
//This below method is processing commaands based on ~ sign found in any command so that it can expand that using getenv func to extract whole path to that file or directorry

int tildaHomeCustomPrseCmnd(char* modiCmDForHomePathExp, char** valueOfArgOfConsle) {
    int argc = 0;

  //split a string into tokens based on provided delimiters (in this case, space ' ', tab '\t', and newline '\n').
    char* ctkns = strtok(modiCmDForHomePathExp, " \t\n");

  //loop will run until all tokens (Each word or component of command string is treated as a separate token) have been processed

    while (ctkns != NULL) {

        // Check if the ctkns starts with ~ and expand it to the home directory

//ls -1 ~/ch

        if (ctkns[0] == '~') {

        

        // Allocate memory to hold expanded token (argument) by appending home directory to it

            char* appendedExpandedTknForHome = malloc(strlen(ctkns) + strlen(getenv("HOME")) + 1);

            

        // Coppying home directory to appendedExpandedTknForHome

            strcpy(appendedExpandedTknForHome, getenv("HOME"));

            

        //appending content of source string (token + 1) to the end of destination string appendedExpandedTknForHome

            strcat(appendedExpandedTknForHome, ctkns + 1);

            

      //store token (either original token or the expanded version, depending on whether it starts with '~') into arguments array at current index argc

            valueOfArgOfConsle[argc] = appendedExpandedTknForHome;

        } else {

      //  If token does not start with '~'

        //Store the current token in valueOfArgOfConsle array at curr agc index. This adds token to list of arguments without any modifications

            valueOfArgOfConsle[argc] = ctkns;

        }

      //variable to keep track of the number of parsed arguments

        argc++;

      //repeating until all tokens are processed

        ctkns = strtok(NULL, " \t\n");

    }

    

    // This is required for execvp func to determine end of valueOfArgOfConsle array

    valueOfArgOfConsle[argc] = NULL;  // Set the last argument as NULL for execvp()

    return argc;

}



//This below method is manipulating string based on " ". It is taking character pointer array (array of strings) that will hold split commands (arrForHoldMdCDs) and character pointer (string) representing input command to be split

void treatStngInCmmd(char *arrForHoldMdCDs[50], char *givnInpStrg)

{

    int spltCmdsTrk = 0;

    

  //split a string into tokens based on delimiters (in this case, space ' ')

    char *reTkns = strtok(givnInpStrg, " "); //reTkns represnts returned Tokens list

    while (true)

    {

    //used to determine if there are no more tokens left to process in givnInpStrg string

        if (reTkns == NULL)

        {

        

        //terminating the EOS (end of strng) indicating end of the array of strings

            arrForHoldMdCDs[spltCmdsTrk] = NULL;

            spltCmdsTrk++;

            break;

        }

        

        //another word in given command to process

        arrForHoldMdCDs[spltCmdsTrk] = reTkns;

        spltCmdsTrk++;

        

    //In command cat pfle.txt, it will be tokenised using " " (space) individually

        reTkns = strtok(NULL, " "); 

    }



    if (spltCmdsTrk < 1 || spltCmdsTrk > 5)

    {

        puts("The argument count for each comand should be in range of >=1 and <=5");

        exit(10);

    }

}



//DIFFERENT METHODS FOR COMMAND PARSING



//Here, in this function we are performing pipe operator based on the inputted pipe symbol found in command. Also if no pipe is found in commmand, then also thismethd will handle this case (normal simple commands)

void operationOfPipingInShell(char *command) //taking commands to be executed in a pipeline

{



//array of character pointers used to store individual arguments of each command

char* arguments[PERMITARG + 2];  // Add 1 for the NULL terminator

    char* pipeCommands[PERMIT_PIPES + 1]; // Add 1 for the NULL terminator

   // no. of commands in pipeline

    int pipeCount = 0;

    

    // Split the command based on the pipe character (|) using strtok()

    char* ctkns = strtok(command, "|");

    while (ctkns != NULL) {



        // removing leading and trailing whitespaces

        while (*ctkns && (*ctkns == ' ' || *ctkns == '\t'))



//increamenting word count after removing those

            ++ctkns;

            

           //finding size, length ofthose tokenss

        size_t len = strlen(ctkns);



//removing any whitespace or tab

        while (len > 0 && (ctkns[len-1] == ' ' || ctkns[len-1] == '\t'))



            ctkns[--len] = '\0';

        // Add the trimmed command to pipeCommands array



        pipeCommands[pipeCount++] = ctkns;

        

     // setting to next tokenized command using strtok with the delimiter |. This prepares token for the next iteration of the loop, extracting the next command from the input string.

        ctkns = strtok(NULL, "|");

    }

  //putting last element of pipeCommands array to NULL, indicating end of commands list

    pipeCommands[pipeCount] = NULL;

   // printf("pipcount%d\n",pipeCount);

  

  //checks if there is at least one command and no. of commands doesn't exceed PERMIT_PIPES threshold

    if (pipeCount > 0 && pipeCount <= PERMIT_PIPES) {

    

        int prev_pipe[2]; // Store the read & write end of previous pipe

      // iterating over each command in pipeCommands array

        for (int i = 0; i < pipeCount; ++i) {

            char* currentCommand = pipeCommands[i];

        

            int argc = tildaHomeCustomPrseCmnd(currentCommand, arguments);

       //checks if there are valid arguments for current command and that no of arguments does not exceed the PERMITARG limit

       //it should be btweeen range of 1 to 5

            if (argc > 0 && argc <= PERMITARG) {

            

         //here file descriptor for piping is initialised which is an array of size 2 (0 for read end, 1 for write end)   

                int des_f_PIPEING[2];

                

                //here we are checking if piping failed

                if (pipe(des_f_PIPEING) < 0) {

                    perror("Failing of Pipe Creation");

                    exit(EXIT_FAILURE);

                }

                pid_t pp_IdPpe = fork();

                if (pp_IdPpe < 0) {

                    perror("Fork failed");

                    exit(EXIT_FAILURE);

                    

                 // Child process 

                } else if (pp_IdPpe == 0) {

                    

      //If it's not first command, child process connects its standard input (stdin) to read end of previous pipe

//This allows output of previous command to become input of current command in pipeline

                    if (i > 0) {

                        close(prev_pipe[1]); // Close the write end of the previous pipe

                        dup2(prev_pipe[0], STDIN_FILENO); // Redirect stdin to the read end of the previous pipe

                        close(prev_pipe[0]); // Close the read end of the previous pipe

                    }

                    // Connect the write end of the current pipe

                    if (i < pipeCount - 1) {

            //If it's not the last command, the child process connects its standard output (stdout) to the write end of the current pipe.

//This allows output of current command to be sent as input to next command in pipeline

                        close(des_f_PIPEING[0]); // Close the read end of the current pipe

                        dup2(des_f_PIPEING[1], STDOUT_FILENO); // Redirect stdout to the write end of the current pipe

                        close(des_f_PIPEING[1]); // Close the write end of the current pipe

                    }

                    

                   //here the execvp is executing each command in pipeing 

                   //ls -1 ~/ch

                    execvp(arguments[0], arguments);

                    perror("Execution failed");

                    exit(EXIT_FAILURE);

                } else {

                    // Parent process

                    if (i > 0) {

                  //as these are nt required here so parent process closes file descriptors of previous pipe

                        close(prev_pipe[0]);

                        close(prev_pipe[1]);

                    }

                    prev_pipe[0] = des_f_PIPEING[0];



                    prev_pipe[1] = des_f_PIPEING[1];



                    // Wait for the child process to finish

                    int piCkFgST;

                    waitpid(pp_IdPpe, &piCkFgST, 0);

            //it prints exit status of the command, but only for last command in pipeline (i == pipeCount - 1)

                    if (WIFEXITED(piCkFgST)) {

                        if (i == pipeCount - 1) {

                        }       }

                }

            } else {

                printf("Invalid number of arguments\n");

                break;

            }

        }

        // Close the last pipe

        if (pipeCount > 1) {

        

        //closing read, write end pipe becoz it will put "bad file descriptor on console" if we wouldnt close all repsective descriptors respectively

            close(prev_pipe[0]);

            close(prev_pipe[1]);

        }

    } else {

        printf("Invalid number of commands\n");



    }

}



//below method is an implementation defined for appending or output redirection in case of > opertr 

void appdOtpRedi(char *usrGivenConsoleCmd, int chkForTrunOrAppd)

{



//a variable ipCntST (input count of String) that will keep check of how many cmd are there 

    int ipCntST = 0;

  

  //loooping through all the commdss in thw cnsleCmdLgth to check and separate basd on operator in inpuuted string

    for (int i = 0; i < cnsleCmdLgth; i++)

    {

    //checks if current character at index is (>), which denotes the right redirection

        if (usrGivenConsoleCmd[i] == '>')

        {

        //(>>) is encountered, indicating append redirection.

            if (usrGivenConsoleCmd[i + 1] == '>')

            {

            // If the next character is also '>', it's ">>" for append redirection

                i++;

            }

          //after each command is parsed, weare adding a null-terminator ('\0') to seaparte it with othwer comds 

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][ipCntST] = '\0';

            inputtedCmdsonShellCnt++;

            ipCntST = 0;

        }

        else

        {

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][ipCntST] = usrGivenConsoleCmd[i];

            ipCntST++;

        }

    }



    twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][ipCntST] = '\0';

    inputtedCmdsonShellCnt++;

    

// ensuring that func expects exactly two parsed commands for redirection (input command and output file)

    if (inputtedCmdsonShellCnt != 2)

    {

        printf("Please write upto 2 argumnets only for this Redirction case");

          close(pipeForCommunicationPC[0]);



        close(pipeForCommunicationPC[1]);

        //exit on failure condition for not having 2 args

        exit(1);

    }



    int desOfFleForCking;

    int opPD = fork();

    if (opPD == 0)

    {

        char *ag1ForCD[50];

        char *ag2ForCD[50];

        

        //sending parsed cmds from above to funct treatStngInCmmd which will modify command for any spaces, so spliting the parsed commands stord in twoDarrayForStoringCmmds into individual arguments (tokens) and store them in ag1ForCD & ag2ForCD arrays

        treatStngInCmmd(ag1ForCD, twoDarrayForStoringCmmds[0]);

        

        //printf("%s",ag1ForCD); //for just purpose off debug

        

        treatStngInCmmd(ag2ForCD, twoDarrayForStoringCmmds[1]);

        

        //here we are initializes an int variable chkForTrunOrAppd1 with a combination of flags for opening output file

      //flags are O_WRONLY (write-only mode), O_CREAT (create the file if it doesn't exist), and O_TRUNC or O_APPEND, depending on value of chkForTrunOrAppd1. If flag is 0, O_TRUNC is used for normal output, and if flag is not 0, O_APPEND is used for append output

        int chkForTrunOrAppd1 = O_WRONLY | O_CREAT | (chkForTrunOrAppd == 0 ? O_TRUNC : O_APPEND);

        

        //opening output file specified by ag2ForCD[0] (1st argumnt after > in cmmand) with given chkForTrunOrAppd1 and file permissions 0775. File desriptor of opned file is stored in desOfFleForCking 

        desOfFleForCking = open(ag2ForCD[0], chkForTrunOrAppd1, 0775);

        

        //duplicating file descriptor desOfFleForCking to standard output file descriptor (STDOUT_FILENO).any output written to standard output will be redirected to output file now

        dup2(desOfFleForCking, STDOUT_FILENO);

        

        //closing read end of pipe stored in pipeForCommunicationPC array in chld process (if it was open), close write end of pipe in child prcess (if it was open), and closing outpt fle descriptor desOfFleForCking to clean up open file descriptors

        close(pipeForCommunicationPC[0]);  

        close(pipeForCommunicationPC[1]);

        close(desOfFleForCking);

 

        //here execvp() is called with arg1FrCDwhich[0] having first argument of command (before > opertaor) and the next ag1ForCD is having next argument after the > operator . so execvp() will xecute this respective command with provided args 

        execvp(ag1ForCD[0], ag1ForCD);

        

        //printing error

        fprintf(stderr, "Couldnt execute this command");

        

        //exit with failure stat

        exit(EXIT_FAILURE);

    }

    else

    {

        //closing not required pipe ends

        close(pipeForCommunicationPC[0]);

        

        //write end closed

        close(pipeForCommunicationPC[1]);

        exit(EXIT_FAILURE);

    }

}



//here we are reading from a file/taking input (not from console, but from somewhere else) and then redirecting it to console to show output

void inpRedirectMthodForCmmd(char *usrGivenConsoleCmd)

{

  int sze = 0;



    for (int i = 0; i < cnsleCmdLgth; i++) {

        if (usrGivenConsoleCmd[i] == '<') {

      //If < is found, it means that a command is complete (before < symbol). So, it terminates current command by adding a null terminator '\0' at the end of twoDarrayForStoringCmmds[inputtedCmdsonShellCnt] array

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][sze] = '\0';

   //Increments inputtedCmdsonShellCnt to indicate that another command is being processed. It also resets sze to 0 to start building next command

            inputtedCmdsonShellCnt++;

            sze = 0;

        } else {

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][sze] = usrGivenConsoleCmd[i];

            sze++;

        }

    }



    twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][sze] = '\0';

    inputtedCmdsonShellCnt++;

//checking if no. of commands parsed is not equal to 2. Input redirection requires exactly two commands (command before < and command after <)

    if (inputtedCmdsonShellCnt != 2) {

    

    //If number of commands is not 2, it prints an error message to standard error stream (stderr) and exits program with a failure status

        fprintf(stderr, "Error: Input redirection requires two commands.\n");

        exit(EXIT_FAILURE);

    }

    

//file descriptrs

    int ipDsFl;

    //piping array for commincation btw child & parnt  

    int frstPIPE[2];

    

    //creatin error of pipe

    if (pipe(frstPIPE) == -1) {

        perror("Failing of Pipe Communication Creation");

        exit(EXIT_FAILURE);

    }



    int ipPDprc = fork();

    if (ipPDprc == 0) {

        char *inpCF1[50];

        char *inpCF2[50];

        

        //in child prc, the cmds are passed to functions for treating the strings based on spacs between them

        treatStngInCmmd(inpCF1, twoDarrayForStoringCmmds[0]);

        treatStngInCmmd(inpCF2, twoDarrayForStoringCmmds[1]);



        ipDsFl = open(inpCF2[0], O_RDONLY);

        if (ipDsFl == -1) {

            perror("Input file opening failed");

            exit(EXIT_FAILURE);

        }

//dup2(ipDsFl, 0) makes ipDsFl the new standard input

        dup2(ipDsFl, 0);

        

     //makes the write end of frstPIPE the new standard output

        dup2(frstPIPE[1], 1);



//closing now read end (O) of pipe

        close(frstPIPE[0]);

        

    //closing now write end (1) of pipe   

        close(frstPIPE[1]);

        

        //lets close file descritor of pipeing that we have defined above

        close(ipDsFl);

        

//in this statement the srgs before < and after that is passed to execvp() fnct

        execvp(inpCF1[0], inpCF1);

        

        //if command was unsucessful, then fprintf() is putting that to stderr that is stnadard error stream

        fprintf(stderr, "command couldnt be executed\n");

        

        //fail case for command

        exit(EXIT_FAILURE);

        

        //

    } else if (ipPDprc > 0) {

    

    //write end closed in prnt 

        close(frstPIPE[1]);

        

        //waiting for commamd above to get executed

        wait(NULL);



        // Read the output from the pipe and print it to the terminal

        char bfForReadFromFile[1024];

        int bytesRead;

        

     // reads data into buffer array and writes it to standard output (STDOUT_FILENO). This way, it captures output of child process and displays it in terminal

        while ((bytesRead = read(frstPIPE[0], bfForReadFromFile, sizeof(bfForReadFromFile))) > 0) {

        

   //parent process reads data from pipe (where child process writes its output), and as long as there is data available, it writes it to terminal until all output has been processed

            write(STDOUT_FILENO, bfForReadFromFile, bytesRead);

        }

//closes the read end of frstPIPE, as not required in the parent process now

        close(frstPIPE[0]);

    } else {

    //

        perror("Fork failed");

        exit(EXIT_FAILURE);

    }  

}





//In this implementattation here for processing && and || operator and amalgation of both, we have below method where first it checks the occurence of respective symobols in the command inputted by user and based on that, it will determine which one will get executed and which one will be skipped

int cdsOnCnsoleCntForPrc;  // Declare cdsOnCnsoleCntForPrc as a global variable storing num of commands separated by || or &&

char *commandss[256]; 

void condAndcondOrInCmmd(char *usrGivenConsoleCmd)

{



//here we are declarngg ctkns and splitTkn as charactr pinter variables for checking the total num of words of commds found on cnsle given by user

    char *ctkns;

    char *splitTkn;



//the belwo conditions are checking If it finds "&&", it sets splitTkn variable to "&&", otherwise, if it finds "||", it sets splitTkn to "||"

   //strstr() is used to find first occurrence of a && within a given string of command

    if (strstr(usrGivenConsoleCmd, "&&") != NULL) {

        ctkns = strtok(usrGivenConsoleCmd, "&&");

        splitTkn = "&&";

        

    } 

    //same chceking for ||

    else if (strstr(usrGivenConsoleCmd, "||") != NULL) {

        ctkns = strtok(usrGivenConsoleCmd, "||");

        splitTkn = "||";

        

      //if no && or || is found

    } else {

        printf("Invalid input. Operator '&&' or '||' not found");

        exit(1);

    }



//we are looping through all words separated by && and || opratr and saving it t array commandss, until tokens willl get NULL (words in commands are finished)

    while (ctkns != NULL) {

        commandss[cdsOnCnsoleCntForPrc++] = ctkns;

        ctkns = strtok(NULL,splitTkn);

    }



//in case count of commands goes above threshld which is 1 or >6 (operators can be upto 7)

    if (cdsOnCnsoleCntForPrc < 1 || cdsOnCnsoleCntForPrc > 6) {

    

    //so printing this in that case

        printf("Invalid number of commands given");

        

        //and get exit from there

        exit(1);

    }

    

  //  printf("num commands: %d",cdsOnCnsoleCntForPrc);

   

    int ckFgST;

    //iterating over sub-commands and executes each of them as separate child processes 

    for (int i = 0; i < cdsOnCnsoleCntForPrc; i++)

    {

        int runAND_OR_pc = fork();

        if (runAND_OR_pc == 0)

        {

        /*The execlp function takes the following arguments:

Syntx of execlp()

"sh": name of the shell 

"sh": 1st argument to shell, name of shell

"-c": next argument will be command to be executed by the shell

*/

           execlp("sh", "sh", "-c", commandss[i], NULL);

           

           //exit with 1 

            exit(1);

        }

        else if (runAND_OR_pc > 0)

        {

       //here prnt prcis waiting for child prcess execution 

            wait(&ckFgST);

        //so if the above found && in cmd, then it will go look into below success or afilure ststus of child process

            if(strcmp(splitTkn, "&&") == 0){

            if (WIFEXITED(ckFgST) && WEXITSTATUS(ckFgST) != 0) //command failed

            {    

          //if current comand failed in case of && operatr then break out ofloop and next wont get executed

                 break;

            }

            }

  //if || OR is there 

            else if (strcmp(splitTkn, "||") == 0)

            {

           // if command succeeds (i.e., returns a zero exit status)

            if (WIFEXITED(ckFgST) && WEXITSTATUS(ckFgST) == 0) {

                    // Command succeeded in OR case, break out of loop

                    break;

                }  

            }

        }

    }

}



//here each command will be executed sequentially according to commands given on console. Upto 7 commands can be ecxecuted line by line 

void prcSequentialRun(char *usrGivenConsoleCmd)

{

//this variable eachCdLg is initialised to zerro as it is handing how many arg are ther in each comamd

    int eachCdLg = 0;

    

    //now we are iterating over each word in the received commad frm main which user entered till the length of that command

    for (int i = 0; i < cnsleCmdLgth; i++)

    {

    //if at currebt index the ';' symbol is there 

        if (usrGivenConsoleCmd[i] == ';')

        {

        //then we are putting \0 for marking end of current command and prepares to process next command

        //stores the completed command in twoDarrayForStoringCmmds array, where inputtedCmdsonShellCnt is index of current command, and eachCdLg is length of command being parsed

        

        /*twoDarrayForStoringCmmds[0][5] = "ls -l"

twoDarrayForStoringCmmds[1][13]= "echo Prableen"

twoDarrayForStoringCmmds[2][3] = "pwd"

twoDarrayForStoringCmmds[3][4] = "date"

*/

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][eachCdLg] = '\0';

            

           //keeping track of length of current command while parsing. It ensures that next character from usrGivenConsoleCmd will be stored in correct position of buffer

            inputtedCmdsonShellCnt++;

            

            //again for next set of commamd we are setting length to zero

            eachCdLg = 0;

        }

        else

        {

        //when parsing process is not encountering a semicolon (';'), which means it's processing a character belonging to current command being parsed

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][eachCdLg] = usrGivenConsoleCmd[i];

            

            //then increasing the track of length variable

            eachCdLg++;

        }

    }

    

    //after each command, putting a null charcater for marking it as end

    twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][eachCdLg] = '\0';

    

    //count of each word increased (of command)

    inputtedCmdsonShellCnt++;

//printf("The entered total commands are:%d",inputtedCmdsonShellCnt);



//if we pass comamnds greter than 7 or less than 1, then its not correct acc to given ques

    if (inputtedCmdsonShellCnt < 1 || inputtedCmdsonShellCnt > 7)

    {

    

    //so printing this to our customised shell

        printf("You can only pass upto 7 commands");

        exit(1);

    }



//variable for checkng the execution status of each traversed command

    int seSQT;

    

    //iteratting through all words of commnd given

    for (int sqC = 0; sqC < inputtedCmdsonShellCnt; sqC++)

    {

    

    //now forking a child prc which will execute the comamd

        int cpSeqPd = fork();

        

        //if child process

        if (cpSeqPd == 0)

        {

        //here sqCD is a pi]ointer to cahr array that will be given to treatStngInCmmd() func which will append given command string basd on given demiliter (space in our case)

            char *sqCD[50];

            

            //caliing methd

            treatStngInCmmd(sqCD, twoDarrayForStoringCmmds[sqC]);

            

            //execvp is having that treatd string (words) here in which sqCD[0] is command name and sqCDafter that is the rst of argument provided to that comandd

            execvp(sqCD[0], sqCD);

          //in case any command fails to run, so returning only if an error occurs, exit

            exit(1);

        }

        else if (cpSeqPd> 0)

        {

        //parent process waiting for child prc 

            waitpid(cpSeqPd, &seSQT, 0);

            

            //checking the stats of child prc, if it fails, in case of non-zero value

            if (WIFEXITED(seSQT) && WEXITSTATUS(seSQT) != 0)

            {

            //so simply just closing all communication piping btween child and parent (read&write ends)

                close(pipeForCommunicationPC[0]);

                

                

                close(pipeForCommunicationPC[1]);

                exit(1);

            }

        }

    }

    close(pipeForCommunicationPC[0]);

    close(pipeForCommunicationPC[1]);

    exit(1);

}



//This below methd will set a process/program as a background process. Basic idea ofbelowis that first we are taking cmd as input from main and on the basis of detection of &, we will be giving that cmmd to execvp func which will xecute it and will st it as bg process

void puttingPrcToBackground(char *usrGivenConsoleCmd)

{

    int individualParCdsCnt = 0;

    for (int bp = 0; bp< cnsleCmdLgth; bp++)

    {

    

    // checks if the character at the current index (bp) is an ampersand (&), which indicates a background command

        if (usrGivenConsoleCmd[bp] == '&')

        {      

       // hee we are making sure that if there are any characters after ampersand. If there are, it means there are additional argumnts, which are not allowed for background execution

            if ((bp + 1) != cnsleCmdLgth)

            {

                printf("Error in given input command. Please dont put anything after &, not even space");

                exit(1);

            }

          //these below lines are handlin case when there is any space added after & sign of background in cmd

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][individualParCdsCnt] = ' ';

            

       //This line assigns an ampersand ('&') to the position in array of cmds right after space added in previous line. This ensures that & is appended to end of current command, denoting that it should be executed in background

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][individualParCdsCnt + 1] = '&';

            

        // adds a null-terminator ('\0') right after &, ensuring that current command is null-terminated

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][individualParCdsCnt + 2] = '\0';

            

        //total counet of cmds are increased

            inputtedCmdsonShellCnt++;

            

       // setting  individualParCdsCnt variable to 0, preparing it for parsing next command in string of cmdds

            individualParCdsCnt = 0;

        }

        else

        {

            twoDarrayForStoringCmmds[inputtedCmdsonShellCnt][individualParCdsCnt] = usrGivenConsoleCmd[bp];

            individualParCdsCnt++;

        }

    }

    

    //here are are forking a child process, which is responsible for executing parsed command using execvp()

    int bgPrc = fork();

    if (bgPrc == 0)

    {

        char *c1Pc[50];

    //we are sending the     

        treatStngInCmmd(c1Pc, twoDarrayForStoringCmmds[0]);

        

        printf("\n");

        //closes the standard input (file descriptor 0) and standard output (file descriptor 1)of child process

        close(0);close(1);

        

        //closinng theread end of pipe as it is not requirednow in childprocess

        close(pipeForCommunicationPC[0]);

        

        //clsing the write nd 

        close(pipeForCommunicationPC[1]);

        

        //running execvp with the given list of arg, with c1pc[0] having procss and c1Pc having next arg of background process, &

        execvp(c1Pc[0], c1Pc);

        

        // terminate command with a failure status if it got failed

        exit(EXIT_FAILURE);

    }

    else if (bgPrc > 0)

    {

    

    //closeing of unnesearry pipe end(read 0, write1)

        close(pipeForCommunicationPC[0]);  close(pipeForCommunicationPC[1]);

        

        //waiting for child process to execute comds

        wait(NULL);

        exit(1);

    }

    //if any failure occured

    exit(EXIT_FAILURE);

}





//Acc to the operator/character/symbol found btween commands given from user, thisfunc will determine which respective func neds to get called by looping through thestring command from the main method which is stord in the charactr array usrGivenConsoleCmd

void symbolAccExectCD(char *usrGivenConsoleCmd)

{



//we are itrating over all commannds in the inputtted string of array which have length of cnsleCmdLgth

    for (int cp = 0; cp < cnsleCmdLgth; cp++)

    {

    //if 2 consecutive || is there in the given string usrGivenConsoleCmd, so that means it is conditional OR condition

        if (usrGivenConsoleCmd[cp] == '|')

        {

        //next elemnt is also | (OR)

            if (usrGivenConsoleCmd[cp + 1] == '|')

            {

            //so here we arr calling the condAndcondOrInCmmd fucn with the given array of string containing the command

                condAndcondOrInCmmd(usrGivenConsoleCmd);

                

                //and after succesful running of this functin, we will just exit from this condition of loop

                exit(1);

            }

          

            //if onnly pipe symbol is there in cmdds, then we are calling pipe method with given input command form console

            operationOfPipingInShell(usrGivenConsoleCmd);

            //printf("\n");

            //exit form this condition

            exit(1);

        }

      //if we found a & symbol in the given command

        if (usrGivenConsoleCmd[cp] == '&')

        {

        //we are checking if the next symbol is also &, that means it is a && condition in command given so we will call condAndcondOrInCmmd() with given input string of comand

            if (usrGivenConsoleCmd[cp + 1] == '&')

            {

                condAndcondOrInCmmd(usrGivenConsoleCmd);

                exit(1);

            }

            

            //if single & is there that means it is for running the background process function, so we will cal this function

            puttingPrcToBackground(usrGivenConsoleCmd);

            exit(1);

        }



//if > is found at current index of array usrGivenConsoleCmd[cp], that means it is for redirection (output)

        if (usrGivenConsoleCmd[cp] == '>')

        {

        //if 2 consecutive >'s are found, that means 

            if (usrGivenConsoleCmd[cp + 1] == '>')

            {

            //double > and 1 means Redirect to file with Append mode, 

                appdOtpRedi(usrGivenConsoleCmd, 1);

                exit(1);

            }
// Here 0 is kind of a flag paased to func meaning Redirect to file with truncate mode
            appdOtpRedi(usrGivenConsoleCmd, 0);
            exit(1);
        }

//now we are checking fr conditin where < is found in given user string so that means it is input Redirection function

        if (usrGivenConsoleCmd[cp] == '<')

        {

            inpRedirectMthodForCmmd(usrGivenConsoleCmd);

            exit(1);

        }

//if we encunter ; on current index of array of string of cmamnd, so that means the comamnds have ; between them so we will be executing sequential unction belw

        if (usrGivenConsoleCmd[cp] == ';')

        {

            prcSequentialRun(usrGivenConsoleCmd);

            exit(1);

        }

        

    }

  //if no symbol is from above list is there, that means only a single commaad is inpuuted and we are calling   operationOfPipingInShell() because it is handling the case too when no pipe is there in comands (normal 1 command)

    operationOfPipingInShell(usrGivenConsoleCmd);

}



int main()

{

//here we are declaring pcForExecCurrCPFun variable which is used for frking a child process below which will run symbolAccExectCD() method below for executing commands on console based on operators/symblos between them

    int pcForExecCurrCPFun;

    

    //declaring character array inpConsoleUser (input from console by user) which have a fixed size LENCMD_ST (which is declared abve globally)

    char inpConsoleUser[LENCMD_ST];

    

    //" infinitely looping until user prsses CTRL-C or exit on this "mshell$"

    while (true)

    {

    

    //its printing the 'mshell$' on customised sheell

        printf("mshell$ ");

        

        //used for taking input from user

        fgets(inpConsoleUser,LENCMD_ST, stdin);



        // here we are maniputing string by rem \n char and replacing it with null terminator so as to mark end

        inpConsoleUser[strcspn(inpConsoleUser, "\n")] = '\0';



        // if exit is written, break out of loop

        if (strcmp(inpConsoleUser, "exit") == 0)

        {

        

        //now breka out

            break;

        }

        

        //cnsleCmdLgth is having the length of the given string on our shell by use of strlen

        cnsleCmdLgth = strlen(inpConsoleUser);

        

        //here we ar froking a child prcess for running ourfunction that will handle comamdns based on opertors founsd

        pcForExecCurrCPFun = fork();
        //child process running (checking for PID of child) if its zero
        if (pcForExecCurrCPFun == 0)
        {
       //calling func symbolAccExectCD(), which is short for symbol Accordingly Executing command
            symbolAccExectCD(inpConsoleUser);
            //exit after running
            exit(1);

        }
        else if (pcForExecCurrCPFun >= 1)

        {
            // Parent process waits for the child process to complete
            char spForStrinCmdsTemporarliy[LENCMD_ST];
            int ppc = read(pipeForCommunicationPC[0], spForStrinCmdsTemporarliy,LENCMD_ST);

            spForStrinCmdsTemporarliy[ppc] = '\0';

        }
        else
        {   
            exit(1);

        }

    }

    return 0;

}

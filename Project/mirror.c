//Prableen Kaur Sachdeva

//Student ID: 110100670

//Sction:4



//This is having almost same functions in mirror as i have shown in server, so i have given detailed explnaatin in server already

//in this also mirror server this lbrary will be helping for socket-related  &structures functions

#include <sys/socket.h>



//For various data types

#include <sys/types.h>



//For directory entry manipulation

#include <dirent.h>



//For handling time-related functions

#include <time.h>



//For standard input& output operation

#include <stdio.h>



//For memory allocation & other standard functions

#include <stdlib.h>



//strcpy like funcs for string manipulation

#include <string.h>



//lby for using boolean data type and values

#include <stdbool.h>



//For various POSIX API functions, including file descriptor operations

#include <unistd.h>



//so like our main srver, in this mirror also ill be doing internet address manipulation, so its for that lbry

#include <arpa/inet.h>



//// provide functions & structures related to domain name resolution

#include <netdb.h>



//here im using it for internet protocol constants

#include <netinet/in.h>



////For signal handling

#include <sys/signal.h>



//For file status information

#include <sys/stat.h>



//For waiting for child processes

#include <sys/wait.h>



//For file control options

#include <fcntl.h>



//For error number definitions

#include <errno.h>



//here im using it for internet protocol constants

#include <netinet/ip.h>



//compare filenames against a pattern that contains wildcard characters like * - Matches any sequence of characters

//? - Matches any single character

#include <fnmatch.h>



//this is my port for mirror fixed

#define MYMIRRORTAKENPORT 1441



//so here it is the respond that our mirroer will be senting out to client and we have preddefined its value here

#define PERMITTED_MIRRORRESPONSE_SIZE 1024



//these are the total no. og argumnets we can write upto on our client to b handled by mirror

#define MR_PRMIT_ARGUMNTS 10



//so basically im defining this for miirror too as i declared for server so as to send whattype of response will be sentto client

#define MIRRORTEXTUALRSPONSE 1



//this is for determining to quit or not by mirror fn

#define MRRORRESPONSE_ISQUITORNOT 3





//________________________________________FGETS FUNCTION_____________________________________



//opens specified file, moves file pointer to EOF to determine its size,then closes file,If file opening encounters an error, function returns -1 to indicate error. Otherwise, it returns size of file in bytes

long int szeFleInBytesGvnFrM(const char *tkenFileNmeMr)

{

    /*"rb" mode is used,it stands for:

"r": "read" mod, which meansfile will be opened for reading

"b": mode stands fr "binary" mode,When "b" mode is included, it indicates that file will be treated as a binary file rather thantext file.*/

    

    //so raeson for getting to open is binry is  toensure that data is read or written exactly as it exists in file, without any character interpretatation or platform-specific transformations

    FILE *givnFlePnter = fopen(tkenFileNmeMr, "rb");

     

  // so our mirrorservr checkingwhether file opening was succeessful ornot, if givnFlePnter pointer =NULL, it means there was an error opening file

    if (givnFlePnter == NULL)

    {

        perror("Error opening file");

        

      //  printf("%d", givnFlePnter);

        

        // so it is just for some indiction  -1 to say error

        return -1; 

    }



// so now ill be just movin myfile pointer to my ennd of file

    fseek(givnFlePnter, 0, SEEK_END);  

     

    // get current position of file pointer, which is file size in bytes 

    long int size = ftell(givnFlePnter); 

    

    //it is necesary to close all filepointers after using as it will throw a erreor or bad file descriptor

    fclose(givnFlePnter);



//so it is having siz of  our specifieed file in bytes whuch we took aftr redaing whole file till end

    return size;

}



//ecursively traverses a directory and its subdirectories, searching for files that match given target filenames. When a match is found, full path of the matching file is written to a temporary file descriptor. This function is useful for collecting a list of paths to files that match specific criteria within a directory hierarchy

void mrrorRcsivlyTrvrseDirrctryForFgets(char *mrrorPaathofDirrectory, char **rcusrivlyFileNmeMrr, int num_files, int mrrorFiledesTempr)

{

    //opens the directory specified by mrrorPaathofDirrectory &returns a pointer to a DIR structure that represents directory. If directory cannot be opened, it returns NULL

    DIR *chkingdirctry = opendir(mrrorPaathofDirrectory);

    

    //so samly we are just gtting it for NULL

    if (chkingdirctry == NULL)

    {

        perror("There is some error in opening given directory");

        return;

    }

    

    // Declareing apointer to a struct dirent which holds informtionabout a directory entry

    struct dirent *mirrorEntryDirctoryInfo;

    

    //looping reading each directory entry (file or subdirectory) using readdir,loop continues until there are no more entries

    while ((mirrorEntryDirctoryInfo = readdir(chkingdirctry)) != NULL)

    {

        char flepaathfull[PERMITTED_MIRRORRESPONSE_SIZE];

        

        //creating full path of current file by combining mrrorPaathofDirrectory andd_name (name of current entry) using the / separator.resultnt is stored in flepaathfull array

        sprintf(flepaathfull, "%s/%s", mrrorPaathofDirrectory, mirrorEntryDirctoryInfo->d_name);



        // Check if current file matches any of specified filenames

        for (int fgh = 0; fgh < num_files; fgh++)

        {

            if (strcmp(mirrorEntryDirctoryInfo->d_name, rcusrivlyFileNmeMrr[fgh]) == 0)

            {

                write(mrrorFiledesTempr, flepaathfull, strlen(flepaathfull));

                

                // writes a newline character to temporary file descriptor. This separates paths of different matching files.

                write(mrrorFiledesTempr, "\n", 1);

                break;

            }

        }

        

        // checks if current entry is a subdirectory (DT_DIR), and it's not the current directory (.) or the parent directory (..).

        if (mirrorEntryDirctoryInfo->d_type == DT_DIR && strcmp(mirrorEntryDirctoryInfo->d_name, ".") != 0 && strcmp(mirrorEntryDirctoryInfo->d_name, "..") != 0)

        {

            mrrorRcsivlyTrvrseDirrctryForFgets(flepaathfull, rcusrivlyFileNmeMrr, num_files, mrrorFiledesTempr);

        }

    }

    

    //closeing directory once all entries have been processed

    closedir(chkingdirctry);

}



// like our server fn, this is excatly same thing doing:retrieves matching files from a specified directory &its subdirectories, compresses them into a tar archive, sendingarchive's size toclient, and sends archive's content to client. mthod uses child &parent processses, &temporary files for intermediate storage

void fgetsFunc(int mirrorSktClientConnectionFD, char **mrrorArrayyForCmmmdArgs, int argLen)

{

    // value of user's home directory retrieved using getenv function

    char *mrrorhmeUSRcrrnt = getenv("HOME");

    

    //variable to the name of the temporary compressed archive file.

    char *mrNmeOfFileCrted = "temp.tar.gz";



    // character array to temporarily hold data

    char buf[PERMITTED_MIRRORRESPONSE_SIZE];

    

    //Declares an array fd to hold two file descriptors

    int fd[2];

    

    //Declares a process ID variable

    pid_t mrrorNPrcss;



    // mkstemp replaces XXXXXX with just for one exampleim giving abc123, stemporary filename could become something like tempabc123. This ensures that each time the program runs, a new and unique temporary filename is generated, reducing the chance of naming conflicts with other files in the system.

    char mrIntrmediteFle[] = "tempXXXXXX";

    

    //so our main fnction of doing above that here and saving it to mrrorFiledesTempr 

    int mrrorFiledesTempr = mkstemp(mrIntrmediteFle);

    

    //if some issue encoeunterd so we'll just print it off

    if (mrrorFiledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }



//to search for files in specified directory and its subdirectories that match command arguments

    mrrorRcsivlyTrvrseDirrctryForFgets(mrrorhmeUSRcrrnt, mrrorArrayyForCmmmdArgs + 1, argLen - 1, mrrorFiledesTempr);



    //checks if temporary file is empty by attempting to seek to end of the file and checking if the current position is at the beginning. If it's true, no matching files were found.

    if (lseek(mrrorFiledesTempr, 0, SEEK_END) == 0)

    {

        // No files found, send "No file found" response

        int mirrorSentTypeofRespond = 1;

        

        //writingvalue of response_type to mirrorSktClientConnectionFD using write function. It sendsvalue to client so that client can understand the type of response being sent

        write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));

        

        //this string provides a message to client indicating that no files meeting the specified criteria were found.

        write(mirrorSktClientConnectionFD, "No file found", 13);

        

        //closeing mrrorFiledesTempr, which was opened earlier to storelist of matching files. Since there are no matching files in this case,temprary file doesn't need to be used any further, so it's closed.

        close(mrrorFiledesTempr);

        

        //useingunlink function to delete temporary file that was created earlier with mkstemp fun. Since no files were found, &temporary file isn't needed anymore, it's removed from the filesystem

        unlink(mrIntrmediteFle);

        

        //just getting returned_

        return;

    }



    // Fork a child process to handle the tar and zip operation

    if (pipe(fd) < 0)

    {

        perror("Error creating pipe");

        return;

    }

     mrrorNPrcss = fork();

    if (mrrorNPrcss < 0)

    {

        perror("Error forking child process");

        return;

    }

    

    //here mirror again strting a new child prc to do below commnd

    else if (mrrorNPrcss == 0)

    {

        //s the read end of pipe becozchild process is not going to read anything from pipe

        close(fd[0]);

        

        // duplicates write end of pipe to standard output file descriptor,redirecting its output

        dup2(fd[1], STDOUT_FILENO);

        

        // closingoriginal write end of pipe

        close(fd[1]);

        

        // child process executes tar command to compress files specified in tmp_file into a single archive named filename,-czf flags indicate compression and specifying the archive filename.

        execlp("tar", "tar", "-czf", mrNmeOfFileCrted, "-T", mrIntrmediteFle, NULL);

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

        // Parent process

        close(fd[1]);

        // Send file response

        int mirrorSentTypeofRespond = 2;

        

        //parent process sends a response type to the client indicating that a file response is coming

        write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));



//parent process waits for child process (handling tar operation) to finish

        wait(NULL);

        

        //lik we have done in server, here also  flushing standard output buffer

        fflush(stdout);

        long int filesize = szeFleInBytesGvnFrM("temp.tar.gz");

        write(mirrorSktClientConnectionFD, &filesize, sizeof(filesize));



        // Delete temporary file

        unlink(mrIntrmediteFle);



        // Send tar file to client

        int mrFileDescritorForTarFile = open(mrNmeOfFileCrted, O_RDONLY);

        if (mrFileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }

        ssize_t mrrorBytesReading, sentingBytesToCTFromMirror;

        off_t afterRemainBytesInmirror = filesize;

        while (afterRemainBytesInmirror > 0 && (mrrorBytesReading = read(mrFileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

        

        //send function to send data tobufferto client socket identified, number of bytes sent is determined by mrrorBytesReading, which is the number of bytes read from the tar archive file in previous read operation

            sentingBytesToCTFromMirror = send(mirrorSktClientConnectionFD, buf, mrrorBytesReading, 0);

            if (sentingBytesToCTFromMirror < 0)

            {

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInmirror -= sentingBytesToCTFromMirror;

        }

        close(mrFileDescritorForTarFile);



        // Delete taar file

        unlink(mrNmeOfFileCrted);

    }

}





//________________________FILESERCH OPERATION_____________________________



//searches for a file in the user's home directory using the find command, retrieves file information usingstat function, and constructs a response message based on the search results. The response message is then sent back to the client socket

void mirrorFilesrchOpertion(int mirrorSktClientConnectionFD, char **mrrorArrayyForCmmmdArgs)

{



//declare a variable filename to store the name of the file that needs to be found, and an array response to store the response message that will be sent back to the client

    char *srchFleNmeMr = mrrorArrayyForCmmmdArgs[1];

    char mirrorSentingrespseToCT[1024];



//getting home directory path of user using the getenv function, which retrieves the value of the specified environment variable. In this case, it's getting the home directory path

    char *directoryOfUserHome = getenv("HOME");  

    

    //dynamically allocates memory to store the command string that will be used to search for the file. The size of the memory allocation is calculated based on the lengths of the home directory path, filename, and additional characters in the command.                                       

    char *mrrorContructedcmmand = (char *)malloc(strlen(directoryOfUserHome) + strlen(srchFleNmeMr) + 27); 

    

    // constructs the find command that will be executed to search for the specified file in the user's home directory. The command is constructed using the sprintf function, which formats and stores the command in the previously allocated memory

    sprintf(mrrorContructedcmmand, "find %s -name '%s' -print -quit", directoryOfUserHome, srchFleNmeMr);  

    

    //opens a pipe to the command using the popen function,pipe allows the output of the command to be read by the program. The mode "r" indicates that the pipe will be used for reading

    FILE *mrCmmnicationPipe = popen(mrrorContructedcmmand, "r");                                         

    if (mrCmmnicationPipe != NULL)

    {

        char line[256];

        

        //reads the first line of output from the pipe using the fgets function. It checks if the read operation was successful and if there is any output.

        if (fgets(line, sizeof(line), mrCmmnicationPipe) != NULL)

        {    

        //emoves newline character ('\n') from EOL string combined with array indexing,This is done to format the output properly                                

            line[strcspn(line, "\n")] = '\0'; 

            

       //uses stat function to retrieve file information (such as size and creation time) for file indicated by line variable. If stat function returns 0, it meansfile information was successfully retrieved

            struct stat sb;

            if (stat(line, &sb) == 0)

            {    

      //formats respone mesge using retrieved file information. It includes name of the file), its size (sb.st_size), and its creation time (sb.st_ctime), which is formatted using ctime

                sprintf(mirrorSentingrespseToCT, "Name of file: %s\nSize: %lld bytes,\nCreated on: %s", srchFleNmeMr, (long long)sb.st_size, ctime(&sb.st_mtime));

            }

            else

            {

       //If stat function fails (returns a value other than 0), this line generates a response message indicating that file information could not be retrieved

                sprintf(mirrorSentingrespseToCT, "Unable to get file information for %s\n", line);

            }

        }

        else

        {

      //If the fgets function didn't read any output from the pipe, this line generates a response message indicating that the file was not found.

            sprintf(mirrorSentingrespseToCT, "File not found\n");

        }

        

        // closes the pipe that was opened using popen

        pclose(mrCmmnicationPipe);

    }

    else

    {

        printf("There might be some error in opening pipe to command\n");

    }

    free(mrrorContructedcmmand); // Free the memory allocated for the command string



    // sending response message back to client socket. sends an integer indicating the response type (in this case, it's a text response)

    int mirrorSentTypeofRespond = MIRRORTEXTUALRSPONSE;

    write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));



//sending actual response message stored in response array

    write(mirrorSktClientConnectionFD, mirrorSentingrespseToCT, strlen(mirrorSentingrespseToCT));

}







//______________________TARFGETZ MIRROR OPERATION________________________________________





//same thing like servor: checks whether a file's size, located at given path, falls within a specified range (srverSideFleSZ1 to srverSideFleSZ1). It uses stat function to retrieve file information, checks if file is a regular file, and then compares its size against specified range,function returns true if file meets size criteria &false otherwise

bool rngeszeFrFlieMrr(char *path, size_t mrrorSideFleSZ1, size_t mrrorSideFleSZ2)

{

    struct stat st;

    if (stat(path, &st) == 0 && S_ISREG(st.st_mode) && st.st_size >= mrrorSideFleSZ1 && st.st_size <= mrrorSideFleSZ2)

    {

        return true;

    }

    return false;

}



//same like our srver://recursivelytraverses through a directory and its subdirectories, checking size of files along way. It uses readdir function to read directory entries, constructs full paths, checks file sizes, & uses recursion to traverse subdirectories

void mrrorRcsivlyTrvrseDirrctry(char *mrrorPaathofDirrectory, size_t mrrorSideFleSZ1, size_t mrrorSideFleSZ2, int mrrorFiledesTempr)

{

    DIR *dir = opendir(mrrorPaathofDirrectory);

    if (dir == NULL)

    {

        perror("Error opening directory");

        return;

    }

    struct dirent *mirrorEntryDirctoryInfo;

    while ((mirrorEntryDirctoryInfo = readdir(dir)) != NULL)

    {

        char path[PERMITTED_MIRRORRESPONSE_SIZE];

        sprintf(path, "%s/%s", mrrorPaathofDirrectory, mirrorEntryDirctoryInfo->d_name);

        if (rngeszeFrFlieMrr(path, mrrorSideFleSZ1, mrrorSideFleSZ2))

        {

            write(mrrorFiledesTempr, path, strlen(path));

            write(mrrorFiledesTempr, "\n", 1);

        }

        if (mirrorEntryDirctoryInfo->d_type == DT_DIR && strcmp(mirrorEntryDirctoryInfo->d_name, ".") != 0 && strcmp(mirrorEntryDirctoryInfo->d_name, "..") != 0)

        {

            mrrorRcsivlyTrvrseDirrctry(path, mrrorSideFleSZ1, mrrorSideFleSZ2, mrrorFiledesTempr);

        }

    }

    closedir(dir);

}





//so like our server, this is again doin same thing, comapring sizes fr fiel between range or not, if it is then t willl create a tar

void tarfgetz(int mirrorSktClientConnectionFD, char **mszeArgmnets)

{

    //agin like my main srvr, it is getting home dirtory

    char *mrrorhmeUSRcrrnt = getenv("HOME");

    

    //then created fiel name

    char *mrNmeOfFileCrted = "temp.tar.gz";

    

    //then my files inputted size

    size_t mrrorSideFleSZ1 = atoi(mszeArgmnets[1]);

    

    //next file inpuuted 

    size_t mrrorSideFleSZ2 = atoi(mszeArgmnets[2]);



//now lets declaring a same thing fr storing mirror response

    char buf[PERMITTED_MIRRORRESPONSE_SIZE];

    

    //this is my fle desciptor here to communicate over pipe

    int fd[2];

    

    //now im crating new prc

    pid_t mrrorNPrcss;



    // Creatinga temporary file to store t list of files

    char mrIntrmediteFle[] = "tempXXXXXX";

    

    //so it is here a genral dscriptor for my file

    int mrrorFiledesTempr = mkstemp(mrIntrmediteFle);

    if (mrrorFiledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }



    // Traverse directory tree &write filenames

    mrrorRcsivlyTrvrseDirrctry(mrrorhmeUSRcrrnt, mrrorSideFleSZ1, mrrorSideFleSZ2, mrrorFiledesTempr);



    // Fork a child process to handle the tar and zip operation

    if (pipe(fd) < 0)

    {

        perror("Error creating pipe");

        return;

    }

    mrrorNPrcss = fork();

    if (mrrorNPrcss < 0)

    {

        perror("Error forking child process");

        return;

    }

    else if (mrrorNPrcss == 0)

    {

        // Child process

        close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);

        close(fd[1]);

        execlp("tar", "tar", "-czf", mrNmeOfFileCrted, "-T", mrIntrmediteFle, NULL);

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

        // Parent process

        close(fd[1]);

        // Send file response

        int mirrorSentTypeofRespond = 2;

        write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));



        wait(NULL);

        fflush(stdout);

        long int filesize = szeFleInBytesGvnFrM("temp.tar.gz");

        write(mirrorSktClientConnectionFD, &filesize, sizeof(filesize));



        // Delete the temporary file

        unlink(mrIntrmediteFle);



        // Send tar file to client

        int mrFileDescritorForTarFile = open(mrNmeOfFileCrted, O_RDONLY);

        if (mrFileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }

        ssize_t mrrorBytesReading, sentingBytesToCTFromMirror;

        off_t afterRemainBytesInmirror = filesize;

        while (afterRemainBytesInmirror > 0 && (mrrorBytesReading = read(mrFileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

            sentingBytesToCTFromMirror = send(mirrorSktClientConnectionFD, buf, mrrorBytesReading, 0);

            if (sentingBytesToCTFromMirror < 0)

            {

                //printf(afterRemainBytesInmirror);

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInmirror -= sentingBytesToCTFromMirror;

        }

        //printf(afterRemainBytesInmirror);

        close(mrFileDescritorForTarFile);



        // Delete tar file

        unlink(mrNmeOfFileCrted);

    }

}



//________________________________________getdirf______________________________________





//same like we have done in our servor, checking a file's modification time, located at given path, falls within a specified time range (gvnT1 to gvnT2)stat function to retrieve file information, checks if file is a regular file, and then compaaribg its modification time againstset specified timegvnrange

bool vrifydtemrr(char *fileLooction, time_t gvnT1, time_t gvnT2)

{

      //here im declring a strcture type varible 

    struct stat cndition;

    

    //now here i have taken one ternry opertor which is ching 3 conditions simultatnously ie if file info is successfuly retrieved and if file is a regulr file (nota dirctory type ) and if that particular file which is in check is falling between givn time period

 return (stat(fileLooction, &cndition) == 0 && S_ISREG(cndition.st_mode) && cndition.st_mtime >= gvnT1 && cndition.st_mtime <= gvnT2) ? true : false;

}





// traverses through a directory and its subdirectories, checking modification time of files,It constructs full paths, writes paths to a file descriptor if they meet time givn, & recursively traverses subdirectories

void mrrorRcsivlyTrvrseDirrctryForDate(char *mrrorPaathofDirrectory, time_t gvnT1, time_t gvnT2, int mrrorFiledesTempr)

{

    DIR *dir = opendir(mrrorPaathofDirrectory);

    if (dir == NULL)

    {

        perror("There is some problem in opening the directory");

        return;

    }

    struct dirent *mirrorEntryDirctoryInfo;

    while ((mirrorEntryDirctoryInfo = readdir(dir)) != NULL)

    {

        char path[PERMITTED_MIRRORRESPONSE_SIZE];

        sprintf(path, "%s/%s", mrrorPaathofDirrectory, mirrorEntryDirctoryInfo->d_name);

        if (vrifydtemrr(path, gvnT1, gvnT2))

        {

            write(mrrorFiledesTempr, path, strlen(path));

            write(mrrorFiledesTempr, "\n", 1);

        }

        if (mirrorEntryDirctoryInfo->d_type == DT_DIR && strcmp(mirrorEntryDirctoryInfo->d_name, ".") != 0 && strcmp(mirrorEntryDirctoryInfo->d_name, "..") != 0)

        {

            mrrorRcsivlyTrvrseDirrctryForDate(path, gvnT1, gvnT2, mrrorFiledesTempr);

        }

    }

    closedir(dir);

}



//parsing date arguments, creating a temporary file, traversing directories to find files within a specific date range, using pipes for inter-process communication, forking a child process to execute a tar command, and sending the compressed file to the client.

void mirrorOprtionGetdirfFn(int mirrorSktClientConnectionFD, char **mrrorArrayyForCmmmdArgs)

{

    //again Get user's home directory path

    char *mrrorhmeUSRcrrnt = getenv("HOME");

    

    //Name of temporary tar archive file

    char *mrNmeOfFileCrted = "temp.tar.gz";

    

    //Get first date argument

    char *mrrorDate1Strng = mrrorArrayyForCmmmdArgs[1];

    

    // Get 2nd date argument

    char *mrrorDate2Strng = mrrorArrayyForCmmmdArgs[2];



//Convert date strings to struct tm and adjust values

    struct tm mrrorDate1, mrrorDate2;

    

    //inpuuted Parsing year, month, day

    sscanf(mrrorDate1Strng, "%d-%d-%d", &mrrorDate1.tm_year, &mrrorDate1.tm_mon, &mrrorDate1.tm_mday);

    

    // inpuuted Parsing year, month, day

    sscanf(mrrorDate2Strng, "%d-%d-%d", &mrrorDate2.tm_year, &mrrorDate2.tm_mon, &mrrorDate2.tm_mday);

    

    // Adjust year to be years since 1900

    mrrorDate1.tm_year -= 1900;

    

    //printf("%d",mrrorDate1.tm_mon);

    mrrorDate1.tm_mon -= 1;

    

    //printf("%d",mrrorDate1.tm_hour);

    mrrorDate1.tm_hour = 0;

    

    // Set  minute to 0

    mrrorDate1.tm_min = 0;

    

    // Set second to 0

    mrrorDate1.tm_sec = 0;

    

    // Similar adjustments for the second date

    mrrorDate2.tm_year -= 1900;

    

    //we are subtrcating by 1 is necessary becz many C libraries use a 0-based indexing system for months 

    mrrorDate2.tm_mon -= 1;

    

    ////printf("%d",mrrorDate2.tm_mon);

    //mrrorDate2.tm_mon

    mrrorDate2.tm_hour = 23;

    

    //By setting tm_min to 59, code ensures that end time is accurately represented at last second of specified day

    mrrorDate2.tm_min = 59;

    

    //By setting tm_sec to 59, the code ensures that the end time is accurately represented at the last second of the specified day.

    mrrorDate2.tm_sec = 59;

    

    // Convert struct tm to time_t

    time_t date1 = mktime(&mrrorDate1);

    

    // Convert the adjusted date struct to time_t

    time_t date2 = mktime(&mrrorDate2);



    // Buffer for reading file data

    char buf[PERMITTED_MIRRORRESPONSE_SIZE];

    

    // Pipe file descriptors

    int fd[2];

    

    // Process ID for forking

    pid_t mrrorNPrcss;



    // here again like server, Creating a tmporary file to store list of files

    char mrIntrmediteFle[] = "tempXXXXXX";

    

    //

    int mrrorFiledesTempr = mkstemp(mrIntrmediteFle);

    

    //

    if (mrrorFiledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }



    // Traverse directory tree and write filenames

    mrrorRcsivlyTrvrseDirrctryForDate(mrrorhmeUSRcrrnt, date1, date2, mrrorFiledesTempr);



    // Fork a child process to handle tar &zip operation

    if (pipe(fd) < 0)

    {

        perror("Error creating pipe");

        return;

    }

    mrrorNPrcss = fork();

    if (mrrorNPrcss < 0)

    {

        perror("Error forking child process");

        return;

    }

    else if (mrrorNPrcss == 0)

    {

        // Child process

        close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);

        close(fd[1]);

        execlp("tar", "tar", "-czf", mrNmeOfFileCrted, "-T", mrIntrmediteFle, NULL);

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

        // Parent process

        close(fd[1]);



        // Send file response

        int mirrorSentTypeofRespond = 2;

        write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));



// Wait for child process to finish

        wait(NULL);

        

      // Flush the standard output buffer



        fflush(stdout);

        

        // Get the size of the tar file

        long int filesize = szeFleInBytesGvnFrM("temp.tar.gz");

        

        //here ill be Sending file size to client

        write(mirrorSktClientConnectionFD, &filesize, sizeof(filesize));



        // Delete temporary file

        unlink(mrIntrmediteFle);



        // Send tar file to client

        int mrFileDescritorForTarFile = open(mrNmeOfFileCrted, O_RDONLY);

        if (mrFileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }

        ssize_t mrrorBytesReading, sentingBytesToCTFromMirror;

        off_t afterRemainBytesInmirror = filesize;

        while (afterRemainBytesInmirror > 0 && (mrrorBytesReading = read(mrFileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

            sentingBytesToCTFromMirror = send(mirrorSktClientConnectionFD, buf, mrrorBytesReading, 0);

            if (sentingBytesToCTFromMirror < 0)

            {

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInmirror -= sentingBytesToCTFromMirror;

        }

        close(mrFileDescritorForTarFile);



        // Delete tar file

        unlink(mrNmeOfFileCrted);

    }

}





//_____________________________MIRRORtargzf OPERATION___________________________



void mrrorRcsivlyTrvrseDirrctryFrTargzf(char *mrrorPaathofDirrectory, char **extensions, int num_extensions, int mrrorFiledesTempr)

{

    DIR *dir = opendir(mrrorPaathofDirrectory);

    if (dir == NULL)

    {

        perror("Error opening directory");

        return;

    }

    struct dirent *mirrorEntryDirctoryInfo;

    while ((mirrorEntryDirctoryInfo = readdir(dir)) != NULL)

    {

        char path[PERMITTED_MIRRORRESPONSE_SIZE];

        sprintf(path, "%s/%s", mrrorPaathofDirrectory, mirrorEntryDirctoryInfo->d_name);



        // Check if the current file matches any of the specified filenames

        char *ext = strrchr(mirrorEntryDirctoryInfo->d_name, '.');

        if (ext != NULL && ext != mirrorEntryDirctoryInfo->d_name && strlen(ext) <= 5)

        {

            ext++; // skip the dot character

            for (int i = 0; i < num_extensions; i++)

            {

                if (strcmp(ext, extensions[i]) == 0)

                {

                    write(mrrorFiledesTempr, path, strlen(path));

                    write(mrrorFiledesTempr, "\n", 1);

                    break;

                }

            }

        }

        if (mirrorEntryDirctoryInfo->d_type == DT_DIR && strcmp(mirrorEntryDirctoryInfo->d_name, ".") != 0 && strcmp(mirrorEntryDirctoryInfo->d_name, "..") != 0)

        {

            mrrorRcsivlyTrvrseDirrctryFrTargzf(path, extensions, num_extensions, mrrorFiledesTempr);

        }

    }

    closedir(dir);

}



//creates a compressed archive containing files from a specified directory and sends the archive to a client over a socket connection. It uses pipes, forking, and various file operations to achieve this.

void mirrorOprtiontargzfFn(int mirrorSktClientConnectionFD, char **mrrorArrayyForCmmmdArgs, int argLen)

{

    //stores the user's home directory path obtained using the getenv function, and filename is set to the desired name for the resulting compressed archive file.

    char *mrrorhmeUSRcrrnt = getenv("HOME");

    char *mrNmeOfFileCrted = "temp.tar.gz";



    char buf[PERMITTED_MIRRORRESPONSE_SIZE];

    int tmflds[2];

    pid_t mrrorNPrcss;



    // Create a temporary file to store the list of files

    char mrIntrmediteFle[] = "tempXXXXXX";

    int mrrorFiledesTempr = mkstemp(mrIntrmediteFle);

    if (mrrorFiledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }

    mrrorRcsivlyTrvrseDirrctryFrTargzf(mrrorhmeUSRcrrnt, mrrorArrayyForCmmmdArgs + 1, argLen - 1, mrrorFiledesTempr);



    // Check if any files were found

    if (lseek(mrrorFiledesTempr, 0, SEEK_END) == 0)

    {

        // No files found, send "No file found" response

        int mirrorSentTypeofRespond = 1;

        write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));

        write(mirrorSktClientConnectionFD, "No file found", 13);

        close(mrrorFiledesTempr);

        unlink(mrIntrmediteFle);

        return;

    }



    // Fork a child process to handle the tar and zip operation

    if (pipe(tmflds) < 0)

    {

        perror("Error creating pipe");

        return;

    }

    mrrorNPrcss = fork();

    if (mrrorNPrcss < 0)

    {

        perror("Error forking child process");

        return;

    }

    else if (mrrorNPrcss == 0)

    {

       /*child ps closes read end of pipe (tmflds[0]), redirects its standard otp to write end of pipe using dup2, and then executes tar command to creat compressed archive.-czf options is abt compression &file naming.archive is created using the list of files specified in the temporary file.

       */



        close(tmflds[0]);

        dup2(tmflds[1], STDOUT_FILENO);

        close(tmflds[1]);

        execlp("tar", "tar", "-czf", mrNmeOfFileCrted, "-T", mrIntrmediteFle, NULL);

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

        // Parent process

        close(tmflds[1]);



        // Send file response

        int mirrorSentTypeofRespond = 2;

        write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));



        wait(NULL);

        fflush(stdout);

        long int filesize = szeFleInBytesGvnFrM("temp.tar.gz");

        write(mirrorSktClientConnectionFD, &filesize, sizeof(filesize));



        // Delete temporary file

        unlink(mrIntrmediteFle);



        // Send tar file to client

        int mrFileDescritorForTarFile = open(mrNmeOfFileCrted, O_RDONLY);

        if (mrFileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }



        ssize_t mrrorBytesReading, sentingBytesToCTFromMirror;

        off_t afterRemainBytesInmirror = filesize;

        while (afterRemainBytesInmirror > 0 && (mrrorBytesReading = read(mrFileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

            sentingBytesToCTFromMirror = send(mirrorSktClientConnectionFD, buf, mrrorBytesReading, 0);

            if (sentingBytesToCTFromMirror < 0)

            {

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInmirror -= sentingBytesToCTFromMirror;

        }

        close(mrFileDescritorForTarFile);



        // Delete tar file

        unlink(mrNmeOfFileCrted);

    }

}





//This is again a string manipulation fn which im using just like srver in my mirror code also, it just removes any newliene chrcaters from lime of argmnsts which is there in commmand that we are executing in client side

void mrStringManipulationForRmvingNewLine(char **mirrorCmdStringTkns, int mirrorTotalCountOfTkns)

{

    //iterates through each word in array of words/tkns

    for (int mmr = 0; mmr < mirrorTotalCountOfTkns; mmr++)

    {

        //current token at index i is assigned to the pointer token

        char *mirrorTken = mirrorCmdStringTkns[mmr];

        

   //calculates length of token until 1stoccurrence of a newlinne chracter ('\n'). strcspn funct returns lengh of initial segment of the string that doesnt contain specified characters

        int gnSze = strcspn(mirrorTken, "\n");

        

        //Memory is allocated dynamically to create a new string createdTknNw with a length of gnSze + 1

        // additional byte is for null terminator '\0'

        char *createdTknNw = (char *)malloc(gnSze + 1);

        

       //copingcharacters from original token to the new token, excluding the newline character

        strncpy(createdTknNw, mirrorTken, gnSze);

        

        //null terminator is added at the end of the new token to ensure it is a valid null-terminated string

        createdTknNw[gnSze] = '\0';

        

        // new token is assigned to the original token array, replacing original token with new one

        mirrorCmdStringTkns[mmr] = createdTknNw;

    }

}



//_________________________________PROCESSCLIENT___________________________________________



//

void processClient(int mirrorSktClientConnectionFD)

{

    char mrCmmdHandling[255];

    char mirrorSentingrespseToCT[1024];

    int n, mrrorNPrcss;

    write(mirrorSktClientConnectionFD, "Send commands", 14);



    while (true)

    {

        memset(mrCmmdHandling, 0, sizeof(mrCmmdHandling));

        n = read(mirrorSktClientConnectionFD, mrCmmdHandling, 255);



        if (n <= 0)

        {

            printf("Client disconnected.\n");

            break; // Exit loop and terminate processclient() function

        }



        char *mrrorArrayyForCmmmdArgs[MR_PRMIT_ARGUMNTS];

        int mrCntOfArgmnts = 0;



        // here imParsing command received from client & Tokenize command using space as delimiter

        char *token = strtok(mrCmmdHandling, " "); 



        while (token != NULL)

        {

            mrrorArrayyForCmmmdArgs[mrCntOfArgmnts++] = token; 

            token = strtok(NULL, " ");         

        }

        mrrorArrayyForCmmmdArgs[mrCntOfArgmnts] = NULL; // Set the last element of array to NULL



        // Remove line breaks from tokens

        mrStringManipulationForRmvingNewLine(mrrorArrayyForCmmmdArgs, mrCntOfArgmnts);



        char *cmd = mrrorArrayyForCmmmdArgs[0]; // Extract first token as the command



        printf("\nExecuting the following command: \n");

        for (int i = 0; i < mrCntOfArgmnts; i++)

        {

            printf("%s ", mrrorArrayyForCmmmdArgs[i]);

        }

        printf("\n\n");



        // here below ,like our server, mirror is als processing out commands

        if (strcmp(cmd, "filesrch") == 0)

        {

            //here ill be calling our filerch oprtion and mirror will be handlin it

            mirrorFilesrchOpertion(mirrorSktClientConnectionFD, mrrorArrayyForCmmmdArgs);

        }

        else if (strcmp(cmd, "tarfgetz") == 0)

        {

            //here ill be calling our tarfgetz oprtion and mirror will be handlin it

            tarfgetz(mirrorSktClientConnectionFD, mrrorArrayyForCmmmdArgs);

        }

        else if (strcmp(cmd, "getdirf") == 0)

        {

            //here ill be calling our getdirf oprtion and mirror will be handlin it

            mirrorOprtionGetdirfFn(mirrorSktClientConnectionFD, mrrorArrayyForCmmmdArgs);

        }

        else if (strcmp(cmd, "fgets") == 0)

        {

            //here ill be calling our fgets oprtion and mirror will be handlin it

            fgetsFunc(mirrorSktClientConnectionFD, mrrorArrayyForCmmmdArgs, mrCntOfArgmnts);

        }

        else if (strcmp(cmd, "targzf") == 0)

        {

            //here ill be calling our targzf oprtion and mirror will be handlin it

            mirrorOprtiontargzfFn(mirrorSktClientConnectionFD, mrrorArrayyForCmmmdArgs, mrCntOfArgmnts);

        }

        else if (strcmp(cmd, "quit") == 0)

        {

            printf("Client requested to quit.\n");

            

            //here ill be calling our quit oprtion and mirror will be handlin it

            int mirrorSentTypeofRespond = MRRORRESPONSE_ISQUITORNOT;

            

            //

            write(mirrorSktClientConnectionFD, &mirrorSentTypeofRespond, sizeof(mirrorSentTypeofRespond));

            

            //we will be just exiting from our fnction to execute this cooomand

            break; 

        }

        else

        {

        //so here mirror will be senting out a reply to client that provided command is not valid

            snprintf(mirrorSentingrespseToCT, PERMITTED_MIRRORRESPONSE_SIZE, "Invalid command\n"); 

            

            //simply writing to ct with skt

            write(mirrorSktClientConnectionFD, mirrorSentingrespseToCT, strlen(mirrorSentingrespseToCT));

            

            //// Continue to next iteration of loop to wait for new command

            continue; 

        }

    }

    exit(0);

}



//_________________OUR MAIN MIRROR METHOD_____________________________________



int main(int argc, char *argv[])

{

    int mrDscrptorSkt, mrStClSkDstp, status;

    socklen_t len;

    struct sockaddr_in servAdd; // ipv4



    if ((mrDscrptorSkt = socket(AF_INET, SOCK_STREAM, 0)) < 0)

    {

        printf("Mirror: Error in creating socket for communication\n");

        exit(1);

    }



    // Add information to the servAdd struct variable of type sockaddr_in

    servAdd.sin_family = AF_INET;

    // When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces.

    // The htonl function takes a 32-bit number in host byte order and returns a 32-bit number in the network byte order used in TCP/IP networks

    servAdd.sin_addr.s_addr = htonl(INADDR_ANY); // Host to network long



    // htonos: Host to network short-byte order

    servAdd.sin_port = htons(MYMIRRORTAKENPORT);



    // struct sockaddr is the generic structure to store socket addresses

    // The procedure it to typecast the specific socket addreses of type sockaddr_in, sockaddr_in6, sockaddr_un into sockaddr



    bind(mrDscrptorSkt, (struct sockaddr *)&servAdd, sizeof(servAdd));

    listen(mrDscrptorSkt, 5);



    printf("Mirror server has been started, waiting for client... !\n");



    while (true)

    {

        mrStClSkDstp = accept(mrDscrptorSkt, (struct sockaddr *)NULL, NULL);

        printf("Connected to a Client\n");

        if (!fork()) // Child process

            processClient(mrStClSkDstp);

        close(mrStClSkDstp);

        waitpid(0, &status, WNOHANG);

    }

}


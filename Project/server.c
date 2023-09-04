//ASP Project Final Server File

//Prableen Kaur Sachdeva

//Student ID: 110100670

//Sction:4



//For internet protocol header structure

#include <netinet/ip.h>



// For filename matching

#include <fnmatch.h>



//For signal handling

#include <sys/signal.h>



//For file status information

#include <sys/stat.h>



// For waiting for child processes

#include <sys/wait.h>



// For file control options

#include <fcntl.h>



//For error number definitions

#include <errno.h>



//For handling time-related functions

#include <time.h>



//For standard input and output operations

#include <stdio.h>



// For memory allocation and other standard functions

#include <stdlib.h>



//For string manipulation functions

#include <string.h>



// For using boolean data type and values

#include <stdbool.h>



//For various POSIX API functions, including file descriptor operations

#include <unistd.h>



//later ill be doing internet address manipulation, so its for that lbry

#include <arpa/inet.h>



// provide functions & structures related to domain name resolution

#include <netdb.h>



//here im using it for internet protocol constants

#include <netinet/in.h>



//for socket-related functions &structures

#include <sys/socket.h>



//For various data types

#include <sys/types.h>



//For directory entry manipulation

#include <dirent.h>



//port number on which server will be listening for ourclient connections

#define MYSERVERTAKENPORT 1440



//taken reponse size to client

#define TKENRSPONSESZE 1024



//i've taken argument as10 as it is reprsting the length of coommand len argumnets

#define PRMIT_ARGUMNTS 10



//so it is a fixed macro which is helping in ditinguishing whether servor is sending a noormal reposne like text format or a file 

#define TEXTUALRSPONSEBYSRVER 1



//for executing our quit commaand by user on client

#define SRVERRESPONSE_ISQUITORNOT 3



//so this is a varieble that is there to get us about how many currrennt cleints are bieng coonected 

int noOfCleintCnnctedLve = 0;



//________________FILESEARCH OPERATION____________________________



//searches for a given filename in user's home directory using find command. It then retrieves file information using stat fucion and constructs a response message containing details about file. Finally, it sends response back to a client over a socket connection

void opertionFrFilesrchFn(int skCmmctionCTFD, char **flNmeArgmt)

{



//getting argument from user as fil name will be givn in this argumnet 

    char *csoleFlFrmU = flNmeArgmt[1];

    

    //a file that is searched for in user's home directory, this information is then sent as a response back to client over a socket connection. response contains details about file, including its name, size, and creation time

    char cmmctionRspToClnt[1024];



//// Get home directory path

    char *dirOfHmeOfUsr = getenv("HOME");  

    

         

      // Allocate memory for command string   

    char *frshcmmd = (char *)malloc(strlen(dirOfHmeOfUsr) + strlen(csoleFlFrmU) + 27); 

    

    // Construct find command

    sprintf(frshcmmd, "find %s -name '%s' -print -quit", dirOfHmeOfUsr, csoleFlFrmU); 

    

    // 

    FILE *pipe = popen(frshcmmd, "r");  

     

       // Check if pipe was opened successfully

    if (pipe != NULL)

    {

        char opFrmPp[256];

        

        // Read 1st line of output from pipe

        if (fgets(opFrmPp, sizeof(opFrmPp), pipe) != NULL)

        {       

             // Remove newline character from end of line

            opFrmPp[strcspn(opFrmPp, "\n")] = '\0'; 

            struct stat sb;

            // Get file information using stat() function

            if (stat(opFrmPp, &sb) == 0)

            {     

            // Get file information using stat()

                sprintf(cmmctionRspToClnt, "Name of file: %s\nSize: %lld bytes\nCreated on: %s", csoleFlFrmU, (long long)sb.st_size, ctime(&sb.st_mtime)); 

            }

            else

            {

            // If unable to get file information, construct an error message

                sprintf(cmmctionRspToClnt, "Unable to get file information for %s\n", opFrmPp);

            }

        }

        else

        {

        // If find command didn't produce any output, construct a "File not found" mssage

            sprintf(cmmctionRspToClnt, "File not found\n");

        }

        

        // Close pipe

        pclose(pipe); 

    }

    else

    {

        printf("Error opening pipe to command\n");

    }

    // Free memory allocated for command string

    free(frshcmmd); 



    //initializes an integer variable typeOfRespponseToBeSent with value of TEXTUALRSPONSEBYSRVER.purpose of this variable is to indicate type of response that is being sent to client

    int typeOfRespponseToBeSent = TEXTUALRSPONSEBYSRVER;

    write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));



// writes contents of response array tosocket specified by skCmmctionCTFD descrptor,response array hold text-based informtn abt file search results. response is a null-terminated string, so strlen(response) is used to determine length of string, write function sends this data over socket to client

    write(skCmmctionCTFD, cmmctionRspToClnt, strlen(cmmctionRspToClnt));

}





// This function takes a file name as input and returns size of file in bytes

long int szeFleInBytesGvn(const char *gnNmeofFl)

{

// open file in binary mode

    FILE *fp = fopen(gnNmeofFl, "rb"); 

    

    // Check if file was opened successfully

    if (fp == NULL)

    {

        perror("Error opening file");

        

        //// Return -1 to indicate an error

        return -1; 

    }



// move file pointer to end of file

    fseek(fp, 0, SEEK_END); 

    

     // Get current position of file pointer, which is file size in bytes

    long int size = ftell(fp); 

    

    // Close file

    fclose(fp);                



// Return file size

    return size; 

}







//_______________________GETDIRF OPERATION___________________________________



//checks whether a file's modification time, located at given path, falls within a specified time range (gvnT1 to gvnT2). It uses stat function to retrieve file information, checks if file is a regular file, and then compares its modification time againset specified time range

bool dateVrification(char *fileLooction, time_t gvnT1, time_t gvnT2)

{

    //here im declring a strcture type varible 

    struct stat cndition;

    

    //now here i have taken one ternry opertor which is ching 3 conditions simultatnously ie if file info is successfuly retrieved and if file is a regulr file (nota dirctory type ) and if that particular file which is in check is falling between givn time period

 return (stat(fileLooction, &cndition) == 0 && S_ISREG(cndition.st_mode) && cndition.st_mtime >= gvnT1 && cndition.st_mtime <= gvnT2) ? true : false;

}





// traverses through a directory and its subdirectories, checking modification time of files,It constructs full paths, writes paths to a file descriptor if they meet time givn, & recursively traverses subdirectories

void rcsivlyTrvrseDirrctryFrDATE(char *paathofDirrectory, time_t gvnT1, time_t gvnT2, int fledesTempr)

{



//hrere im  openinng directory for traversal

    DIR *dir = opendir(paathofDirrectory);

    

    //if no directory is there

    if (dir == NULL)

    {

        perror("There is some problem in opening the directory");

        return;

    }

    

    // Declare a pointer to hold directory entry information

    struct dirent *cmngEntryinDirctory;

    

    //Loop through each directory entry

    while ((cmngEntryinDirctory = readdir(dir)) != NULL)

    {

    

    /// Buffer to store full path of current entry

        char path[TKENRSPONSESZE];

        

        //Construct the full path by combining parent directory path &entry name

        sprintf(path, "%s/%s", paathofDirrectory, cmngEntryinDirctory->d_name);

        

        // Check if modification time of file falls within specified rang

        if (dateVrification(path, gvnT1, gvnT2))

        {

            // If time criteria are met, write path to specified file descriptor

            write(fledesTempr, path, strlen(path));

            

            //

            write(fledesTempr, "\n", 1);

        }

        

        // Check if the entry is a subdirectory and not "." or ".."

        if (cmngEntryinDirctory->d_type == DT_DIR && strcmp(cmngEntryinDirctory->d_name, ".") != 0 && strcmp(cmngEntryinDirctory->d_name, "..") != 0)

        {

            //// Recursively traverse the subdirectory

            rcsivlyTrvrseDirrctryFrDATE(path, gvnT1, gvnT2, fledesTempr);

        }

    }

    

    //printf(ent->d_type);

    

    //Close the directory after traversal

    closedir(dir);

}





//in this below method, it will takes date argumnt, traverses directory for files modified within specified date range, archives them using 'tar' command, sends archive's size &content to client over our socket & then cleans up temporary file

void operationGetdirfFn(int skCmmctionCTFD, char **arrayyForCmmmdArgs)

{

    //again in this fun,  im doing same logic by firts intialising thsese below var

    //my home dirctorry path

    char *hmeUSRcrrnt = getenv("HOME");

    

    //now ill be giving nameof out tar careted fiel

    char *nmeOfFileCrted = "temp.tar.gz";

    

    //the gvn firt adate in string formaat

    char *srverDate1Strng = arrayyForCmmmdArgs[1];

    

    //2nd date alslo in string formmat

    char *srverDate2Strng = arrayyForCmmmdArgs[2];



//structures are used to hold date and time components

    struct tm srverDate1, srverDate2;

    

    //parsing date string srverDate1,format "%d-%d-%d" specifies that function should expect 3 integer values separated by hyphens. parsed values are assigned to corresponding fields of srverDate1, including tm_year (years since 1900), tm_mon (months since January), and tm_mday (day of the month)

    sscanf(srverDate1Strng, "%d-%d-%d", &srverDate1.tm_year, &srverDate1.tm_mon, &srverDate1.tm_mday);

    

    //same for second date given

    sscanf(srverDate2Strng, "%d-%d-%d", &srverDate2.tm_year, &srverDate2.tm_mon, &srverDate2.tm_mday);

    

    //here to set srverDate1 to start of specified date. tm_year value is adjusted by subtracting 1900 (since tm_year represents years since 1900), and tm_mon is adjusted by subtracting 1 (since tm_mon represents months starting from January as 0). tm_hour, tm_min, and tm_sec are set to 0 to represent start of day

    srverDate1.tm_year -= 1900;

    

    //printf("srverDate1.tm_year");

    srverDate1.tm_mon -= 1;

    

    //setting time components of srverDate1 (hours, minutes, and seconds) to 0. This sets time to beginning of day, i.e., midnight, for date specified in srverDate1

    srverDate1.tm_hour = 0;

    

    //here ill be doing same for minuties as well

    srverDate1.tm_min = 0;

    

    //again for scnds as also

    srverDate1.tm_sec = 0;

    

    //now ill be doing it for our date2 tken (same thing)

    srverDate2.tm_year -= 1900;

    

    //printf(srverDate2); //testing purpose

    srverDate2.tm_mon -= 1;

    

    //setting time components of srverDate2 to their maximum values, setting time to last second of day (23:59:59) for datein srverDate2

    srverDate2.tm_hour = 23;

    

 //settime to last secnd of day,by setting srverDate2.tm_min to 59 &srverDate2.tm_sec to 59,time is adjusted to 23:59:59, which represents end of day

    srverDate2.tm_min = 59;

    

    //doin same for seconds 

    srverDate2.tm_sec = 59;

    

    //now mktime is used to convert adjusted struct tm variables (srverDate1 and srverDate2) into time_t values, which represent no.of secods since (January 1, 1970)

    time_t date1 = mktime(&srverDate1);

    time_t date2 = mktime(&srverDate2);





//now here we are impementating same logic of our above functions, whre we are sendiing response to our ct by dclring thsese bufffer here which stores respnt type

    char buf[TKENRSPONSESZE];

    

    //a decsriptor filees for coomicating with pipe

    int dsFl[2];

    

    //this is again crrating a new prc to exceute below command

    pid_t nprcSrv;



    // now ill be createng a temporary file to store list of files

    char intrmediteFle[] = "tempXXXXXX";

    int fledesTempr = mkstemp(intrmediteFle);

    if (fledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }



    // Traverse directory tree and write filenames

    rcsivlyTrvrseDirrctryFrDATE(hmeUSRcrrnt, date1, date2, fledesTempr);



    // Fork a child process to handle tar &zip operation

    if (pipe(dsFl) < 0)

    {

        perror("Oops! There is some issue in creating pipe");

        return;

    }

    //so here ill be forking  a new again children process which have nprcSrv as my pid

    nprcSrv = fork();

    

    //if any problom in this forkin

    if (nprcSrv < 0)

    {

        perror("sorry!! There is some issue in forking children process");

        return;

    }

    

    //so hre the chid process will be executeing our commaand

    else if (nprcSrv == 0)

    {

        //we will just close our read side of pipeing as not needed here

        close(dsFl[0]);

        

        //so basically it is jyst redircting otp from stndard fileotp to write end of ourpipe

        dup2(dsFl[1], STDOUT_FILENO);

        

        //nw we are closing thiss also

        close(dsFl[1]);

        

        //so this below it replaces current process with tar command process,tar command is then executed with the specified arguments, creating a compressed archive file using the list of file names from temporary file (intrmediteFle)

        //"-czf": additional arguments to tar command. They are used to specify operation to perform:



//-c: Create a new archive

//-z: Compress the archive using gzip

//-f: Specifing name of archive file

        execlp("tar", "tar", "-czf", nmeOfFileCrted, "-T", intrmediteFle, NULL);

        

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

         //read end of pipe (fd[0]) is closed, and 

        close(dsFl[1]);

       // printf("%d",dsFl);

       

        //a response type of 2 is sent to client using write function. This response type indicates that a file will be sent

        int typeOfRespponseToBeSent = 2;

        

        write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));



//This ensures that child process (which created tar archive) has completed before goingon

        wait(NULL);

        

        //here im flushing standard output buffer to ensure anybufferred data is written before further operations

        fflush(stdout);

        long int filesize = szeFleInBytesGvn("temp.tar.gz");

        write(skCmmctionCTFD, &filesize, sizeof(filesize));



       //in belowcreated tar file (temp.tar.gz) is deleted using to clean up the temporary archive file

        unlink(intrmediteFle);



        // Send tar file to client

        int fileDescritorForTarFile = open(nmeOfFileCrted, O_RDONLY);

        if (fileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }

        

        //just like other fns we are doing same thing here, sending bytes to cleint

        ssize_t bytes_read, sentingBytesToCT;

        //this is specifying remaining bytes

        off_t afterRemainBytesInSrver = filesize;

        

        //checking the buffer sent being to client and decrementing it

        while (afterRemainBytesInSrver > 0 && (bytes_read = read(fileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

            //here the main decremnetation is happening and sending it simulatnously

            sentingBytesToCT = send(skCmmctionCTFD, buf, bytes_read, 0);

            

            if (sentingBytesToCT < 0)

            {

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInSrver -= sentingBytesToCT;

        }

        close(fileDescritorForTarFile);



        // Delete tar file

        unlink(nmeOfFileCrted);

    }

}





//__________________________TARFGETZ OPERATION______________________________



// checks whether a file's size, located at given path, falls within a specified range (srverSideFleSZ1 to srverSideFleSZ1). It uses stat function to retrieve file information, checks if file is a regular file, and then compares its size against specified range,function returns true if file meets size criteria &false otherwise

bool rngeszeFrFlie(char *path, size_t srverSideFleSZ1, size_t srverSideFleSZ2)

{

    struct stat st;

    // S_ISREG:checks whether file is a regular file (not a directory or other type of file)

    if (stat(path, &st) == 0 && S_ISREG(st.st_mode) && st.st_size >= srverSideFleSZ1 && st.st_size <= srverSideFleSZ2)

    {

    // If stat() call was successful, file is a regular file (not a directory) and its size is within specified range

        return true;

    }

    return false;

}



//recursively traverses through a directory and its subdirectories, checking size of files along the way. It uses readdir function to read directory entries, constructs full paths, checks file sizes, & uses recursion to traverse subdirectories

void rcsivlyTrvrseDirrctry(char *paathofDirrectory, size_t srverSideFleSZ1, size_t srverSideFleSZ2, int fledesTempr)

{



//here ill be doing that ill be opening directory from thw path which i got in paathofDirectory variable and ill be doing this with opendir function wich is in-bilut for this purpose

    DIR *dir = opendir(paathofDirrectory); 

    

    //so if it is returning mw NULLL that means some error is there and that im printin here 

    if (dir == NULL)            

    {

      //printf("dir->paathofDirectory);

      

        perror("Sorry there is some problem in opening directory");

        

        //lets just return from that becus we got an errror

        return;

    }

    struct dirent *trEntryinDrctory;

    

    // startin a loop that reads each direcctory entry within opened directory using readdir function,loop continues as long as there are more entries to read (readdir returns a non-null pointer)

    while ((trEntryinDrctory = readdir(dir)) != NULL) 

    {

    

    // buffer to store path of file or directory

        char path[TKENRSPONSESZE];  

        

        // construct path of file or directory by concatenating `paathofDirrectory` and `trEntryinDrctory->d_name`               

        sprintf(path, "%s/%s", paathofDirrectory, trEntryinDrctory->d_name); 

        

        // check if file size is between `srverSideFleSZ1` & `srverSideFleSZ1` (inclusive)

        if (rngeszeFrFlie(path, srverSideFleSZ1, srverSideFleSZ2))           

        {

        // write path of file to file descriptor `fledesTempr`

            write(fledesTempr, path, strlen(path)); 

            

            // write a newline character to file descriptor `fledesTempr`

            write(fledesTempr, "\n", 1);            

        }

        // check if directory entry is a directory and not `.` or `..`

        if (trEntryinDrctory->d_type == DT_DIR && strcmp(trEntryinDrctory->d_name, ".") != 0 && strcmp(trEntryinDrctory->d_name, "..") != 0) 

        {

            // recursively traverse subdirectory

            rcsivlyTrvrseDirrctry(path, srverSideFleSZ1, srverSideFleSZ2, fledesTempr); 

        }

    }

    // close directory

    closedir(dir); 

}



//so this below fnction is comapring size is in range or not. If it iis then it will use travel n to go in each directory and subdirctoreis to fund files

void oprationTarfgetzFun(int skCmmctionCTFD, char **szeArgmnets)

{

    // Get user's home directory 

    char *hmeUSRcrrnt = getenv("HOME");

    

  //here again for this function, i m giving  name of temporary tar file

    char *nmeOfFileCrted = "temp.tar.gz";



    // Convert size arguments to integers

    size_t srverSideFleSZ1 = atoi(szeArgmnets[1]);

    size_t srverSideFleSZ2 = atoi(szeArgmnets[2]);



    // Initialize a buffer to store file data

    char buf[TKENRSPONSESZE];



    // Initialize a pipe and a process ID

    int dsFl[2];

    pid_t nprcSrv;



    // Create a temporary file to store list of files

    char intrmediteFle[] = "tempXXXXXX";

    

    //

    int fledesTempr = mkstemp(intrmediteFle);

    

    //

    if (fledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }



    // Traverse directory tree and write filenames to temporary file

    rcsivlyTrvrseDirrctry(hmeUSRcrrnt, srverSideFleSZ1, srverSideFleSZ2, fledesTempr);



    // Fork a child process to handle tar and zip operation

    if (pipe(dsFl) < 0)

    {

        perror("Error creating pipe");

        return;

    }

    

    //here im forking a child procees for running my craeted command thst ill be executing using execlp bilow

    nprcSrv = fork();

    

    if (nprcSrv < 0)

    {

        perror("Error forking child process");

        return;

    }

    else if (nprcSrv == 0)

    {

        // Child process - execute tar command

        close(dsFl[0]);

        dup2(dsFl[1], STDOUT_FILENO);

        close(dsFl[1]);

        execlp("tar", "tar", "-czf", nmeOfFileCrted, "-T", intrmediteFle, NULL);

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

        // Parent process - send tar file to client

        close(dsFl[1]);



        // Send file response header

        int typeOfRespponseToBeSent = 2;

        write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));



        // Wait for child process to complete

        wait(NULL);

        fflush(stdout);



        // Get the size of tar file

        long int filesize = szeFleInBytesGvn("temp.tar.gz");

        write(skCmmctionCTFD, &filesize, sizeof(filesize));



        // Delete temporary file

        unlink(intrmediteFle);



        // Send tar file to client

        int fileDescritorForTarFile = open(nmeOfFileCrted, O_RDONLY);

        if (fileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }

        ssize_t bytes_read, sentingBytesToCT;

        off_t afterRemainBytesInSrver = filesize;

        while (afterRemainBytesInSrver > 0 && (bytes_read = read(fileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

            sentingBytesToCT = send(skCmmctionCTFD, buf, bytes_read, 0);

            if (sentingBytesToCT < 0)

            {

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInSrver -= sentingBytesToCT;

        }

        close(fileDescritorForTarFile);



        // Delete tar file

        unlink(nmeOfFileCrted);

    }

}







//_______________________FGETS OPERATION____________________________



void rcsivlyTrvrseDirrctryForFgets(char *paathofDirrectory, char **filenames, int num_files, int fledesTempr)

{

    // Open the directory with the given path

    DIR *dir = opendir(paathofDirrectory);

    if (dir == NULL)

    {

        perror("Error opening directory");

        return;

    }

    struct dirent *ent;

    while ((ent = readdir(dir)) != NULL)

    {

        // Create the full path of the current file

        char path[TKENRSPONSESZE];

        sprintf(path, "%s/%s", paathofDirrectory, ent->d_name);



        // Check if current file matches any of specified filenames

        for (int i = 0; i < num_files; i++)

        {

            if (strcmp(ent->d_name, filenames[i]) == 0)

            {

                // Write the path of the matching file to the temporary file descriptor

                write(fledesTempr, path, strlen(path));

                write(fledesTempr, "\n", 1);

                break;

            }

        }



        // Recursively traverse any subdirectories

        if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)

        {

            rcsivlyTrvrseDirrctryForFgets(path, filenames, num_files, fledesTempr);

        }

    }

    // Close directory

    closedir(dir);

}



//this is our main fgets function where we are first doing same logic, going in each sub directory under home, finding specifed files and then giving response to client with send commandd

void fgetsFunc(int skCmmctionCTFD, char **arrayyForCmmmdArgs, int argLen)

{

    //again same thing, sending home path to this below var

    char *hmeUSRcrrnt = getenv("HOME");

    

    //now getting this name of file created(tar)

    char *nmeOfFileCrted = "temp.tar.gz";



//specifying buffer for response to client

    char buf[TKENRSPONSESZE];

    

    //file descripotor for trversing this file

    int dsFl[2];

    

    //cretaing a neew child prc

    pid_t nprcSrv;



    // Create a temporary file to store the list of files

    char intrmediteFle[] = "tempXXXXXX";

    int fledesTempr = mkstemp(intrmediteFle);

    if (fledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }



    // Traverse specified directory and its subdirectories to find requested files

    rcsivlyTrvrseDirrctryForFgets(hmeUSRcrrnt, arrayyForCmmmdArgs + 1, argLen - 1, fledesTempr);



    // Check if any files were found

    if (lseek(fledesTempr, 0, SEEK_END) == 0)

    {

        // No files found, send "No file found" response

        int typeOfRespponseToBeSent = 1;

        write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));

        write(skCmmctionCTFD, "No file found", 13);

        close(fledesTempr);

        unlink(intrmediteFle);

        return;

    }



    // Fork a child process to handle the tar and zip operation

    if (pipe(dsFl) < 0)

    {

        perror("Error creating pipe");

        return;

    }

    nprcSrv = fork();

    if (nprcSrv < 0)

    {

        perror("Error forking child process");

        return;

    }

    else if (nprcSrv == 0)

    {

        // Child process

        close(dsFl[0]);

        

        //printf("dsfl[0]);

        // Redirect standard output to write end of pipe

        dup2(dsFl[1], STDOUT_FILENO);

        close(dsFl[1]);

        // Execute tar command to compress files into a single archive

        

        //so again letsjust execute our command

        execlp("tar", "tar", "-czf", nmeOfFileCrted, "-T", intrmediteFle, NULL);

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

        // in our mainParentprocess, closing write end

        close(dsFl[1]);

        

        // Send file response

        int typeOfRespponseToBeSent = 2;

        

        ///writing it to client again

        write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));



//wiating for tar file to exceute

        wait(NULL);

        fflush(stdout);

        // Get the size of the tar file

        long int filesize = szeFleInBytesGvn("temp.tar.gz");

        write(skCmmctionCTFD, &filesize, sizeof(filesize));



        // Delete temporary file

        unlink(intrmediteFle);



        // Send tar file to client

        int fileDescritorForTarFile = open(nmeOfFileCrted, O_RDONLY);

        if (fileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }

        ssize_t bytes_read, sentingBytesToCT;

        off_t afterRemainBytesInSrver = filesize;

        // Send the tar file to the client in chunks

        while (afterRemainBytesInSrver > 0 && (bytes_read = read(fileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

            sentingBytesToCT = send(skCmmctionCTFD, buf, bytes_read, 0);

            if (sentingBytesToCT < 0)

            {

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInSrver -= sentingBytesToCT;

        }

        close(fileDescritorForTarFile);



        // Delete tar file

        unlink(nmeOfFileCrted);

    }

}







//___________________TARGZF OPERATION________________________________________



//traverse a directory and its subdirectories, looking for files with specific extensions. When it finds files with matching extensions, it writes their paths to a tempory file descriptor, which can later be used to process those files

void rcsivlyTrvrseDirrctryForTargzf(char *paathofDirrectory, char **extensions, int extnsionCnt, int fledesTempr)

{

    // Open directory specified by paathofDirrectory

    DIR *srverDirctory = opendir(paathofDirrectory);

    

    //if there might be some issue in openning that particular dirctory

    if (srverDirctory == NULL)

    {

     //then we will be printing that error

        perror("There is some issue in opening directory");

        return;

    }

    // Declare a structure to hold directory entry information.

    struct dirent *srverSideEntryDirectoryChecking;

    

    //iterating through each directory entry using readdir 

    while ((srverSideEntryDirectoryChecking = readdir(srverDirctory)) != NULL)

    {

        //

        char constrctedPaath[TKENRSPONSESZE];

        

        //

        sprintf(constrctedPaath, "%s/%s", paathofDirrectory, srverSideEntryDirectoryChecking->d_name);



    //Find position of last occurrence of dot character (.) in filename. This is done using strrchr funct

        char *ext = strrchr(srverSideEntryDirectoryChecking->d_name, '.');

        

       //Check if ext is not null (i.e., if entry has an extension), if it's not at start of filename, and if extension is no longer than 5 characters

        if (ext != NULL && ext != srverSideEntryDirectoryChecking->d_name && strlen(ext) <= 5)

        {

          //Increment ext to move past dot character, so it points to actual extension

            ext++; 

            for (int pp = 0; pp < extnsionCnt; pp++)

            {

              //

                if (strcmp(ext, extensions[pp]) == 0)

                {

                    write(fledesTempr, constrctedPaath, strlen(constrctedPaath));

                    write(fledesTempr, "\n", 1);

                    break;

                }

            }

        }



        if (srverSideEntryDirectoryChecking->d_type == DT_DIR && strcmp(srverSideEntryDirectoryChecking->d_name, ".") != 0 && strcmp(srverSideEntryDirectoryChecking->d_name, "..") != 0)

        {

            rcsivlyTrvrseDirrctryForTargzf(constrctedPaath, extensions, extnsionCnt, fledesTempr);

        }

    }

    

    //After processing all entries, close directory using closedir

    closedir(srverDirctory);

}



//here ill be doing extension fun in which im impleemting same logic: going to directories and sub directories and finding files with those extensions

void oprationTargzfFn(int skCmmctionCTFD, char **arrayyForCmmmdArgs, int argLen)

{

    //again same getting user home directory path

    char *hmeUSRcrrnt = getenv("HOME");

    

    //creating my tar file name

    char *nmeOfFileCrted = "temp.tar.gz";



//to store my repossne being sent to ct

    char buf[TKENRSPONSESZE];

    

    //

    int dsFl[2];

    pid_t nprcSrv;



    // Create a temporary file to store list of files

    char intrmediteFle[] = "tempXXXXXX";

    int fledesTempr = mkstemp(intrmediteFle);

    if (fledesTempr < 0)

    {

        perror("Error creating temporary file");

        return;

    }

    rcsivlyTrvrseDirrctryForTargzf(hmeUSRcrrnt, arrayyForCmmmdArgs + 1, argLen - 1, fledesTempr);



    // Check if any files were found

    if (lseek(fledesTempr, 0, SEEK_END) == 0)

    {

        // No files found, send "No file found" response

        int typeOfRespponseToBeSent = 1;

        write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));

        write(skCmmctionCTFD, "No file found", 13);

        close(fledesTempr);

        unlink(intrmediteFle);

        return;

    }



    // Fork a child process to handle tar and zip operation

    if (pipe(dsFl) < 0)

    {

        perror("Error creating pipe");

        return;

    }

    nprcSrv = fork();

    if (nprcSrv < 0)

    {

        perror("Error forking child process");

        return;

    }

    else if (nprcSrv == 0)

    {

        // Child process

        close(dsFl[0]);

        dup2(dsFl[1], STDOUT_FILENO);

        close(dsFl[1]);

        execlp("tar", "tar", "-czf", nmeOfFileCrted, "-T", intrmediteFle, NULL);

        perror("Error executing tar command");

        exit(EXIT_FAILURE);

    }

    else

    {

        // Parent process

        close(dsFl[1]);

        // Send file response

        int typeOfRespponseToBeSent = 2;

        write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));



        wait(NULL);

        fflush(stdout);

        long int filesize = szeFleInBytesGvn("temp.tar.gz");

        write(skCmmctionCTFD, &filesize, sizeof(filesize));



        // Delete temporary file

        unlink(intrmediteFle);



        // Send tar file to client

        int fileDescritorForTarFile = open(nmeOfFileCrted, O_RDONLY);

        if (fileDescritorForTarFile < 0)

        {

            perror("Error opening tar file");

            return;

        }



        ssize_t bytes_read, sentingBytesToCT;

        off_t afterRemainBytesInSrver = filesize;

        while (afterRemainBytesInSrver > 0 && (bytes_read = read(fileDescritorForTarFile, buf, sizeof(buf))) > 0)

        {

            sentingBytesToCT = send(skCmmctionCTFD, buf, bytes_read, 0);

            if (sentingBytesToCT < 0)

            {

                perror("Error sending file data");

                break;

            }

            afterRemainBytesInSrver -= sentingBytesToCT;

        }

        close(fileDescritorForTarFile);



        // so now we are just here dealeting our tempory created tar file

        unlink(nmeOfFileCrted);

    }

}





//_______________STRING ARGUMENT MANIPULATION_____________________________



//process an array of strings (tokens) and remove anynewline characters ('\n') from each token



/*

char *wrds[] = { "ASP\n", "Final", "Project\n" };



word at index 0th: "ASP\n" would become "Hello" (newline removed)

word at index 1st: "Final" would remain unchanged

word at index 2nd: "Project\n" would become "Project" (newline removed)

*/

void stringManipulationForRmvingNewLine(char **cmdStringTkns, int totalCountOfTkns)

{

//iterating through each token in array from index 0 to totalCountOfTkns - 1.

    for (int svr = 0; svr < totalCountOfTkns; svr++)

    {

    //so in our serevr, current token at index i is assigned to the pointer token

        char *wrds = cmdStringTkns[svr];

        

   //calculates length of token until 1stoccurrence of a newlinne chracter ('\n'). strcspn funct returns lengh of initial segment of the string that doesnt contain specified characters

        int szeServer = strcspn(wrds, "\n");   

        

    //Memory is allocated dynamically to create a new string createdTknNw with a length of gnSze + 1

        // additional byte is for null terminator '\0'       

        char *tknNewSrver = (char *)malloc(szeServer + 1); 

        

       //copingcharacters from original token to new token, excluding newline character

        strncpy(tknNewSrver, wrds, szeServer);     

        

        //null terminator is added at end of new token to ensure it is a valid null-terminated string     

        tknNewSrver[szeServer] = '\0';         

        

        // new token is assigned to original token array, replacing original token with new one         

        cmdStringTkns[svr] = tknNewSrver;                        

    }

}







//_________________________PROCESSCLIENT__________________________________



//so this is our main prc fn to run excute each command givn by client, it is comparing arg[0] using strcmp() fn below

void processClient(int skCmmctionCTFD)

{

    char srverExectuablecmmand[255];

    char response[1024];

    int n, nprcSrv;

    write(skCmmctionCTFD, "Send commands", 14);



    while (true)

    {

        memset(srverExectuablecmmand, 0, sizeof(srverExectuablecmmand));

        n = read(skCmmctionCTFD, srverExectuablecmmand, 255);

        // Check if client has disconnected

        if (n <= 0)

        {

            printf("Client disconnected.\n");

            // Exit loop and terminate processclient() function

            break; 

        }

// here i have declared anArray to store command arguments

        char *arrayyForCmmmdArgs[PRMIT_ARGUMNTS]; 

        

        // Counter for no. of arguments

        int cntOfArgmnts = 0;          



        // Tokenize the command into arguments

        char *token = strtok(srverExectuablecmmand, " "); // Tokenize command using space as delimiter



        while (token != NULL)

        {

            arrayyForCmmmdArgs[cntOfArgmnts++] = token; // Store token in the array

            token = strtok(NULL, " ");          // Get the next token

        }

        arrayyForCmmmdArgs[cntOfArgmnts] = NULL; // Set the last element of the array to NULL



        // Remove line breaks from tokens

        stringManipulationForRmvingNewLine(arrayyForCmmmdArgs, cntOfArgmnts);



        char *cmd = arrayyForCmmmdArgs[0]; // Extract first token as the command



        printf("\nExecuting the following command: \n");

        for (int i = 0; i < cntOfArgmnts; i++)

        {

            printf("%s ", arrayyForCmmmdArgs[i]);

        }

        printf("\n\n");



        // here we are chkin for a particuar process

        if (strcmp(cmd, "filesrch") == 0)

        {

            opertionFrFilesrchFn(skCmmctionCTFD, arrayyForCmmmdArgs);

        }

        //this one gets all fliles in together

        else if (strcmp(cmd, "fgets") == 0)

        {

            fgetsFunc(skCmmctionCTFD, arrayyForCmmmdArgs, cntOfArgmnts);

        }

        //here we chking for extnsion one

        else if (strcmp(cmd, "targzf") == 0)

        {

            oprationTargzfFn(skCmmctionCTFD, arrayyForCmmmdArgs, cntOfArgmnts);

        }

          //now are chking for this size one of tarfgetz

        else if (strcmp(cmd, "tarfgetz") == 0)

        {

            oprationTarfgetzFun(skCmmctionCTFD, arrayyForCmmmdArgs);

        }

        //

        else if (strcmp(cmd, "getdirf") == 0)

        {

            operationGetdirfFn(skCmmctionCTFD, arrayyForCmmmdArgs);

        }

        //

        else if (strcmp(cmd, "quit") == 0)

        {

            printf("Request from Client to quit.\n");

            int typeOfRespponseToBeSent = SRVERRESPONSE_ISQUITORNOT;

            write(skCmmctionCTFD, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));

            // Exit loop & terminate processclient() function

            break; 

        }

        else

        {

        //printf(responnse); //debuuging

            snprintf(response, TKENRSPONSESZE, "Invalid command\n"); 

            //so basically we are gining this response to clint that provideed commaand is not valid, that is why we are usin this write method here which has this descriptore of sockt

            write(skCmmctionCTFD, response, strlen(response));

            // Continue to next iteration of loop to wait for new command

            continue; 

        }

    }

    exit(0);

}

//________________________OUR MAIN SERVER METHOD__________________________________



int main(int argc, char *argv[])

{



//so our main declaration and initialisation of various vraibles begin from now

    int dscrptorSkt, stClSkDstp, conndition;

    

    struct sockaddr_in servAdd; 



    //Create a socket using socket() function with domain (AF_INET for IPv4), type (SOCK_STREAM for TCP), &protocol (0 for default protocol)

    if ((dscrptorSkt = socket(AF_INET, SOCK_STREAM, 0)) < 0)

    {

        printf("Error in creating socket for communication\n");

        exit(1);

    }

//Set up server address structure (servAdd) with IP address (INADDR_ANY for all available network interfaces) and port number
    servAdd.sin_family = AF_INET;

    

 //assigns network byte order representation of IP address "0.0.0.0" to sin_addr field of servAdd structure. This tells socket to bind to all available network interfaces on host, making server reachable from any valid IP address associated with machine

 // htons is used to convert value to network byte order when sending data over ntwk

    servAdd.sin_addr.s_addr = htonl(INADDR_ANY); 



 //configures server's socket address structure to listen on a specific port number specified by PORT ie 1440

 // sin_port is a field in struct sockaddr_in that have port number used to establish network communication

    servAdd.sin_port = htons(MYSERVERTAKENPORT);



   // 

    bind(dscrptorSkt, (struct sockaddr *)&servAdd, sizeof(servAdd));



    //make server socket start listening for incoming client connections.5 specifies maxnumber of clients that can be waiting in connection queue to be served by server

    listen(dscrptorSkt, 5);



//when all connection isdone, now we will strt our server

    printf("Server has been started, waiting for client to connect.. !\n");



    // Wait for clients to connect

    while (true)

    {

        // Accept a client connection, and get a new socket descriptor (stClSkDstp) to communicate with client

        stClSkDstp = accept(dscrptorSkt, (struct sockaddr *)NULL, NULL);

        

        //as soon as a clint is connected, the count for live connected clints gets increased

        noOfCleintCnnctedLve++;

        

        //so just for showing we are also printing here the connceted clients to our server

        printf("Client Count: %d\n", noOfCleintCnnctedLve);



//so now basicaly i'll be checking based on no of client connected, whether server or mirror will handle commaamd

        int flgTknSrvrAccptOrNot = 0; //flag taken for server accept or not

        

        //here it will be taken care by our main srver (till 6)

        if (noOfCleintCnnctedLve <= 6)

        {

        //so we are chnging our flag value to 1

            flgTknSrvrAccptOrNot = 1;

        }

        

        //here it will bbe handled by our mirror server (frm 7 to 12 clients)

        else if (noOfCleintCnnctedLve >= 7 && noOfCleintCnnctedLve <= 12)

        {

        

        //so our main server flag will be zero as it is not handling it

            flgTknSrvrAccptOrNot = 0;

        }

        

        //now alterating part starts

        else

        {

       // so logic hre is that from 13 onwrds we will check if no is divisble by 2 then servr will take care else mirror

       //so 13 is not divisible by 2 so it will be tken care by mirror

       //14

            if ((noOfCleintCnnctedLve - 13) % 2 == 0)

            {

            

            //again  settting this to 1 for my srevr 

                flgTknSrvrAccptOrNot = 1;

            }

            else

            {

            // mirror will take this 

                flgTknSrvrAccptOrNot = 0;

            }

        }

        // If there is space for client to connect

        if (flgTknSrvrAccptOrNot)

        {

            printf("Connected to a Client\n");



            // here so ill be sending an aknowlegennt mesge to our ct to indicate that it can connect

            int typeOfRespponseToBeSent = 1;

            write(stClSkDstp, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));

            // Create a new process to handle client communication

            if (!fork()) 

            //so childprocess will be wunning our processClient() fun with our socket comm descrptor

                processClient(stClSkDstp);

            // Close socket descriptor

            close(stClSkDstp);

            // as i have crting a child process above, so we will just wait here to get its sttsus to giv us info whether it was success or not

            waitpid(0, &conndition, WNOHANG);

        }
        // If there is no space for client to connect

        else

        {

            // Send a message to client to indicate that it cannot connect

            int typeOfRespponseToBeSent = 0;

            write(stClSkDstp, &typeOfRespponseToBeSent, sizeof(typeOfRespponseToBeSent));



            // Close socket descriptor

            close(stClSkDstp);

        }

    }

}


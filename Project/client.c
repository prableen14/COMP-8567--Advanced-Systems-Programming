//Prableen Kaur Sachdeva

//Student ID: 110100670

//Section:4

//This is the client side implementation where user will be asked to give commaands to server or mirror (based on no. of clients connected)

//I've also printed given commands supportd by our server and mirror


//For running it, first check hostname -I on your machine to check IP address on which server and mirror is running

//In my machine, it was 10.0.2.15 for both, so for running this executable of client, we will write:

// ./client 10.0.2.15 10.0.2.15

//library for using inbuetl signls 
#include <sys/signal.h>

//as i'll be using macro for status of waiting  
#include <sys/wait.h>

//library is used for opening files and managing their attributes
#include <fcntl.h>

//various standard ip/op
#include <stdio.h>

//for using malloc func,atoi, exit
#include <stdlib.h>

//copying, concatenating, comparing, and searching for characters within strings
#include <string.h>

//read &write functins, fork system call
#include <unistd.h>

//inet_pton funtion that i used to convert an IP address from its human-readable string format to a binary representation that can be used by network functions
#include <arpa/inet.h>

//performing network-related operations, such as hostname resolution, service name to port number mapping, &netwrk address manipulation
#include <netdb.h>

//htons in-built metheod
#include <netinet/in.h>

//create a socket for communication
#include <sys/socket.h>

//used for defining data types such as pid_t
#include <sys/types.h>

//defined constants used throughout the program to specify server ports, response types, maximum number of argumnts
#define USRGVNCDONCONSLEOFXLEINT 256

//server port as 1440 
#define PREDEFINED_PRTOF_SRVR 1440

//mirror port as 1441
#define PREDEFINED_PRTOF_MRR 1441

//argument count that client will be permitted to accept as global
#define CLIENTARGUMENTSPERMITTED 10

//global varible to determine client textual rpond 1
#define CLIENTTEXTUALRESPONT 1

//and whether it has requsted for quit or not 
#define CLIENTRESPONSEQUIT 3

//store socket file descriptor used for communication
int cmmFlDsSckt = 0;

//function establishes a connection to a server or mirror using provided IP address and port. 
//It creates a socket, sets up a sockaddr_in structure with provided IP and port, and then tries to connect. It reads a flag from server/mirror to check if connection was accepted and returns that flag
int estblshCnntToSrvYaMrror(char *ip, int port)
{
//starting client and printing some info below
	// Print debug information about connection attempt
	printf("Trying to connect to %s with port no. %d\n", ip, port);
	// Create a socket for connection
	struct sockaddr_in serv_addr;
//if socket connections fails
	if ((cmmFlDsSckt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
//printing error of creation in skt
		printf("Socket creation error\n");
		return 0;
	}
	// Set up socket address structure where it has address (IP) and port for htons funtion
	serv_addr.sin_family = AF_INET;
	//converting  the port number to the correct byte order for network communicationThis ensures that portnumber is correctly interpreted by other devices or systems on the network that the server is communicating with.
	serv_addr.sin_port = htons(port);
	// Convert IP address string to a network address structure
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
	{
          //printf("Not a correct valid format");
		printf("Invalid address or Address not supported\n");
		return 0;
	}
  
	//connecting to server/mirror
	if (connect(cmmFlDsSckt, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
	 //if any problom during this,i.e,<0 
		return 0;

	}
	//reading a flag from our server/mirror indicating whether connection was taken or not
	int tkenOrNot;

	//reading this socket fdescriptor to check whether it accepted or not
	read(cmmFlDsSckt, &tkenOrNot, sizeof(tkenOrNot));

	//returning flag
	return tkenOrNot;
}

//attempts to extract files from a tar.gz archive. It constructs a system command using snprintf and then executes it using system. If extraction fails, it returns 1; otherwise, it returns 0
int flsExtractFromTAR()
{
	//so here this below is describing a varible that will have status of child process executed or not
	int extFlgTAR;

//declaring tar file name that will be created
    char *filename = "temp.tar.gz";

//forking a process to executed this tar unzippping cmmd
    pid_t cltnewPrc = fork();

//if this fork had issue
    if (cltnewPrc == -1)
    {
        perror("Fork failed");      
        return 1;

    }
    //in child process
    else if (cltnewPrc == 0)
    {
        //executing this tar commmand with xzf flag to unzip our tar fil
        char *args[] = {"tar", "-xzf", filename, NULL};
       
        //main execvp to run command
        execvp(args[0], args);

        // Execvp failed, print an error message
        perror("Execvp failed");
        exit(1);

    }
    else
    {
        int chdst;  
      
        //waiting for child process
        waitpid(cltnewPrc, &chdst, 0);
        if (WIFEXITED(chdst))
        {
            extFlgTAR = WEXITSTATUS(chdst);
            if (extFlgTAR != 0)
            {

                // Print an error message if the extraction failed
                fprintf(stderr, "Failed to extract files from %s\n", filename);
            }
        }
        else
        {
            // Child process did not exit normally
            fprintf(stderr, "Child process did not exit normally\n");
            extFlgTAR = 1;
        }
    }

    return extFlgTAR;
}

//below function is validating each command taken from console
int cnsleGnvARGMTSVALIDITYChkler(char arryOfArgs[][USRGVNCDONCONSLEOFXLEINT], int agrCntCsl)
{
	//taking command from argumnets, 0th index have the name of our option that we will executing in server/mirror side
	char *intpcomdOnClt = arryOfArgs[0];
  
// compring each input command using this strcmp() method below
	//checking for "quit" commad
	if (strcmp(intpcomdOnClt, "quit") == 0)
	{
	      //if user has provided not exactly 1 args, then it will throw below statement

		if (agrCntCsl != 1)

		{

		//so if its not eq1

			printf("Sorry but quit command doesn't take any arguments\n");	
			return -1;
		}
		return 0;
	}

	// Check for commands that require at least two arguments
	if (agrCntCsl < 2)
	{

		printf("Invalid arguments: must specify a command and a filename/extension list\n");

		return -1;

	}
	// Check for "filesrch" command
	if (strcmp(intpcomdOnClt, "filesrch") == 0)
	{
		if (agrCntCsl != 2)
		{
			printf("Invalid arguments: filesrch command must have a single filename argument\n");
			return -1;

		}

		// Check for the -u flag
		char *filename = arryOfArgs[agrCntCsl - 1];
		if (strcmp(filename, "-u") == 0)
		{
			printf("Invalid arguments: filesrch command must have a single filename argument\n");
			return -1;
		}
		return 0;

	}

	// checking for "tarfgetz" and "getdirf" commands
	else if (strcmp(intpcomdOnClt, "tarfgetz") == 0 || strcmp(intpcomdOnClt, "getdirf") == 0)
	{
	//checking conditon if it is in range of less than 3 or grter than 4, if so lets just prnt error
		if (agrCntCsl < 3 || agrCntCsl > 4)

		{
			printf("Sorry but tarfgetz/getdirf command should have size/date range arguments and -u option (if wanted) \n");

			return -1;

		}

		// Checking here forsize range arguments for "tarfgetz" command
		if (strcmp(intpcomdOnClt, "tarfgetz") == 0)

		{
			int range1 = atoi(arryOfArgs[1]);
			int range2 = atoi(arryOfArgs[2]);
      
	//cheking if its in range specified
			if (range1 < 0 || range2 < 0 || range1 > range2)
			{
				printf("Invalid arguments: tarfgetz command must have both size1 & size2 >= 0 and size1 <= size2\n");
				return -1;
			}
		}
      
		// Check for date range arguments for "getdirf" command
		else

		{
			// extracting dates from argumnts
			char *date1 = arryOfArgs[1];
			char *date2 = arryOfArgs[2];
      
			// converting dates to integers
			int year1, month1, day1;
			int year2, month2, day2;
			if (sscanf(date1, "%d-%d-%d", &year1, &month1, &day1) != 3)
			{
				fprintf(stderr, "Error: Invalid date format: %s\n", date1);
				return -1;

			}

//if date is having below format and if that is not eqto3
			if (sscanf(date2, "%d-%d-%d", &year2, &month2, &day2) != 3)
			{
				fprintf(stderr, "Error: Invalid date format: %s\n", date2);
				return -1;
			}

			//comparing dates that is taken from user in each 
			if (year1 < year2 || (year1 == year2 && (month1 < month2 || (month1 == month2 && day1 <= day2))))
			{
				// do nothing, valid dates
			}
			else
			{
				printf("%s is greater than %s\n", date1, date2);
				return -1;
			}
		}

		//unziiping flag
		char *flag = arryOfArgs[agrCntCsl - 1];
		//if last one is -u
		if (strcmp(flag, "-u") == 0)
		{
      
		//then we are chiking if it has -u or not
			if (agrCntCsl - 2 == 0)
			{
				printf("Invalid arguments: tarfgetz/getdirf command must have size/date range arguments and optionally -u option\n");
				return -1;
			}
			return 1;
		}
		//checking if it has -u option or not
		else if (agrCntCsl == 4 && strcmp(flag, "-u") != 0)
		{
			printf("Invalid arguments: tarfgetz/getdirf command must have -u option as last argument if specified\n");
			return -1;

		}

		return 0;

	}

	else if (strcmp(intpcomdOnClt, "fgets") == 0)
	{
		if (agrCntCsl < 2)

		{
			printf("Invalid arguments: fgets command must have at least one filename argument\n");
			return -1;
		}

//so we'll check if it is grter than 5
		if (agrCntCsl > 5)
		{
			printf("Invalid arguments: fgets command can have at most 4 filename arguments\n");
			return -1;

		}

//now here im chking if last argument is u or not

		char *flag = arryOfArgs[agrCntCsl - 1];

		if (strcmp(flag, "-u") == 0)
		{
				printf("Invalid arguments: fgets command couldn't have -u option\n");

				return -1;
		}
		return 0;
	}

//if we encontered this below command of extensins
	else if (strcmp(intpcomdOnClt, "targzf") == 0)
	{
		if (agrCntCsl < 2)
		{

			printf("Invalid arguments: targzf command must have at least one extension argument\n");
			return -1;
		}

		if (agrCntCsl > 6)
		{
			printf("Invalid arguments: targzf command can have at most 4 extension arguments and optionally the -u flag\n");
			return -1;
		}
		char *flag = arryOfArgs[agrCntCsl - 1];
		if (strcmp(flag, "-u") == 0)
		{
			if (agrCntCsl - 2 == 0)
			{
				printf("Invalid arguments: targzf command can have at most 4 extension arguments and optionally the -u flag\n");
				return -1;
			}
			return 1;
		}
		else if (agrCntCsl == 6 && strcmp(flag, "-u") != 0)
		{
			printf("Invalid arguments: targzf command must have -u flag as last argument if specified\n");
			return -1;

		}
		return 0;
	}
	else
	{
		printf("Invalid command: %s\n", intpcomdOnClt);
		return -1;
	}
}

//____________________________________OURMAIN__CLIENT____STARTING_______________________

int main(int argc, char *argv[])

{
      // IP address of server	  
	char intrntPIPFrSrver[16];

	  // IP address of mirror	  
	char intrtPIPForMrreor[16];

	// buffer for incoming data
	char buffer[1024] = {0};  

	// buffer for response text
	char response_text[1024];

	 // buffer for command entered by user
	char nCMMDUsrCT[1024];

	//checking if usar has entered server IP address and mirror IP address as command-line arguments
	if (argc < 3)
	{
		printf("Usage: %s <server_ip> <mirror_ip>\n", argv[0]);
		return 1;

	}

	//copying argumnt1 from console to this var specified in this field, it is IP here of server
	strcpy(intrntPIPFrSrver, argv[1]);

	//copying argumnt2 from console to this var specified in this field, it is IP here of mirrror
	strcpy(intrtPIPForMrreor, argv[2]);

	// Try to connect to server. If connection fails, try connecting to the mirror
	if (estblshCnntToSrvYaMrror(intrntPIPFrSrver, PREDEFINED_PRTOF_SRVR) > 0)

	{
		printf("Connected to the Server! \n");

	}

	else if (estblshCnntToSrvYaMrror(intrtPIPForMrreor, PREDEFINED_PRTOF_MRR) > 0)

	{
		printf("Connected to the Mirror! \n");

	}
	else
	{
		printf("Connection failed\n");
		return 1;
  }

	// Read welcome message from server/mirror
	read(cmmFlDsSckt, buffer, 1024);
	printf("Message from server: '%s' \n", buffer);
printf("   \nBelow are the supported commands:\n");
printf("    1)fgets file1 file2 file3 file4\n");
printf("    2)tarfgetz size1 size2 <-u>\n");
printf("    3)filesrch filename\n");
printf("    4)targzf <extension list> <-u>\n");
printf("    5)getdirf date1 date2 <-u>\n");
printf("    6)quit\n");

	// iterating through till usr discnnts or pressses CTRL-C 
	while (1)
	{
		printf("\nEnter a command:\n");
		// Clear buffer and get user input
		memset(buffer, 0, sizeof(buffer));
		//taking input from user
		fgets(buffer, 1024, stdin);

		// Copy user input to command buffer
		strcpy(nCMMDUsrCT, buffer);

		// Extract arguments from command
		char arryOfArgs[10][USRGVNCDONCONSLEOFXLEINT];
		int agrCntCsl = 0;
		int i = 0;
		int len = strlen(nCMMDUsrCT) - 1;

		while (i < len && nCMMDUsrCT[i] == ' ')

		{

			i++;

		}
		// extract arguments

		while (i < len && agrCntCsl < 10)

		{

			int j = 0;

			while (i < len && nCMMDUsrCT[i] != ' ')

			{

				arryOfArgs[agrCntCsl][j] = nCMMDUsrCT[i];

				i++;

				j++;

			}
			arryOfArgs[agrCntCsl][j] = '\0';

			agrCntCsl++;

			// skip any extra spaces between arguments

			while (i < len && nCMMDUsrCT[i] == ' ')
			{
				i++;
			}

		}
		// Check if command is for unzipping file
		int fgExtrctFles = 0;
		int checkArguments = cnsleGnvARGMTSVALIDITYChkler(arryOfArgs, agrCntCsl);

		if (checkArguments == -1)

		{
			continue;
		}
		else if (checkArguments == 1)
		{
			fgExtrctFles = 1;
		}
		else
		{
			fgExtrctFles = 0;
		}
		// here ihave declared this var toConcatenate arguments into single string
		char *result = malloc(CLIENTARGUMENTSPERMITTED * 100);
		//now we'll just get rst in this

		memset(result, 0, sizeof(result));
		//and adding ne null trmiated hre

		result[0] = '\0';
    
//looping through each file xtrcated
		for (int ttp= 0; ttp < agrCntCsl - fgExtrctFles; ttp++)
		{
			strcat(result, arryOfArgs[ttp]);

		//used to determine whether a space should be added after the current string being concatenated. If it's not the last string to be concatenated, a space is added
			if (ttp != agrCntCsl - 1 - fgExtrctFles)
			{
	// line appends a space to the end of the result string. This space is added between the concatenated strings to ensure there is separation between them
				strcat(result, " ");
			}
		}

		// send command to server
		send(cmmFlDsSckt, result, strlen(result), 0);
		int response_type;

		// read the response type from socket
		read(cmmFlDsSckt, &response_type, sizeof(response_type));

//here im again chking whattyppe of response it is, if client reqiested to quit, 
		if (response_type == CLIENTRESPONSEQUIT)
		{

			//so well just print that server has indicated that client should disconnect
			printf("\nDisconnected from server.\n");
			//and get off from there with exit
			exit(0);
		}
		else if (response_type == CLIENTTEXTUALRESPONT)

		{

			// server has sent a text response
			memset(response_text, 0, sizeof(response_text));  // clear the response text buffer
			read(cmmFlDsSckt, response_text, sizeof(response_text)); // read the response text from the socket
			printf("\nReceived text response:\n%s\n", response_text);
		}
		else
		{
	    // open a file to write the received tar file to
			// the server has sent a tar file
			FILE *fp = fopen("temp.tar.gz", "wb"); 
			//now im chking if its null so lets print rror

			if (fp == NULL)

			{
				printf("Error: Could not open file for writing.\n");
			}
			else
			{
				// read the size of the tar file
				long filesize;
				//reding the sockt to file size detrmne
				read(cmmFlDsSckt, &filesize, sizeof(filesize));
 //flush out any stndard otp
				fflush(stdout);
//to check what there in getting response from srver
				char buffer[1024];
				int bytes_read;
				// read the tar file data from the socket in chunks and write it to the file
				while ((filesize > 0) && ((bytes_read = read(cmmFlDsSckt, buffer, sizeof(buffer))) > 0))
				{
					if (fwrite(buffer, 1, bytes_read, fp) != bytes_read)
					{
						// error writing to the file
						printf("Error: could not write to file\n");
						fclose(fp);
						return 1;
					}
					filesize -= bytes_read;
				}
				printf("Tar File received successfully.\n");
				// close file before unzip
				fclose(fp);
				if (bytes_read < 0)
				{
					// error receiving the file data
					printf("Error: Failed to receive file.\n");
				}
				if (fgExtrctFles)
				{
					// if fgExtrctFles` flag is set, attempt to unzip the received tar file
					if (flsExtractFromTAR() != 0)
					{
						printf("Error: Failed to unzip a tar file.\n");
					}
					else
					{

						printf("Successfully unzipped received Tar file !\n");
					}
				}
			}
		}
	}
	close(cmmFlDsSckt);
	return 0;
}

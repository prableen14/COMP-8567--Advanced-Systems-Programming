//Prableen Kaur Sachdeva

//110100670

//sec:4

#include <dirent.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <sys/types.h>

#include <sys/wait.h>

#define MAX_PID_ALLOWED 5



//Function for printing the PID and PPID of all processes inputted frm console

void procsOnCslPID_PPID(int* process_ids, int num_processes) {

   //In this below line, we are initialising a char array to string that will be used to store command string for output

    char cmd[100];

    // Here we are declaring a character array to store the output lines from the command

    char line[900]; 

    for (int np = 1; np < num_processes; np++) { //looping through all processes mentioned by user

   // In this line, the ps command is used to get the PID and PPID of processes given on console

        sprintf(cmd, "ps -o pid,ppid -p %d", process_ids[np]); //sprintf is constructing command

        FILE* op = popen(cmd, "r");

        if (op == NULL) {

            printf("Failed to execute command for process %d.\n", process_ids[np]);

            return;

        }

        int line_count = 0; //variable to keep track of the line number.        

        // we are reading each line of the command output here

        while (fgets(line, sizeof(line), op) != NULL) {

            line_count++;



            // Skip the first line (header)

            if (line_count == 1) {

                continue; //As header line does not contain process information (PID and PPID), continue is executed

            }

            int pid, ppid;

            sscanf(line, "%d %d", &pid, &ppid); // Parsing line to get PID and PPID

            printf("%d %d\n", pid, ppid);

        }

        pclose(op);

        }

}



//Function for printing the immediate descendants of 1st process after root process given in cmd

int* processID1ImmediateDescendants(pid_t process_id, int* count) {

    char pathToProcessFile[90];

    char line[890];

    sprintf(pathToProcessFile, "/proc/%d/task/%d/status", process_id, process_id);

    FILE* file = fopen(pathToProcessFile, "r");

    if (file == NULL) {

        printf("Process %d not found in the process tree.\n", process_id);

        return NULL;

    }

    fclose(file);

    sprintf(pathToProcessFile, "/proc/%d/task/%d/children", process_id, process_id);

    file = fopen(pathToProcessFile, "r");

    if (file == NULL) {

        printf("Failed to open children file for process %d.\n", process_id);

        return NULL;

    }

    int* arrayToStoreChildren = malloc(sizeof(int));  // Allocate memory for initial PID

    *count = 0;  // Initialize count to 0



    while (fgets(line, sizeof(line), file) != NULL) {

    // strtok is used here to extract PIDs from line string.

    // It takes two arguments: string to be split (line in this case) and delimiter character or string (" " space character in this case)

        char* stringPID = strtok(line, " ");

        int pid;

        while (stringPID != NULL) {

            pid = atoi(stringPID);

         //realloc function is used to dynamically resize arrayToStoreChildren to accommodate more (PIDs) 

            arrayToStoreChildren = realloc(arrayToStoreChildren, (*count + 1) * sizeof(int));  // Resizing  array to accommodate new PID

            arrayToStoreChildren[*count] = pid;  // Store PID in array

            (*count)++;  // Increment count

            stringPID = strtok(NULL, " ");  // Moving to the next PID



            // Checking if the next PID is a duplicate PID

            if (stringPID != NULL && atoi(stringPID) == pid) {

                stringPID = strtok(NULL, " ");

            }

        }

    }



    fclose(file);

    return arrayToStoreChildren;  // Return the array with stored PIDs

}



//Printing the Sibling processes PID of the 1st process after the root process (processID1)

int* processID1SiblingProcesses(pid_t process_id, int* numSiblings) {

    char pathToProcStatusFile[100]; //used to store the path to the status file of each process

    sprintf(pathToProcStatusFile, "/proc/%d/status", process_id);



    FILE* file = fopen(pathToProcStatusFile, "r");

    if (!file) {

        printf("Process %d couldn't be found in given tree of processes\n", process_id);

        return NULL;

    }

    fclose(file);



    pid_t ppid = -1; // Parent Process ID

    

    //format the pathToProcStatusFile with the given process ID. It creates the path to the status file of the process. Eg: /proc/2034/status will give all details about process 2034

    sprintf(pathToProcStatusFile, "/proc/%d/status", process_id);

    file = fopen(pathToProcStatusFile, "r"); //read the contents of the file in r mode but doesn't allow modifications or writing to file

    if (file != NULL) {

        char line[1000];

        while (fgets(line, sizeof(line), file) != NULL) {

          if (strncmp(line, "PPid:", 5) == 0) { //check if current line starts with string "PPid:",then compare 1st 5 characters of line with given string

                ppid = atoi(line + 6); //atoi converts string to int here for ppid

                break;

            }

        }

        fclose(file);

    } else {

        printf("Failed to open status file for process %d.\n", process_id);

        return NULL;

    }



    DIR* proc_dir = opendir("/proc"); //Open the "/proc" directory, which contains information about processes

    if (proc_dir == NULL) {

        printf("Failed to open /proc directory.\n");

        return NULL;

    }

    int* siblings = NULL;

    int count = 0;

    struct dirent* initialFileOrDirectory;

    while ((initialFileOrDirectory = readdir(proc_dir)) != NULL) {

    //d_type is an object of  dirent structure

        if (initialFileOrDirectory->d_type == DT_DIR) { //checking if current entry is a directory by  d_type,it ensures that only directory entries are processed

            int pid = atoi(initialFileOrDirectory->d_name); // Converting dirctory name to process ID

            if (pid != 0 && pid != process_id) { //processID1 should be excluded from siblings becoz a process cant be sibling of itself and it should not be 0

                sprintf(pathToProcStatusFile, "/proc/%d/status", pid);

                FILE* status_file = fopen(pathToProcStatusFile, "r");

                if (status_file != NULL) {

                    char line[1000]; //Create a buffer to read each line of the status file

                    while (fgets(line, sizeof(line), status_file) != NULL) {

                        if (strncmp(line, "PPid:", 5) == 0) {

                            int parent_pid = atoi(line + 6);

                            if (parent_pid == ppid) { //if parent process ID is same as stored parent process ID (ppid), it means current process is a sibling of provided process, if they are having same parent

                                count++;

                                siblings = (int*)realloc(siblings, count * sizeof(int)); //dynamically incraesaing the siblings array for storing all the sibings found in every iteration

                                siblings[count - 1] = pid;

                            }

                            break;

                        }

                    }

                    fclose(status_file);

                }

            }

        }

    }

    closedir(proc_dir); //closing the proc file when work is done

    *numSiblings = count;

    return siblings;

}



//Function for printing the grandchildren process PID of processID1

void processID1GrandchildrenProcesses(int* child_pids, int child_count) {

    for (int i = 0; i < child_count; i++) {

        int* grandchild_pids;

        int grandchild_count;

//here we are first finding the immediate child of process and then we are traversing each child to print its respective

        grandchild_pids = processID1ImmediateDescendants(child_pids[i], &grandchild_count);

        if (grandchild_pids == NULL) {

            continue;

        }

        for (int j = 0; j < grandchild_count; j++) {

            printf("%d\n", grandchild_pids[j]);

        }



        free(grandchild_pids);

    }

}



//Function for checking the status of processID1 (Defunct or not) (ZZ option)

void processID1Status_ZZ(pid_t process_id) {

   char command[100];

    sprintf(command, "ps -o stat= -p %d", process_id); //using ps command to find the ststus of process, ps -o stat= 3456 where 3456 is some random process to cehck if its defunct or not

    FILE* file = popen(command, "r");

    if (!file) {

        perror("popen");

        return;

    }

    char chk[3]; //it is char array for storing whether a process is having ststsus as Z. if its S, then that means it is not zombie

    if (fgets(chk, sizeof(chk), file) != NULL) {

        if (strlen(chk) > 0) {

            if (chk[0] == 'Z') {

                printf("DEFUNCT\n");

            } else {

                printf("NOT DEFUNCT\n");

            }

        } else {

            printf("Failed to retrieve process status for %d.\n", process_id);

        }

    }



    pclose(file);

}



//Function for printing the non-direct descendants of processID1 (-nd option)

void processID1NonDirectDescendants(pid_t processID1, int* arrayForDirectCildren, int count, pid_t originalProcessId) {

    pid_t childPid;

    FILE *fp;

    char cmd[50];

    sprintf(cmd, "pgrep -P %d", processID1); //this is creating a command where it will give all the processes under processID1

    if ((fp = popen(cmd, "r")) == NULL) {

        perror("popen");

        exit(1);

    }

    // Checking if processID1 is present in arrayForDirectCildren

    int pc;

    int found = 0; 

    for (pc = 0; pc < count; pc++) {

        if (arrayForDirectCildren[pc] == processID1) { //if its found in direct children, that means it is not non-descendants, so break from this

            found = 1;

            break;

        }

    }

    // If pid is not in array of immediate children and not equal to the original processId, print it, that means it is all non-direct children (descendants) 

    if (!found && processID1 != originalProcessId) {

        printf("%d\n", processID1);

    }

    while (fgets(cmd, sizeof(cmd), fp) != NULL) {

        childPid = atoi(cmd); //this will be storing the PIDs found from output of pgrep, atoi will convrt it into pid_t type

        processID1NonDirectDescendants(childPid, arrayForDirectCildren, count, originalProcessId); //a recursive call where each process will be used for finding the processes under it

    }

    pclose(fp);

}

//In this method, all those processes having Z in stste o proc file will be printed

void printZombieProcess(pid_t process_id) {

    char path[100];

    sprintf(path, "/proc/%d/status", process_id);

    FILE* file = fopen(path, "r");

    char line[980];

    while (fgets(line, sizeof(line), file) != NULL) {

        if (strncmp(line, "State:", 6) == 0) {

            char state[10];

            sscanf(line, "State: %s", state);



            if (strncmp(state, "Z", 1) == 0) {

                printf("%d\n", process_id);

            }

            break;

        }

    }

    fclose(file);

}



int main(int cnsleArgCnt, char* cnsleArgVle[]) {

    if (cnsleArgCnt < 3) { //if no. of arguments is less than 3, if its not atleast of format: ./prcinfo rootProcess processID1

        printf("Arguments doesnt meet the required criteria");

        return 1;

    }

    pid_t rootProcess = atoi(cnsleArgVle[1]); //root of process stored from argumnt1

    pid_t process_id1 = atoi(cnsleArgVle[2]); //processID1 stored from argumnt2

    

    int prCnt; //used for storing process count mentioned on console by user

 char* selection; //it is for the option provided by user

 

 if (atoi(cnsleArgVle[cnsleArgCnt - 1]) <= 0) { //if the argument value of last on console is less than 0, means it is an option and not a process

    selection = cnsleArgVle[cnsleArgCnt - 1]; //it is storing the value of option provided 

} else {

    selection = NULL;

}

if (selection == NULL) {

    prCnt = cnsleArgCnt - 1; //if no option is provided, then total sum of processes is found by subtracting name of program from total number of argument count on console. For eg; ./prcinfo 2349 2356 2366, here total number of processes will be 3 (4-1)

} else {

    prCnt = cnsleArgCnt - 2; //if option is given, that means that will also get subtracted from total number of argumnets provided for calculating total processses

}

    int consoleProcessArr[prCnt]; //array for storing all the processes from console

    for (int i = 0; i < prCnt; i++) {

        consoleProcessArr[i] = atoi(cnsleArgVle[i + 1]);

    }

    if (prCnt > MAX_PID_ALLOWED) { //total processes can only be upto max 5

        printf("Invalid number of process IDs.\n");

        return 1;

    }

    if(selection==NULL)

    { //if no option provided

    char pt[100];

    sprintf(pt, "/proc/%d/task/%d/status", rootProcess, rootProcess); //checking the ststus of root process using proc/rootprocess/task/rootprocess/status, so this will tell whether root process exists or not

    FILE* file = fopen(pt, "r"); 

    if (file == NULL) { //if its not

        printf("Process %d couldnot found in process tree.\n", rootProcess);

        return 1;

    }

    fclose(file);

    sprintf(pt, "/proc/%d/task/%d/status", process_id1, process_id1);//similarly we are checking ststus of process_ID1, if its exists in the tree rootd at root procss

    file = fopen(pt, "r");

    if (file == NULL) {

        printf("Process %d not found in the process tree.\n", process_id1);

        return 1;

    }

    fclose(file);

    

    procsOnCslPID_PPID(consoleProcessArr, prCnt); //function for printing PID and PPId of processes}

   

    }

    // Checking if any option is provided

    else if (selection != NULL) {

    procsOnCslPID_PPID(consoleProcessArr, prCnt); //print PID and PPID that time aslso when option is there, because we need to provide for every process ID on console

    

        if (strcmp(selection, "-nd") == 0) { //Non-direct descndnts

            int count; //for no. of non-driect descedents

            

            int* arrayForDirectChildren = processID1ImmediateDescendants(process_id1, &count); //first direct descendts are found and then that is stored in an array to pass it to non-direct descendants function (so as to exclude dd ones)

            

            processID1NonDirectDescendants(process_id1, arrayForDirectChildren, count, process_id1);

        } 

        else if (strcmp(selection, "-dd") == 0) {

            int count;

            int* ddArrDynamic = processID1ImmediateDescendants(process_id1, &count);

            if (ddArrDynamic != NULL) {

                for (int i = 0; i < count; i++) {

                    printf("%d\n", ddArrDynamic[i]); //direct children are printed (dd option from process1)

                }

                free(ddArrDynamic);

            }

        } 

        else if (strcmp(selection, "-sb") == 0) { //if option for siblings are taken

            int numSiblings = 0;

    int* siblings = processID1SiblingProcesses(process_id1, &numSiblings); //finding siblings fucntion invokation

    if (siblings != NULL) {

        for (int i = 0; i < numSiblings; i++) {

            printf("%d\n", siblings[i]);

        }

        free(siblings);

    }

        } 

        else if (strcmp(selection, "-sz") == 0) { //siblings that are defunct for procssID1

          int numSiblings = 0;

    int* siblings = processID1SiblingProcesses(process_id1, &numSiblings); //first normal siblings are stored in array

    for (int i = 0; i < numSiblings; i++) {

     printZombieProcess(siblings[i]);} //and then this funct. is chhecking which among thse siblings are zombies

        } 

        else if (strcmp(selection, "-gc") == 0) { //case of grandchildrne

            int* child_pids;

    int child_count;

    child_pids = processID1ImmediateDescendants(process_id1, &child_count); //children are found first

    if (child_pids != NULL) {

        processID1GrandchildrenProcesses(child_pids, child_count); //then that will be processed further for its children(grandchilden in this case)

        free(child_pids);

    }

        } 

        else if (strcmp(selection, "-zc") == 0) { //direct desdents in defucnt state

           int count;

            int* zcArrChildForComparsion = processID1ImmediateDescendants(process_id1, &count); //direct desdents are found first and then those who are zombie procsses will be printed from blow fucntion call

            for (int i = 0; i < count; i++) {

        printZombieProcess(zcArrChildForComparsion[i]);

    }

        } 

        else if (strcmp(selection, "-zz") == 0) { //status whether processID1 is a defucnt or not

            processID1Status_ZZ(process_id1);

        }

        else printf("Not a valid option");

    } 

    return 0;

}

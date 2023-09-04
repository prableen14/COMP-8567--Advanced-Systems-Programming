//Prableen Kaur Sachdeva
//Section: 4
//Student ID: 110100670

#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

//Global variable declaration for source path, destination path, number of extensions, move flag and initial directory processed flag
char pathOfSource[400];
char pathOfDestination[400];

int num_ext = 0;
int moveFlag = 0;
int entryDirProcessed = 0; 
char destinationFinalPath[400];
const char **ext;

//Function to check the extensins provided in the command line arguments
int extensionChecker(const char *path)
{
    char *ptrPath = strrchr(path, '.'); //finding the last occurrence of the dot ('.') in the string path
    if (ptrPath == NULL)
        return 0;
    int i = 0;
    while (i < num_ext)
    {
        if (strcmp(ptrPath + 1, ext[i]) == 0) //strcmp function is used here to compare each extension in ext array with extension extracted from file path (ptrPath + 1)
        {
            return 1;
        }
        i++;
    }
    return 0;
}

//Function which is creating destination directory by taking destination directory path as argument from main function
int createDirectoryIfNeeded(const char *path)
{
    struct stat statusD_F; //struct type object which is used to find the file or directory type
    if (!(stat(path, &statusD_F) == 0 && S_ISDIR(statusD_F.st_mode))) //checking if path matches to existing directory by using stat() to find file information and then checking the file type using S_ISDIR()
    {
        char* parentDirectory = strdup(path); //creating duplicate path of parent directory (destination here)
        char* last_slash = strrchr(parentDirectory , '/'); //search for last occurrence of character '/' in parentDirectory string to extract filename or directory name from path
        if (last_slash != NULL)
        {
            *last_slash = '\0';  // Remove last slash to get parent directory path
//recursively calling create directory function so as to create destination directory as specified in parentDirectory path (which is holding the destination path received from main function
            int status = createDirectoryIfNeeded(parentDirectory); 
            free(parentDirectory );
            if (status == 0)
                return 0;  // Error occurred while creating the parent directory
        }
        int status = mkdir(path, 0775);
        if (status == 0)
        {
            return 1;  // Directory created successfully
        }
        else
        {
            return 0;  // Error occurred while creating directory
        }
    }
    return 1;  // Directory already exists
}

/*Callback Function from nftw() function argument which is creating hard links for regular files (based on extensions) and creates destination directories when finding directory types during a directory traversal
*/
int copyFileOrDirectory(const char *path, const struct stat *sb, int fileTypeF_or_D, struct FTW *buf)
{
    char dst_path[400];
    sprintf(dst_path, "%s%s", pathOfDestination, path + strlen(pathOfSource)); //creating destination path by appending relative path from source to destination.
    if (fileTypeF_or_D == FTW_F)
    {
//If num_ext is zero, indicating that no specific extensions are specified to be copied or if extensionChecker func returns false for path, means that file doesn't have valid extension
        if (num_ext == 0 || !extensionChecker(path))
        {
         int linkCreation = link(path, dst_path); //creating a hard link between source file and destination file using link()
        if (linkCreation == -1) //checking if its successful or not
            perror("Error in creating link");
        }
    }
    else if (fileTypeF_or_D == FTW_D) //If its a directory
    {
        if (entryDirProcessed == 0) 
        {
//if entryDirProcessed is 0,means this is first time directory is encountered, and entryDirProcessed is incremented by 1, so it means it doesnt create source folder again in destination folder
            entryDirProcessed++; 
        }
        else
        {
          int mkdirCreation = mkdir(dst_path, 0776); //mkdir function is used to create the directory specified by dst_path here
        if (mkdirCreation == -1)
          perror("Error in executing mkdir");
        
        }
    }    
    return 0;
}

//Function to delete a file or directory based on the type (file, symbolic link, or directory)
int deleteFileOrDirectory(const char *path, const struct stat *sb, int flag, struct FTW *buf)
{
    int deleteOperationReturnValue;
    if (S_ISREG(sb->st_mode) || S_ISLNK(sb->st_mode))  //checking if the file or symbolic link is a regular file or a symbolic link
    {
        deleteOperationReturnValue= remove(path); //remove function is called to delete the file or symbolic link specified by path
        if (deleteOperationReturnValue== -1)
            perror("Error: Directory or Files can't be removed");
    }
    else if (S_ISDIR(sb->st_mode)) //checking if it is a directory
    {
        deleteOperationReturnValue = rmdir(path); //rmdir function is called to remove the directory specified by path
        if (deleteOperationReturnValue == -1)
            perror("Error: rmdir function failed");
    }
    return 0;
}

//Function using nftw() system call to recursively copying all the files and folder from source to destination directory
int copyDirectory(const char *src, const char *dst)
{
    return nftw(src, copyFileOrDirectory, 15, FTW_PHYS);
}

//Function using nftw() system call to recursively moving all the files and folder from source to destination directory
int moveDirectory(const char *src, const char *dst)
{
    int moveOperationResult;
    moveOperationResult = copyDirectory(src, dst); //first copying the contents from source to destination directory using copyDirectory() function
//then calling nftw() function with deleteFileOrDirectory() function as argument to delete source directory 
    moveOperationResult = nftw(src, deleteFileOrDirectory, 15, FTW_DEPTH | FTW_PHYS); 
    return moveOperationResult ;
}
int main(int argCount, char *argValue[])
{
    if (argCount < 4) //checking if the argument count is less than 4
    {
        printf("Invalid Argument List");
        return 1;
    }
     int max_extensions = 6;
    ext = malloc(max_extensions * sizeof(char *)); //dynamically allocating memory for an array of strings (ext) based on the max_extensions value
    if (argCount > 4) { 
//If there are more than four arguments,loop iterates over additional arguments starting from fifth argument, stores them as strings in the ext array, and increments num_ext accordingly
        for (int i = 4; i < argCount && i - 4 < max_extensions + 1; i++) {
            ext[i - 4] = argValue[i];
            num_ext++;
        }
if (num_ext > 6) {
        printf("Error: Maximum number of extensions allowed is 6.\n");
exit(0);
    }
    }
     strncpy(pathOfSource, argValue[1], sizeof(pathOfSource) - 1); //copying the source path provided as a command-line argument into the pathOfSource buffer
    pathOfSource[sizeof(pathOfSource) - 1] = '\0'; //assigning '\0' to the last character of pathOfSource to ensure that buffer is properly null-terminated
  
   struct stat sourceDirectoryExistsCheck;
    if (stat(pathOfSource, &sourceDirectoryExistsCheck) != 0 || !S_ISDIR(sourceDirectoryExistsCheck.st_mode)) //checking if the source directory exists or not
    {
        printf("Source directory does not exist");
        return 1;
    }
    
    strncpy(pathOfDestination, argValue[2], sizeof(pathOfDestination) - 1); //copying the destination path provided as a command-line argument into the pathOfDestination buffer
    pathOfDestination[sizeof(pathOfDestination) - 1] = '\0'; //assigning '\0' to the last character of pathOfDestination to ensure that buffer is properly null-terminated
char *sourceDirectoryName=NULL;    
char *lastSlash = strrchr(pathOfSource, '/');
    if (lastSlash != NULL) {
        sourceDirectoryName = lastSlash + 1;
    } else {
        sourceDirectoryName = pathOfSource;
    }

	strcat(pathOfDestination, "/");
	strcat(pathOfDestination, sourceDirectoryName);
    strcpy(destinationFinalPath,pathOfDestination);
	
 umask(0000);  // Set file mode creation mask to allow all permissions    
 if(createDirectoryIfNeeded(pathOfDestination) == 0){
	 perror("Error");
 }//function call to create destination directory if it doesnt exist
 if (strcmp(argValue[3], "-cp") == 0) //if copy operation is mentioned in command line argument
    {
        moveFlag = 0;
    }
    else if (strcmp(argValue[3], "-mv") == 0) //if move operation is mentioned in command line argument
    {
        moveFlag = 1;
    }
    else
    {
        printf("Invalid operation");
        return 1;
    }

   return moveFlag ? moveDirectory(pathOfSource, pathOfDestination) : copyDirectory(pathOfSource, pathOfDestination);
}

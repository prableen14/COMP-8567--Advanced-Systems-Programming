#!/bin/bash

#Asssignment-4
#Prableen Kaur Sachdeva
#Student ID:110100670
#Section: 4

#___VAR ASSIGNING_____
#giving values to all below varibles in this section

#sets number of seconds script should sleep between backup operations. In this case, it's set to 2 mints, 120 scds

ctSlpGivnTm=120

#asssigning value of envrnment varble $HOME (which gives user's home directory) to variable myusrHmDr
myusrHmDr="$HOME"

#storing where the log filee will be created and stored
fleForBckpLg="$HOME/home/backup/backup.log"

#setting path to directories where incremental backups will be stored
genrtDirecForBckupForIcb="$HOME/home/backup/ib"

#setting path to dirctoriies where complete backups will be stored
genrtDirecForBckupForCmp="$HOME/home/backup/cb"

#%a=short day name (e.g., "Tues")
#%d=Date as a decimal number(01, 02,)
#%b=monthh name (e.g., "oct")
#%Y=Year with century as deciml num (..e.g., 2022)
#%I=Hour(01-12) as decimal number
#%M=Minute as a decimaal numer
#%S=Second as a decimal number
#%p=AM or PM fr time (e.g., "AM" or "PM")
#%Z=Timzone nme (here.e.g., "EDT")
#So, below ths format string will result in a timestamp like: "Wed 02 Aug 2023 12:23:45 PM EDT"
gvTsFrmtLogg="%a %d %b %Y %I:%M:%S %p %Z"

#sets format for timestamps (name backup formt) that will be used in names of backup files 
nmebkpfmt="%Y-%m-%d %H:%M:%S %Z"

#settting numric prefxes fr names of complete and incremental backup files
# first complete backup file will be named something like cb20001.tar, & next complete backup file will have prefix 20002, then 20003, and so on,as backps are created
prevnmeIncmpbp=20001



#here itrepesents prefix for incremental backup file names, value is set to 10001

#Similrly, this says that 1stincremental backup file will be named like ib10001.tar, & aftr this incrmntal bckups wil follow patern 10002, 1003, &so on

fmrtnmeForIcbpv=10001



#__NOW CREATING DIRCTORIES___(AS GIVN IN QUESTIN)

#These lines create necessary backup directories using -p flag, which ensures that parent directories are created if they don't already exist
mkdir -p "$genrtDirecForBckupForCmp"

#creating directory for backup of incremental
mkdir -p "$genrtDirecForBckupForIcb"

#creating a home dirctory in my current pwd
mkdir -p "$myusrHmDr"

# creates backup log file if it doesn't exist
touch "$fleForBckpLg"

#___FUNCTIONS NOW STARTING__

#function searches for .txt files in specified directory, creates a compressed tar archive with a unique name based on prevnmeIncmpbp, logs creation of backup, and increments prefix for next backup

fncForCmpltBkp() {
local backupFileName="cb$prevnmeIncmpbp.tar"
#command searches for all files with .txt extnsion in homeDirtory myusrHmDr
#-print0 flag is used to separate file names with null characters, which helps hndle files with special characters or spaces in theiir names
#pipeing here is connecting output of find command to input of next comand

#This cmmand creates a compressed tar archive named cb$prevnmeIncmpbp.tar in genrtDirecForBckupForCmp dirrctory
#options used are:
#c=Creatng new archieve
#z=Compress archive using gzip
#v=listing files being archived
#f=Specify name of archive file

#--null -T -: These options are used to process null-separated file names frm find cmmand, --null tells tar to expect null-separated input, and -T - instructs tar to read file names from standrd iput

  find "$myusrHmDr" -name "*.txt" -type f -print0 | tar czvf "$genrtDirecForBckupForCmp/$backupFileName" --null -T -
  
  #adds a log entry indcating that complete backup was created. log entry includes timestamp (passed as 1st argument to function) and name of backup file, >> appends this log entry to fleForBckpLg (backup log file)
  echo "$1 $backupFileName was created" >>"$fleForBckpLg"

# incremets prevnmeIncmpbp variable by 1, next complete backup will have an incremented prefix, creating uniqeue name for each backup file
  prevnmeIncmpbp=$((prevnmeIncmpbp + 1))

}

# given belw fucnt methodd searches for recntly modfied .txt fils in specfied directry
#If thosee files are found, it cretes a compressed tar archive containing those files, logs creation of backup, and increments fmrtnmeForIcbpv
#If no changes are detected, it logs that no incremental backup was created
fncForIncrementalBckps() {

#finding files with .txt extension within specified home Directry
# -type f flag for only reguular files are considered,  -name "*.txt" flag gives for files with .txt extnsion,  -newermt "$2" flag finds files modified after timestamp passed as 2nd argment to funtion
#fslsTxFl will cntain a list of file paths of recently modified .txt files

  fslsTxFl=$(find "$myusrHmDr" -type f -name "*.txt" -newermt "$2")

  #-n flag checks if variable $fslsTxFl is not empty, meaning there are files that have been modified
  if [ -n "$fslsTxFl" ]; then

  #sets fleBkpFinal variable, which will save name of incremental backup file, name is constructed using ib prefix followed by value of fmrtnmeForIcbpv, resulting in something like ib10001.tar.gz.
    fleBkpFinal="ib$fmrtnmeForIcbpv.tar"

    

    #creates a compressed tar archive using tar command:

#-czf: Specifies that archive should be created with compression using gzip

#"$genrtDirecForBckupForIcb/$fleBkpFinal": path to backup file that will be created

#$fslsTxFl: list of files to include in backup

    tar -czf "$genrtDirecForBckupForIcb/$fleBkpFinal" $fslsTxFl

    

 #adds a log entry indicating that incremental backup was created. log entry includes timestamp (passed as 1st agument to function) and name of backup file. >> operator appends this log entry to fleForBckpLg

    echo "$1  $fleBkpFinal was created" >>"$fleForBckpLg"



#line increments fmrtnmeForIcbpv variable by 1, ensuring that next incremental backup will have a different prefix for its name
    fmrtnmeForIcbpv=$((fmrtnmeForIcbpv + 1))
  else
  #else block of conditional statement, indicating that there were no recently modified .txt files.
  #adds a log entry indicating that no changes, and thus, no incremental backup was created,log entry includes timestamp (passed as 1st argument to function).
    echo "$1  No changes-Incremental backup was not created" >>"$fleForBckpLg"
  fi
}

#____ITERATING STRTING_____

#starting an infinite loop
while :; do
# sets variable flFmrtCrrLg to current timestamp formatted according to gvTsFrmtLogg,date command with +"$gvTsFrmtLogg" arjment is used to generate timestamp with specifeid formaat
  flFmrtCrrLg=$(TZ=EDT date +"$gvTsFrmtLogg")
  #updates tmeCrr variable with current timestamp formatted according to nmebkpfmt
  tmeCrr=$(date +"$nmebkpfmt")
  fncForCmpltBkp "$flFmrtCrrLg"
  # introduces a sleep delay of ctSlpGivnTm seconds. This delay controls how often loop iterates. In this case, script will pause for specified number of seconds before proceeding with next iteration of loop
  sleep $ctSlpGivnTm
#looping 3 times bec incrmtal will work 3tims
  for prb in {1..3}; do
  # updates flFmrtCrrLg variable with current timestamp formatted according to gvTsFrmtLogg
    flFmrtCrrLg=$(TZ=EDT date +"$gvTsFrmtLogg")
    #fncForIncrementalBckps function, passing flFmrtCrrLg & tmeCrr as argments, This function performs an incremental backup based on changes since tmeCrr & logs action
    fncForIncrementalBckps "$flFmrtCrrLg" "$tmeCrr"
    tmeCrr=$(date +"$nmebkpfmt")
    # introducees sleep delay of ctSlpGivnTm seconds (which initially i took 120scds) within inner loop,It contrls timing betwen conssecutive incrementl backups
    sleep $ctSlpGivnTm
  done
  #This ampersanad at endofloop mking loop run in background, script to continue executing other instructions without waiting for loop to finish, useful for running backup operations in background
done &

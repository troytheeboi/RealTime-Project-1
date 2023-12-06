#include <stdio.h>
#include<string.h>
#include<errno.h>
#include <stdlib.h>
#include<sys/ipc.h>
#include "fileReaders.h"
#include <sys/types.h>
#include <sys/shm.h>


// Function to read values from the file

int main() {
    char *argFile = "arguments .txt"; 
    char *itemfile = "item.txt"; 

    readConfigFile(argFile); //from fileReaders.h
    readItemsIntoShm(itemfile); //from fileReaders.h and
    
    return 0;
}

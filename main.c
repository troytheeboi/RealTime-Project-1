#include <stdio.h>
#include<string.h>
#include<errno.h>
#include <stdlib.h>
#include<sys/ipc.h>
#include "fileReaders.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <GL/freeglut.h>
#include<math.h>
#include"openGLfunctions.h"
#include <sys/sem.h>
#include <sys/wait.h>
#include "cashierSm_Sem.h"
#include <time.h>
#include <unistd.h>
#include "randomNumGen.h"
#include <fcntl.h>  
#include <sys/stat.h>

int mainparent; //main parent process id
pid_t allChildrent[100]; //all children process ids will be used for terminating them //TODO: terminate all


int main(int argc, char **argv) {

    mainparent = getpid();

    char *argFile = "arguments .txt"; 
    char *itemfile = "item.txt"; 

    readConfigFile(argFile); //from fileReaders.h
    readItemsIntoShm(itemfile); //from fileReaders.h and

    makeCahierShm_Sem(); //from cashierSm_Sem.h

    //TODO: make shared mem and semaphore for customers that have left to be used in termination
    //TODO: make shared mem and semaphore for cashiers that have left to be used in termination

    int pid;

    int hasEntered = 0;
    int order = 0;

    for (int i = 0; i < NUM_CASHIERS; i++ ) {

        
        //only main parent function can fork so we can get n processes not 2^n
        if(getpid()==mainparent || pid !=0){
    
            pid = fork();
            order++;
            
            if (pid  == -1) {
                printf("fork failure ... getting out\n");
                perror("fork");
            }
        
        }

    
        if (pid == 0 && hasEntered != 1){

            hasEntered = 1;
            int cashierParent = getpid();
            int scanTime = getRandomNumber(SCAN_TIME_PER_ITEM_LOWER, SCAN_TIME_PER_ITEM_UPPER);

            Cashier_arr[order].cashierId = getpid();
            Cashier_arr[order].cashierQueueSize = 0;
            Cashier_arr[order].timePerItem = scanTime;
            Cashier_arr[order].behavior = INITIAL_CASHIER_BEHAVIOR;
            Cashier_arr[order].totalItemsInQueue = 0;
            Cashier_arr[order].cashierAvailable = 1;

            char fifoName[20] ;

            sprintf(fifoName, "%d", getpid());

            if (mkfifo(fifoName, 0666) == -1) {
                perror("mkfifo");
                exit(EXIT_FAILURE);
            }
            
            strcpy(Cashier_arr[order].fifoName, fifoName);

            int pid2 = fork();

            if (pid2 == -1) {
                printf("fork failure ... getting out\n");
                perror("fork");
            }

            if(pid2 == 0){
                //TODO: cashier watcher process 
                printf("I am the watcher  => PID = %d and i = %d and my parent is %d \n ", getpid(),order,getppid());

            }

            if(getpid()==cashierParent){


                printf("I am the child  => PID = %d and i = %d and my parent is %d \n ", getpid(),order,getppid());

                //TODO: remove after test
                // wait(NULL);

            
                //TODO: cashier process

            }

            
        }
  }

    if(getpid()==mainparent){
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Cash Registers");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black

    glutMainLoop();

    }

    return 0;
}


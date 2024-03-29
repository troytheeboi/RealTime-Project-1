#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include "fileReaders.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <GL/freeglut.h>
#include <math.h>
#include "openGLfunctions.h"
#include <sys/sem.h>
#include <sys/wait.h>
#include "cashierSm_Sem.h"
#include <time.h>
#include <unistd.h>
#include "randomNumGen.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "cashierProc.h"
#include <sys/msg.h>
#include "watcher.h"
#include "arrivals.h"
#include <signal.h>

int mainparent; // main parent process id

int itemShmid;          // items array shmdid
int cashierShmid;       // cashiers array shmdid
int customerCountShmid; // customers left counter
int cashierCountShmid;  // cashiers left counter
int customersLeftSem; // customers left semaphore
int cashiersLeftSem; // cashiers left semaphore


void sigusr1_handler(int signo)
{
    if (signo == SIGUSR1)
    {

        shmctl(itemShmid, IPC_RMID, NULL);          // remove item array shm
        shmctl(cashierShmid, IPC_RMID, NULL);       // remove cashier array shm
        shmctl(customerCountShmid, IPC_RMID, NULL); // remove customer count shm
        shmctl(cashierCountShmid, IPC_RMID, NULL);  // remove cashier count shm

        semctl(cashiersSemaphore, 0, IPC_RMID); // remove cashiers array semaphore
        semctl(itemsSemaphore, 0, IPC_RMID);    // remove item array semaphore
        semctl(customersLeftSem, 0, IPC_RMID);    // remove customer counter semaphore
        semctl(cashiersLeftSem, 0, IPC_RMID);    // remove cashiers counter semaphore

        kill(0, SIGKILL); // kills all processes in the group
    }
}

int main(int argc, char **argv)
{
    if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return 1;
    }

    mainparent = getpid();

    char *argFile = "arguments .txt";
    char *itemfile = "item.txt";

    readConfigFile(argFile);                // from fileReaders.h
    itemShmid = readItemsIntoShm(itemfile); // from fileReaders.h and

    cashierShmid = makeCahierShm_Sem(); // from cashierSm_Sem.h

    key_t CustomerCountKey = ftok(".", 'z'); // Generate a key for cashiers left int shm

    if ((customerCountShmid = shmget(CustomerCountKey, sizeof(int), IPC_CREAT | 0666)) < 0)
    { // Create a shared memory segment for cashiers left int shm
        perror("shmget");
        exit(1);
    }

    int *customersLeft = (int *)shmat(customerCountShmid, 0, 0); // attach to main process memory space

    printf("shm customer  is  %p\n", customersLeft);

    if (customersLeft == (int *)-1)
    {
        perror("shmat");
        exit(1);
    }

    customersLeftSem = initSemaphores('n'); // init semaphore for cashiers left int shm

    sem_wait(customersLeftSem);   // wait for semaphore to be available
    *customersLeft = 0;           // set initial value of cashiers left to 0
    sem_signal(customersLeftSem); // release semaphore

    key_t cashierCountKey = ftok(".", 'o'); // Generate a key for cashiers left int shm

    if ((cashierCountShmid = shmget(cashierCountKey, sizeof(int), IPC_CREAT | 0666)) < 0)
    { // Create a shared memory segment for cashiers left int shm
        perror("shmget");
        exit(1);
    }

    int *cashiersLeft = (int *)shmat(cashierCountShmid, 0, 0); // attach to main process memory space

    printf("shm is  %p\n", cashiersLeft);

    if (cashiersLeft == (int *)-1)
    {
        perror("shmat");
        exit(1);
    }

    cashiersLeftSem = initSemaphores('l'); // init semaphore for cashiers left int shm

    sem_wait(cashiersLeftSem);   // wait for semaphore to be available
    *cashiersLeft = 0;           // set initial value of cashiers left to 0
    sem_signal(cashiersLeftSem); // release semaphore

    key_t key = ftok(".", 'q');              // Generate a key for queue
    int qid = msgget(key, IPC_CREAT | 0666); // Create a queue and get the queue id
    printf("this is qid %d\n", qid);
    if (qid == -1)
    {
        perror("msgget");
        exit(1);
    }

    int pid;

    int hasEntered = 0;
    int order = 0;
    long cashierType = 1L;
    long leaveType = (long)(NUM_CASHIERS + 1);

    for (int i = 0; i < NUM_CASHIERS; i++, cashierType++, leaveType++)
    {

        // only main parent function can fork so we can get n processes not 2^n
        if (getpid() == mainparent || pid != 0)
        {

            pid = fork();
            order++;

            if (pid == -1)
            {
                printf("fork failure ... getting out\n");
                perror("fork");
            }
        }

        if (pid == 0 && hasEntered != 1)
        {

            hasEntered = 1;
            int cashierParent = getpid();
            srand(time(NULL) ^ (getpid() << 16)); // seed random number generator with pid
            int scanTime = getRandomNumber(SCAN_TIME_PER_ITEM_LOWER, SCAN_TIME_PER_ITEM_UPPER);
            printf("scan time %d \n", scanTime);

            struct Cashier *Cashier_arr2 = (struct Cashier *)shmat(cashierShmid, 0, 0); // Attach the shared memory segment to cashier process's address space

            Cashier_arr2[i].cashierId = getpid();
            Cashier_arr2[i].cashierQueueSize = 0;
            Cashier_arr2[i].timePerItem = scanTime;
            Cashier_arr2[i].behavior = INITIAL_CASHIER_BEHAVIOR;
            Cashier_arr2[i].totalItemsInQueue = 0;
            Cashier_arr2[i].cashierAvailable = 1;

            Cashier_arr2[i].messsageType = cashierType;

            int pid2 = fork();

            if (pid2 == -1)
            {
                printf("fork failure ... getting out\n");
                perror("fork");
            }

            if (pid2 == 0)
            {

                int *shmCashiersL = (int *)shmat(cashierCountShmid, 0, 0);
                struct Cashier *Cashier_arr3 = (struct Cashier *)shmat(cashierShmid, 0, 0);

                watcherProcess(qid, Cashier_arr3, cashiersSemaphore, order, leaveType, shmCashiersL, cashiersLeftSem, mainparent);

                printf("I am the watcher  => PID = %d and i = %d and my parent is %d shmid %p\n ", getpid(), order, getppid(), shmCashiersL);
            }

            if (getpid() == cashierParent)
            {

                printf("I am the child  => PID = %d and i = %d and my parent is %d \n ", getpid(), order, getppid());

                CashierProcess(scanTime, qid, cashierType, leaveType, Cashier_arr2, cashiersSemaphore, order); // from cashierProc.h
            }
        }
    }

    if (getpid() == mainparent)
    {

        int forkArrivals = fork();

        if (forkArrivals == -1)
        {
            printf("fork failure... getting out\n");
            perror("fork");
        }

        if (forkArrivals == 0)
        {
            arrivalProcess(qid, itemShmid, itemsSemaphore, cashierShmid, cashiersSemaphore, customerCountShmid, customersLeftSem);
        }
        else
        {

            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
            glutInitWindowSize(windowWidth, windowHeight);
            glutCreateWindow("Cash Registers");

            glutDisplayFunc(display);
            glutReshapeFunc(reshape);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black

            glutMainLoop();
        }
    }

    return 0;
}


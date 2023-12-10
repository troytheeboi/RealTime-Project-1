#include<errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "fileReaders.h"
#include <sys/types.h>
#include <sys/shm.h>
#include "cashierSm_Sem.h"


int cashiersSemaphore;


struct{
    int cashierId;
    int cashierQueueSize;
    long messageType;
    int timePerItem;
    int behavior;
    int totalItemsInQueue;
    int cashierAvailable;
}Cashier;

struct Cashier * Cashier_arr;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};


int makeCahierShm_Sem(){

    int shmid;

    key_t key = ftok("arguments.txt", 'D'); // Generate a key for shared memory

    size_t shm_size = sizeof(Cashier) * NUM_CASHIERS; // Calculate the size of the shared memory

    if ((shmid = shmget(key, shm_size, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    Cashier_arr = (struct Cashier *) shmat(shmid, 0, 0); // Attach the shared memory segment

    if (Cashier_arr == (struct Cashier *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }



    cashiersSemaphore = initSemaphores('c');
    printf("Cashiers semaphore created with id in init %d\n",cashiersSemaphore);

    return shmid;

}

int initSemaphores(char unique) {
    
    key_t key = ftok("forkeys.txt", unique); // Generate a key for semaphores
    int semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    union semun sem_arg;

    sem_arg.val = 1;
    if (semctl(semid, 0, SETVAL, sem_arg) < 0) {
        perror("semctl");
        exit(1);
    }

    // printf("Semaphore created with id %d\n",semid);



    return semid;
}

// Semaphore wait operation
void sem_wait(int sem_id) {
    struct sembuf wait_buf = {0, -1, SEM_UNDO};
    if (semop(sem_id, &wait_buf, 1) < 0) {
        perror("sem_wait");
    }
}

// Semaphore signal operation
void sem_signal(int sem_id) {
    struct sembuf signal_buf = {0, 1, SEM_UNDO};
    if (semop(sem_id, &signal_buf, 1) < 0) {
        perror("sem_signal");
    }
}


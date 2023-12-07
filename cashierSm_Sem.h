#ifndef cashierSm_Sem_H
#define cashierSm_Sem_H


extern int cashiersSemaphore;


extern struct Cashier {
    int cashierId;
    int cashierQueueSize;
    char fifoName[50];
    int timePerItem;
    int behavior;
    int totalItemsInQueue;
}cashier;

extern struct Cashier * Cashier_arr;

int initSemaphores(char unique);
void makeCahierShm_Sem(void);
void sem_wait(int sem_id);
void sem_signal(int sem_id);

#endif
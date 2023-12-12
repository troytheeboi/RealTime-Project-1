#include "customer.h"
#include "fileReaders.h"
#include "cashierSm_Sem.h"
#include "randomNumGen.h"
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "notifier.h"
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>

struct {
    long mtype;
    int customer_id;
    int total_price;
    int numOfItems;
    long customerLeave;
}Customer;

void sigusr_handler(int signo) {
    if (signo == SIGUSR1) {
        printf("Received SIGUSR1\n");
    } else if (signo == SIGUSR2) {
        printf("Received SIGUSR2\n");
    }
}

void customerProcess(struct Item* items,int itemsSem,struct Cashier* cashiers, int cashiersSem,int* customersLeft, int customersLeftSem,int qid,long leavetype ){

    printf("entered process customer\n");

    int customerPid = getpid();

    if (signal(SIGUSR1, sigusr_handler) == SIG_ERR) {
        perror("Error setting up SIGUSR1 handler");
        exit(1);
    }

    int notifierLenghth = sizeof(struct Notifier) - sizeof(long);
    struct Notifier notifier;  
    
    srand(time(NULL) ^ (getpid()<<16)); //seed random number generator with pid

    int numOfItemtypes = getRandomNumber(1, countNonEmptyLines("item.txt"));

    struct Customer customer;

    int numItems =0;
    int price =0;

    for(int i =0; i < numOfItemtypes; i++){

        srand((time(NULL) ^ (getpid()<<16))+i); //seed random number generator with pid
        int index = getRandomNumber(0,numOfItemtypes-1);

        sem_wait(itemsSem);

        int quantity = items[index].quantity;

        if(quantity > 0){
            int willGet = getRandomNumber(1,quantity);
            items[index].quantity -= willGet;
            numItems += willGet;
            price += willGet * items[index].itemPrice;
        }

        sem_signal(itemsSem);

    }

    printf("here left loop \n");

    customer.customer_id = customerPid;
    customer.total_price = price;
    customer.numOfItems = numItems;
    customer.customerLeave = leavetype; 

    int done = 0;
    int toLeave;

    while(done == 0) {

        // printf("enter while /// \n");

        int highestScore = 0;
        int indexOfHighest;
        int score;

        sem_wait(cashiersSem);

        // printf("after sem \n");

        for(int i = 0; i < NUM_CASHIERS; i++){

            // printf("lost here? \n");

            if(cashiers[i].cashierAvailable ==1){

                score = cashiers[i].behavior / (cashiers[i].timePerItem * (cashiers[i].cashierQueueSize+1) * (cashiers[i].totalItemsInQueue+1));

                if(score >= highestScore){
                    highestScore = score;
                    indexOfHighest = i;
                }

            }

        }

        cashiers[indexOfHighest].cashierQueueSize +=1;
        cashiers[indexOfHighest].totalItemsInQueue += numItems;

        struct Cashier chosen = cashiers[indexOfHighest];

        printf("Cash chosen %ld",chosen.messsageType);

        sem_signal(cashiersSem);

        // printf("done with sem \n");

        customer.mtype = chosen.messsageType;

        printf("will send to %ld cashier", chosen.messsageType);

        if(msgsnd(qid, &customer, sizeof(struct Customer) - sizeof(long), 0) == -1){
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        int remaining = sleep(CUSTOMER_WAITING_INLINE_TIME);

        if(remaining == 0){
            done = 1;

            sem_wait(customersLeftSem);
            customersLeft+=1;
            sem_signal(customersLeftSem);

            if(*customersLeft == CUSTOMER_IMPATIENCE_THRESHOLD){
                kill(0, SIGKILL); //kills all processes in the group
            }

            raise(SIGKILL);

        }else if (remaining > 0 && (toLeave = msgrcv(qid, &notifier, notifierLenghth, leavetype, IPC_NOWAIT) == -1)){

            done =1;
            sleep(chosen.timePerItem * numItems);
            raise(SIGKILL);

        }

    }

}
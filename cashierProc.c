#include "cashierProc.h"
#include "cashierSm_Sem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "customer.h"
#include <signal.h>
#include "fileReaders.h"
#include <stdlib.h>

void zeroBehaviourHandler(int sigNum){
    if(sigNum == SIGUSR1){

    //TODO: do cleanup before leaving includes: close fifo, close semaphore, not available struct, total cashiers left ++ if threshold send parent terminate (SIGUSR1), send to customer in queue leave, deatch shared mem
        printf("Cashier woken up\n");
        //terminate 
    }
}


int is_process_alive(pid_t pid) {
    return kill(pid, 0) == 0;
}



void CashierProcess(struct Cashier* cashierArr, int TperItem, char* fifoName){

    int fd = open(fifoName, O_RDONLY);
    int totalIncome = 0;
    int remaining;
    volatile int willLeave = 0;


    if (fd == -1) {
        perror("cashier open for reading fifo");
    }

    //Define sig handler for leave now
    signal(SIGUSR1, zeroBehaviourHandler);

    while(1){

        struct Customer customer;
        ssize_t bytesRead = read(fd, &customer, sizeof(struct Customer));

        if (bytesRead == -1) {
            perror("cashier reading fifo input");
        }
        
        if(is_process_alive(customer.customer_id)){ //checks if customer process whose info is in customer struct is still alive

            totalIncome += customer.total_price;

            remaining = sleep(customer.numOfItems*TperItem); // if a signal to leave is caught but still not done scanning all items, sleep again

            
            //check if woken up to leave while scanning 
            if(remaining > 0){
                willLeave = 1;
                sleep(remaining);
            }

            //check if total surpasses threshoold if so send signal to parent to leave (use getppid)

            if(totalIncome >= INCOME_THRESHOLD){
                kill(getppid(), SIGUSR1); //send signal to parent to terminate all
            }

            kill(customer.customer_id, SIGUSR1); //send signal to customer to leave

            if(willLeave == 1){
                //TODO: cleanup
            }

            
        }
        
    }

}



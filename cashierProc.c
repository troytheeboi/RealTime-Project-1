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
#include <sys/msg.h>
#include <string.h>
#include "notifier.h"
#include <signal.h>



void CashierProcess(int TperItem, int qid , long normMessage, long leaveMessage,struct Cashier * Cashier_arr, int cahierSem, int order){


    int totalIncome = 0;

    printf("cashier normal type %ld \n",normMessage);
    printf("cashier leave type %ld \n",leaveMessage);

    while(1){

        struct Customer customer;  
        struct Notifier notifier;      

        int messageLenghth = sizeof(struct Customer) - sizeof(long);
        int notifierLenghth = sizeof(struct Notifier) - sizeof(long);

        int toLeave;
        int result;

        if((toLeave = msgrcv(qid, &notifier, notifierLenghth, leaveMessage, IPC_NOWAIT)) != -1){ // if the cashier recieves a leave message (which is type leave message) clean up resources and leave 
           
            //cleanup and ask all customers to leave queue by sending a message in queue NUM_CASHIER*2 after waking them up first
            printf("cashier leaving \n");

            while (( result = msgrcv(qid, &customer, messageLenghth, normMessage, IPC_NOWAIT)) != -1)
            {
               
               notifier.mtype = customer.mtype; 
               strcpy(notifier.mtext, "customer another queue");
               msgsnd(qid, &notifier, notifierLenghth, 0);
               kill(customer.customer_id, SIGUSR2); //wake up customer to leave queue and find another queue
            }
            raise(SIGKILL); 
            printf("cashier leaving  \n");
            break;
        }

        if(( result = msgrcv(qid, &customer, messageLenghth, normMessage, IPC_NOWAIT)) != -1){ // recieve a customer from the queue  with normal type means it was sent from actual customer
            printf("cashier recieved customer %d \n" , customer.customer_id);

            if(is_process_alive(customer.customer_id)){ //checks if customer process whose info is in customer struct is still alive
            
                // TODO: handle on customer side with remaining sleep time
                kill(customer.customer_id, SIGUSR1); //send signal to customer to wake up from sleep if in waiting in line time and sleep for scanning time then die

                totalIncome += customer.total_price;

                sleep(customer.numOfItems*TperItem);


                if(totalIncome >= INCOME_THRESHOLD){
                    kill(0, SIGKILL); //kills all processes in the group
                    //TODO: do this more gracefully maybe signal to main parent to clean up shm and semaphores and queue
                }

                printf("cashiers semaphore in process %d \n", cahierSem);

                sem_wait(cahierSem); //wait for semaphore to be available

                Cashier_arr[order].totalItemsInQueue -= customer.numOfItems; //decrease total items in queue
                printf("total items in queue %d \n", Cashier_arr[order].totalItemsInQueue);
                Cashier_arr[order].cashierQueueSize--; //decrease queue size

                sem_signal(cahierSem); //signal semaphore is available


                
            }else{
                
                sem_wait(cahierSem); //wait for semaphore to be available

                Cashier_arr[order].totalItemsInQueue -= customer.numOfItems; //decrease total items in queue
                Cashier_arr[order].cashierQueueSize--; //decrease queue size

                sem_signal(cahierSem); //signal semaphore is available
            }

        }

        

        
    }
        
}

int is_process_alive(pid_t pid) {
    return kill(pid, 0) == 0;
}








#include "fileReaders.h"
#include "randomNumGen.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "arrivals.h"
#include <time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "cashierSm_Sem.h"
#include "customer.h"

void arrivalProcess(int qid, int itemsShm, int itemsSem, int cahsiersShm, int cahsiersSem, int customerCountShm, int customerCountSem)
{

    long customerLeave = (long)(NUM_CASHIERS * 2 + 1);

    int arrivalPid = getpid();
    printf("I am arrival maker %d \n", arrivalPid);

    srand(time(NULL) ^ (getpid() << 16));

    int randArrival = getRandomNumber(CUSTOMER_ARRIVAL_RATE_LOWER, CUSTOMER_ARRIVAL_RATE_UPPER);

    printf("Arrival time: %d \n", randArrival);

    while (1)
    {

        if (getpid() == arrivalPid)
        {

            sleep(randArrival);

            customerLeave++;

            int forkAcustomer = fork();

            if (forkAcustomer == 0)
            {
                printf("I am customer %d \n", getpid());
                struct Cashier *Cashier_arr = (struct Cashier *)shmat(cahsiersShm, 0, 0); // Attach the shared memory segment
                struct Item *Item_arr2 = (struct Item *)shmat(itemsShm, 0, 0);
                int *customersLeft = (int *)shmat(customerCountShm, 0, 0); // attach to main process memory space

                // TODO: make customer process
                customerProcess(Item_arr2,itemsSem,Cashier_arr,cahsiersSem,customersLeft,customerCountSem,qid,customerLeave);
            }
        }
    }
}
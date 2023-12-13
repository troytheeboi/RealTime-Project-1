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

struct
{
    long mtype;
    int customer_id;
    int total_price;
    int numOfItems;
    long customerLeave;
} Customer;

void sigusr_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        printf("Received SIGUSR1\n");
    }
    else if (signo == SIGUSR2)
    {
        printf("Received SIGUSR2\n");
    }
}

void customerProcess(struct Item *items, int itemsSem, struct Cashier *cashiers, int cashiersSem, int *customersLeft, int customersLeftSem, int qid, long leavetype)
{

    printf("entered process customer\n");

    int customerPid = getpid();

    if (signal(SIGUSR1, sigusr_handler) == SIG_ERR) // sets up signal to be woken up from sleep
    {
        perror("Error setting up SIGUSR1 handler");
        exit(1);
    }

    int notifierLenghth = sizeof(struct Notifier) - sizeof(long);
    struct Notifier notifier;

    srand(time(NULL) ^ (getpid() << 16)); // seed random number generator with pid

    int numOfItemtypes = getRandomNumber(1, countNonEmptyLines("item.txt")); // number of item types a customer will get

    struct Customer customer;

    int numItems = 0;
    int price = 0;

    for (int i = 0; i < numOfItemtypes; i++)
    {

        srand((time(NULL) ^ (getpid() << 16)) + i);         // seed random number generator with pid
        int index = getRandomNumber(0, numOfItemtypes - 1); // index of item type

        sem_wait(itemsSem); // enter items array critical section

        int quantity = items[index].quantity;

        if (quantity > 0)
        {
            int willGet = getRandomNumber(1, ((quantity / 20) + 1)); // how many items will get from each type no less than 1 no more than total quantity

            items[index].quantity -= willGet;          // decrease quantity
            numItems += willGet;                       // num of items in customer cart will be increased
            price += willGet * items[index].itemPrice; // increase total price of customer items
        }

        sem_signal(itemsSem);
    }

    printf("here left loop \n");

    // set values in customer struct before sending
    customer.customer_id = customerPid;
    customer.total_price = price;
    customer.numOfItems = numItems;
    customer.customerLeave = leavetype;

    int shoppingTime = getRandomNumber(CUSTOMER_SHOPPING_TIME_LOWER, CUSTOMER_SHOPPING_TIME_UPPER);

    sleep(shoppingTime);

    int done = 0;
    int toLeave;
    int remaining = CUSTOMER_WAITING_INLINE_TIME; // to ensure that the total of time spent in any number of queues doesn't exceed CUSTOMER_WAITING_INLINE_TIME

    while (done == 0)
    { // this loop is so a customer could go to another queue

        float highestScore = 0;
        int indexOfHighest;
        float score;

        sem_wait(cashiersSem);

        for (int i = 0; i < NUM_CASHIERS; i++)
        { // loops over cashiers to find the highest score

            if (cashiers[i].cashierAvailable == 1)
            {
                if (cashiers[i].cashierQueueSize == 0 || cashiers[i].totalItemsInQueue == 0)
                {
                    score = (float)cashiers[i].behavior / cashiers[i].timePerItem;
                }
                else
                {
                    score = (float)cashiers[i].behavior / (cashiers[i].timePerItem * cashiers[i].cashierQueueSize * cashiers[i].totalItemsInQueue);
                }
                // score = behaviour/itemsInQueue*customersInQueue*timePerItem*cashiers[i].
                if (score > highestScore)
                {
                    highestScore = score;
                    indexOfHighest = i;
                }
            }
            // printf("score at index %d: %f\n", i, highestScore);
        }

        printf("highestScore: %f at index: %d \n", highestScore, indexOfHighest);

        cashiers[indexOfHighest].cashierQueueSize += 1; // increase apropriate values for chosen cashier
        cashiers[indexOfHighest].totalItemsInQueue += numItems;

        struct Cashier chosen = cashiers[indexOfHighest];

        printf("Cash chosen %ld \n", chosen.messsageType);

        sem_signal(cashiersSem);

        // printf("score for pid: %d is %f \n", chosen.cashierId, score);

        customer.mtype = chosen.messsageType; // send a message of message type accepted by chosen cashier

        if (msgsnd(qid, &customer, sizeof(struct Customer) - sizeof(long), 0) == -1) // send message
        {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        remaining = sleep(remaining); // check remaining sleep time

        if (remaining == 0) // if sleep time = 0 then customer has waited more than they want and became impatient
        {
            done = 1;

            sem_wait(customersLeftSem);
            *customersLeft += 1;
            printf("customers left %d -----------\n", *customersLeft);
            sem_signal(customersLeftSem);

            sem_wait(cashiersSemaphore);
            cashiers[indexOfHighest].cashierQueueSize -= 1;
            cashiers[indexOfHighest].totalItemsInQueue -= numItems;
            sem_signal(cashiersSemaphore);

            if (*customersLeft == CUSTOMER_IMPATIENCE_THRESHOLD)
            {
                msgctl(qid, IPC_RMID, NULL); // deletes queue
                kill(0, SIGKILL);            // kills all processes in the group
            }

            raise(SIGKILL);
        }
        else if (remaining > 0 && (toLeave = msgrcv(qid, &notifier, notifierLenghth, leavetype, IPC_NOWAIT) == -1)) // if the cashier had been woken up without notifier that means its turn in line has come
        {

            done = 1;
            sleep(chosen.timePerItem * numItems);
            raise(SIGKILL);
        }

        // otherwise if it is woken up with notifier that means it has to find another cashier and will loop
    }
}
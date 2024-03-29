#include "fileReaders.h"
#include "notifier.h"
#include "cashierSm_Sem.h"
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "randomNumGen.h"
#include <time.h>
#include <stdlib.h>
#include "watcher.h"

void watcherProcess(int qid, struct Cashier *Cashier_arr, int cashierSem, int order, long leave, int *haveAlreadyLeft, int cashierLeftSem,int mainparent)
{

    int start = INITIAL_CASHIER_BEHAVIOR;

    srand(time(NULL) ^ (getpid() << 16)); // seed random number generator with pid

    int interval = getRandomNumber(CASHIER_BEHAVIOUR_INTERVAL_LOWER, CASHIER_BEHAVIOUR_INTERVAL_UPPER);
    int decrement = getRandomNumber(CASHIER_BEHAVIOR_DECREMENT_LOWER, CASHIER_BEHAVIOR_DECREMENT_UPPER);

    printf("interval; %d, decrement; %d\n", interval, decrement);

    while (start > 0)
    {

        sleep(interval);

        sem_wait(cashierSem);

        Cashier_arr[order].behavior -= decrement;

        sem_signal(cashierSem);

        start -= decrement;
    }

    // cashier no longer available
    sem_wait(cashierSem);
    Cashier_arr[order].cashierAvailable = 0;
    sem_signal(cashierSem);

    struct Notifier notifLeave;

    notifLeave.mtype = leave;
    strcpy(notifLeave.mtext, "leave");

    // notify cashier to leave
    msgsnd(qid, &notifLeave, sizeof(struct Notifier) - sizeof(long), 0);

    // increases the number of cashiers that have left
    sem_wait(cashierLeftSem);
    *haveAlreadyLeft += 1;
    printf("cashier left %d \n", *haveAlreadyLeft);
    printf("left threshold %d \n", CASHIER_LEFT_THRESHOLD);

    if (*haveAlreadyLeft == CASHIER_LEFT_THRESHOLD)
    {

        msgctl(qid, IPC_RMID, NULL);

        kill(mainparent, SIGUSR1);
    }

    sem_signal(cashierLeftSem);

    raise(SIGKILL);
}
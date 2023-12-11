#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 256

// Define the message structure
struct Customer
{
    long mtype;
    int customer_id;
    int total_price;
    int numOfItems;
};

struct Notifier
{
    long mtype;
    char mtext[20];
};

int main() {
    // Get qid from the user
    int qid;
    printf("Enter qid: ");
    scanf("%d", &qid);
    
    long normal;
    printf("enter normal type:");
    scanf("%ld",&normal);

    // Create a message
    struct Customer myMessage;
    myMessage.mtype = normal; // Message type must be greater than 0
    myMessage.customer_id = getpid();
    myMessage.total_price =5;
    myMessage.numOfItems =3;
    

    // Send the message to the queue
    if (msgsnd(qid, &myMessage, sizeof(struct Customer) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to the queue.\n");
    
    sleep(5);
    
    long leave;
    printf("enter leave type:");
    scanf("%ld",&leave);
    
    struct Notifier myMessage2;
    myMessage2.mtype = leave; // Message type must be greater than 0
   

    // Send the message to the queue
    if (msgsnd(qid, &myMessage2, sizeof(struct Notifier) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

   sleep(20);

    return 0;
}


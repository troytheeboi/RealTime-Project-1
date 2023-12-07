#include "fileReaders.h"
#include <stdio.h>
#include<string.h>
#include<errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <GL/freeglut.h>
#include"cashierSm_Sem.h"


int CUSTOMER_ARRIVAL_RATE_LOWER;
int CUSTOMER_ARRIVAL_RATE_UPPER;
int CUSTOMER_SHOPPING_TIME_LOWER;
int CUSTOMER_SHOPPING_TIME_UPPER;
int CUSTOMER_IMPATIENCE_THRESHOLD;
int SCAN_TIME_PER_ITEM_LOWER;
int SCAN_TIME_PER_ITEM_UPPER;
int NUM_CASHIERS;
int INITIAL_CASHIER_BEHAVIOR;
int CASHIER_BEHAVIOUR_INTERVAL;
int CASHIER_BEHAVIOR_DECREMENT;
int CUSTOMER_WAITING_INLINE_TIME;
int CUSTOMER_LEFT_THRESHOLD;
int CASHIER_LEFT_THRESHOLD;
int INCOME_THRESHOLD;

int itemsSemaphore;

struct{
    int itemPrice;
    char itemName[100]; 
    int quantity;
}Item;

struct Item * Item_arr;

int itemCount;



void readConfigFile(const char *filename) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        perror("Error opening file");
        return;
    }
    
    char line[500];
    // Loop through each line in the file
    while (fgets(line, sizeof(line), file) != NULL) {

        char *token = strtok(line, ":");

        if (token == NULL) {
            // Check for the end of file
            break;
        }

        char variableName[500];
        strcpy(variableName,token);

        token = strtok(NULL, ":");

        if (token == NULL) {
            // Check for the end of file
            break;
        }

        char valString[50];
        strcpy(valString,token);

        int value;

        value = atoi(valString);

        // Assign the value to the corresponding global variable
        if (strcmp(variableName, "CUSTOMER_ARRIVAL_RATE_LOWER") == 0) {
            CUSTOMER_ARRIVAL_RATE_LOWER = value;
        } else if (strcmp(variableName, "CUSTOMER_ARRIVAL_RATE_UPPER") == 0) {
            CUSTOMER_ARRIVAL_RATE_UPPER = value;
        } else if (strcmp(variableName, "CUSTOMER_SHOPPING_TIME_LOWER") == 0) {
            CUSTOMER_SHOPPING_TIME_LOWER = value;
        }else if(strcmp(variableName, "CUSTOMER_SHOPPING_TIME_UPPER") == 0){
            CUSTOMER_SHOPPING_TIME_UPPER = value;
        }else if (strcmp(variableName, "CUSTOMER_IMPATIENCE_THRESHOLD") == 0){
            CUSTOMER_IMPATIENCE_THRESHOLD = value;
        }else if (strcmp(variableName, "SCAN_TIME_PER_ITEM_LOWER") == 0){
            SCAN_TIME_PER_ITEM_LOWER = value;
        }else if (strcmp(variableName, "SCAN_TIME_PER_ITEM_UPPER") == 0){
            SCAN_TIME_PER_ITEM_UPPER = value;
        }else if (strcmp(variableName, "NUM_CASHIERS") == 0){
            NUM_CASHIERS = value;
        }else if (strcmp(variableName, "INITIAL_CASHIER_BEHAVIOR") == 0){
            INITIAL_CASHIER_BEHAVIOR = value;
        }else if (strcmp(variableName, "CASHIER_BEHAVIOUR_INTERVAL") == 0){
            CASHIER_BEHAVIOUR_INTERVAL = value;
        }else if (strcmp(variableName, "CASHIER_BEHAVIOR_DECREMENT") == 0){
            CASHIER_BEHAVIOR_DECREMENT = value;
        }else if (strcmp(variableName, "CUSTOMER_WAITING_INLINE_TIME") == 0){
            CUSTOMER_WAITING_INLINE_TIME = value;
        }else if (strcmp(variableName, "CUSTOMER_LEFT_THRESHOLD") == 0){
            CUSTOMER_LEFT_THRESHOLD = value;
        } else if (strcmp(variableName, "CASHIER_LEFT_THRESHOLD") == 0){
            CASHIER_LEFT_THRESHOLD = value;
        }else {
            INCOME_THRESHOLD = value;
        }

        
    }

    fclose(file);
}

void readItemsIntoShm(const char * filename){

    key_t key = ftok("item.txt", 'R');
    int shmid;

    int numOfItems = countNonEmptyLines(filename);

    
    // Calculate the size of the shared memory
    size_t shm_size = sizeof(Item) * numOfItems;

    // Create a shared memory segment
    if ((shmid = shmget(key, shm_size, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory segment to the process's address space
    Item_arr = (struct Item *)shmat(shmid, NULL, 0);

    
    if (Item_arr == (struct Item*)-1) {
        perror("shmat");
        exit(1);
    }
    

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
    }

    // Assuming a maximum of 20 items, you can adjust as needed
    itemCount = 0;

    char line[256];  // Assuming a maximum line length of 100, you can adjust as needed

    while (fgets(line, sizeof(line), file) != NULL) {
        // Using sscanf to parse the line into the struct members
        char* token = strtok(line,",");

        if (token == NULL) {
            // Check for the end of file
            break;
        }


        strcpy(Item_arr[itemCount].itemName,token);

        token = strtok(NULL,",");

         if (token == NULL) {
            // Check for the end of file
            break;
        }

        Item_arr[itemCount].quantity = atoi(token);

        token = strtok(NULL,",");

         if (token == NULL) {
            // Check for the end of file
            break;
        }
        
        Item_arr[itemCount].itemPrice = atoi(token);
        
        itemCount++;
     
    }

    // Close the file
    fclose(file);

    itemsSemaphore = initSemaphores('i');

  
}

int countNonEmptyLines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; // Return -1 to indicate an error
    }

    int count = 0;
    char line[1000]; // Adjust the size based on your needs

    while (fgets(line, sizeof(line), file) != NULL) {
        // Check if the line is not empty
        if (line[0] != '\n' && line[0] != '\0') {
            count++;
        }
    }

    fclose(file);

    return count;
}


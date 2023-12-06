#include "fileReaders.h"
#include <stdio.h>
#include<string.h>
#include<errno.h>
#include <stdlib.h>


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

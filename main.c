#include <stdio.h>
#include<string.h>
#include<errno.h>
#include <stdlib.h>
#include<sys/ipc.h>
#include "fileReaders.h"
#include <sys/types.h>
#include <sys/shm.h>


typedef struct{
    int itemPrice;
    char itemName[100]; 
    int quantity;
}Item;

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


// Function to read values from the file

int main() {
    char *argFile = "arguments .txt"; // Replace with your actual file name

    readConfigFile(argFile); //from fileReaders.h

    key_t key = ftok("item.txt", 'R');
    int shmid;

    int numOfItems = countNonEmptyLines("item.txt");

    
    // Calculate the size of the shared memory
    size_t shm_size = sizeof(Item) * numOfItems;

    // Create a shared memory segment
    if ((shmid = shmget(key, shm_size, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory segment to the process's address space
    Item * Item_arr = (Item *)shmat(shmid, NULL, 0);

    
    if (Item_arr == (Item*)-1) {
        perror("shmat");
        exit(1);
    }

    

    FILE *file = fopen("item.txt", "r");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

      // Assuming a maximum of 20 items, you can adjust as needed
    int itemCount = 0;

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

    // Now, you have the data in the 'items' array of structs
    // You can use the 'itemCount' variable to know how many items were read

    // Example: Print the items
    for (int i = 0; i < itemCount; i++) {
        printf("Item Name: %s, Quantity: %d, Price: %d\n", Item_arr[i].itemName, Item_arr[i].quantity, Item_arr[i].itemPrice);
    }


    
    return 0;
}

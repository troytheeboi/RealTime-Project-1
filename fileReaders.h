#ifndef FILEREADER_H
#define FILEREADER_H

extern int CUSTOMER_ARRIVAL_RATE_LOWER;
extern int CUSTOMER_ARRIVAL_RATE_UPPER;
extern int CUSTOMER_SHOPPING_TIME_LOWER;
extern int CUSTOMER_SHOPPING_TIME_UPPER;
extern int CUSTOMER_IMPATIENCE_THRESHOLD;
extern int SCAN_TIME_PER_ITEM_LOWER;
extern int SCAN_TIME_PER_ITEM_UPPER;
extern int NUM_CASHIERS;
extern int INITIAL_CASHIER_BEHAVIOR;
extern int CASHIER_BEHAVIOUR_INTERVAL;
extern int CASHIER_BEHAVIOR_DECREMENT;
extern int CUSTOMER_WAITING_INLINE_TIME;
extern int CUSTOMER_LEFT_THRESHOLD;
extern int CASHIER_LEFT_THRESHOLD;
extern int INCOME_THRESHOLD;

extern struct Item{
    int itemPrice;
    char itemName[100]; 
    int quantity;
}item;

extern struct Item * Item_arr;

extern int itemCount;
extern int itemsSemaphore;

void readConfigFile(const char *filename);
void readItemsIntoShm(const char *filename);
int countNonEmptyLines(const char *filename);

#endif

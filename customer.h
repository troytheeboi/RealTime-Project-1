#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "fileReaders.h"
#include "cashierSm_Sem.h"


extern struct Customer
{
    long mtype;
    int customer_id;
    int total_price;
    int numOfItems;
    long customerLeave;
}cus;

void customerProcess(struct Item* items,int itemsSem,struct Cashier* cashiers, int cashiersSem,int* customersLeft, int customersLeftSem,int qid,long leavetype );


#endif
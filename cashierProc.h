#include "cashierSm_Sem.h"
#include <sys/types.h>

#ifndef CASHIERPROC_H
#define CASHIERPROC_H

void CashierProcess(int TperItem, int qid , long normMessage, long leaveMessage,struct Cashier* Cashier_arr, int cashierSem,int order);
int is_process_alive(pid_t pid);

#endif
#ifndef WATCHER_H
#define WATCHER_H

void watcherProcess(int qid,struct Cashier* Cashier_arr, int cashierSem, int order, long leave, int* haveAlreadyLeft, int cashierLeftSem);

#endif

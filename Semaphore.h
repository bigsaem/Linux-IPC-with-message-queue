//
// Created by bigsaem on 2020-02-25.
//

#ifndef ASSIGNMENT2_SEMAPHORE_H
#define ASSIGNMENT2_SEMAPHORE_H

#include <sys/sem.h>
#include <errno.h>
#include "Server.h"
int     initsem (key_t key);
void    wait_s(int sid);
void    signal_s(int sid);
#endif //ASSIGNMENT2_SEMAPHORE_H

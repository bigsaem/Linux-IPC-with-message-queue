//
// Created by bigsaem on 2020-02-25.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include "mesg.h"
#include "Semaphore.h"

//this struct is to contain reading buffer and for synchronization between the main process and thread
typedef struct
{
    bool produced;
    Mesg rbuf;
} Cstruct;

int send_message( int qid, int sid, Mesg *qbuf );
int read_message (int qid, long type, Cstruct *rbuf);
void* ThreadFunc (void * clientStruct);
#endif //CLIENT_CLIENT_H

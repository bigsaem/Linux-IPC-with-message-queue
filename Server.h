//
// Created by bigsaem on 2020-02-25.
//

#ifndef ASSIGNMENT2_SERVER_H
#define ASSIGNMENT2_SERVER_H
#include "mesg.h"
#include "Semaphore.h"
#define PRIORITY1 1
#define PRIORITY2 500
#define PRIORITY3 4000

int  open_queue          (key_t keyval);
int  send_message        (int qid, int sid, Mesg *qbuf );
int  read_message        (int qid, int sid, long type, Mesg *rqbuf );
int  send_file           (int qid, int sid, Mesg *sbuf, char * filepath);
/*-------- remove shared memory segments & semaphores ------------*/
void remove_ipc (int signo);
#endif //ASSIGNMENT2_SERVER_H

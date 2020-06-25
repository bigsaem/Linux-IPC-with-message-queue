//
// Created by bigsaem on 2020-02-25.
//

#ifndef ASSIGNMENT2_MESG_H
#define ASSIGNMENT2_MESG_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <pthread.h>
#define SKEY 100
#define QKEY 200
#define MAXPACKETSIZE 4096
#define PACKETINFOSIZE 24
#define MAXMESSAGEDATA MAXPACKETSIZE-PACKETINFOSIZE /* don't want sizeof(Mesg) > 4096 */
#define MESGHDRSIZE (sizeof(Mesg) - MAXMESSAGEDATA) /* length of mesg_len and mesg_type */
typedef struct
{
    long mesg_type; /* message type */
    long pid;
    int  priority;
    int  mesg_len; /* #bytes in mesg_data */
    char mesg_data [MAXMESSAGEDATA];
} Mesg;
#endif //ASSIGNMENT2_MESG_H

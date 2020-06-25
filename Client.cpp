/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Client.cpp - An application that will send a file path to server and receive the content of the file.
--
-- PROGRAM: client
--
-- FUNCTIONS:
-- int send_message (int qid, int sid, Mesg *qbuf );
-- int read_message (int qid, long type, Cstruct *rbuf);
-- void* ThreadFunc (void * clientStruct);
--
--
--
--
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- NOTES:
-- This program gets the created message queue and semaphore for communication with server.
-- It sends a file path to the server and prints the messages retrieved by the server, which is the content of the file.
-- This program creates a thread for printing what it receives, and the main process keeps receiving the messages from
-- the queue. This program terminates when it receives a message that has size -1.
----------------------------------------------------------------------------------------------------------------------*/

#include "Client.h"
using namespace std;
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: int main () //this function accepts two arguments on terminal, which are priority(int) and file path(string)
--
-- RETURNS: int
--
-- NOTES:
-- This function is the driver function of the program. It starts the program and gets a message queue and semaphore id.
-- By having an infinite loop, it keeps receiving a message from server, which contains a file content.
-- Every time it receives messages, it changes a flag in the shared struct between the main process and created thread,
-- so that threads print the content of the message on the console.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    int         qid, sidq, result = -1;
    key_t       qkey, skey;
    long        myPid = getpid();
    Mesg        sbuf;
    Cstruct     cst; //Client structure
    pthread_t   ThreadID;

    if(argc <=2){
        printf("parameter: priority, filepath");
        exit(1);
    } else {
        sbuf.priority = atoi(argv[1]);
        strcpy(sbuf.mesg_data, argv[2]);
    }

    skey = SKEY;
    sidq = semget(skey, 1, 0);
    qkey = QKEY;
    qid = msgget (qkey, 0 );

    sbuf.mesg_type = 1;
    sbuf.pid = myPid;
    sbuf.mesg_len = MAXMESSAGEDATA;

    while(result == -1){
        result=send_message(qid, sidq, &sbuf);
    }

    printf("file path sent\n");
    cst.produced = false; //synchronization flag

    //thread for printing receiving buffer
    pthread_create (&ThreadID, NULL, ThreadFunc, (void*)&cst);

    result = read_message(qid, myPid, &cst);
    return 0;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: int main ()
--
-- RETURNS: int
--
-- NOTES:
-- This function is the driver function of the program. It starts the program and gets a message queue and semaphore id.
-- By having an infinite loop, it keeps receiving a message from server, which contains a file content.
-- Every time it receives messages, it changes a flag in the shared struct between the main process and created thread,
-- so that threads print the content of the message on the console.
----------------------------------------------------------------------------------------------------------------------*/
int send_message( int qid, int sid, Mesg *sbuf )
{
    int result, length;
    length = sizeof (Mesg) - sizeof(long);

    wait_s(sid);
    if( (result = msgsnd ( qid, sbuf, length, IPC_NOWAIT) ) == -1)
    {
        perror("(get status)");
    }
    signal_s(sid);

    return (result);
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: read_message
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: int read_message (int qid, long type, Cstruct *cst)
--            int qid: message queue id
--            long type: message type //process id
--            Cstruct *cst //client struct that has synchronization flag and receiving buffer
--
-- RETURNS: int //0 when successfully read, otherwise -1
--
-- NOTES:
-- This function reads the messages from server that contains the file content.
-- It is a blocking function, so it goes to the next step when it receives a message.
-- It is synchronized with a printing thread by using a flag in the shared structure to avoid that the thread keeps
-- printing the buffer without it's filled with a new message.
-- It return when a message that has size -1 is received, regarding that as the end of file.
----------------------------------------------------------------------------------------------------------------------*/
int read_message (int qid, long type, Cstruct *cst)
{
    int result, length;
    length = sizeof(Mesg) - sizeof(long);
    while(true && cst->rbuf.mesg_len != -1){
        if(!cst->produced){
            result = msgrcv ( qid, &cst->rbuf, length, type, 0);
            cst->produced = true;
        }
    }
    return(result);
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ThreadFunc
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- void* ThreadFunc (void * clientStruct)
--            void * clientStruct: Client struct will be passed.
--
-- RETURNS: void*
--
-- NOTES:
-- This function prints the content of the receiving buffer.
-- It is synchronized with the main process with the flag of the shared structure, so it only reads when a buffer is
-- filled with a new message.
-- This thread terminates when a message that has data size -1.
----------------------------------------------------------------------------------------------------------------------*/
void* ThreadFunc (void * clientStruct)
{
    Cstruct *cst = (Cstruct *)clientStruct;
    while(true && cst->rbuf.mesg_len != -1){
        if(cst->produced){
            cout<<cst->rbuf.mesg_data;
            fflush(stdout);
            cst->produced = false;
        }
    }
    return NULL;
}

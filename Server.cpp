/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Server.cpp - An application that will receive a file path from clients and send the content of the
--                           file.
--
-- PROGRAM: server
--
-- FUNCTIONS:
-- int  open_queue          (key_t keyval);
-- int  send_message        (int qid, int sid, Mesg *qbuf );
-- int  read_message        (int qid, int sid, long type, Mesg *rqbuf );
-- int  send_file           (int qid, int sid, Mesg *sbuf, char * filepath);
--
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- NOTES:
-- This program opens a message queue and wait for client's message that contains a file path.
-- It opens the file path and send the content back to the client.
-- This program will create a process per client to handle its request, so it can handle multiple clients at the same
-- time. For synchronization for sending a message on the queue, a semaphore is used on each process.
-- This program also check the priority of the message sent by clients, and change the buffer size of the message to
-- send to differentiate the process speed on client.
----------------------------------------------------------------------------------------------------------------------*/

#include "Server.h"

using namespace std;
int qid, sid; //global for signal
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
-- This function is the driver function of the program. It starts the program and create a message queue and semaphore.
-- By having an infinite loop, it keeps receiving a message from client, which contains a file path.
-- If it receives a message, a process is created to handle that request.
----------------------------------------------------------------------------------------------------------------------*/

int main() {
    key_t qkey, skey;
    int result;
    Mesg rbuf;
    struct sigaction act;

    skey = SKEY;
    qkey = QKEY;

    if((sid=initsem(skey)) == -1){
        perror ("semaphore creation (get status)failed!");
        exit(1);
    }
    signal_s(sid); //initial signal

    if ((qid = open_queue (qkey)) < 0)
    {
        perror ("msgget failed!");
        exit(2);
    }

    printf("A message queue and a semaphore have been created.\n");

    act.sa_handler = remove_ipc; //ctrl+c command handler
    act.sa_flags = 0;
    fflush(stdout);
    while(true){
        if((result = read_message(qid, sid, 1, &rbuf)) != -1){
            printf("Received: %s\n", rbuf.mesg_data);
            fflush(stdout);
            if(fork()==0){
                printf("%d Connected \n", getpid());
                Mesg sbuf;
                sbuf.pid = getpid();
                sbuf.mesg_type= rbuf.pid;
                sbuf.priority = rbuf.priority;
                if((result = send_file(qid, sid, &sbuf, rbuf.mesg_data) == -1)){
                    perror("send file failed!");
                    strcpy(sbuf.mesg_data, "File read failed\n");
                    send_message(qid, sid, &sbuf);
                    //terminating message
                    sbuf.mesg_len = -1;
                    send_message(qid, sid, &sbuf);
                    printf("Fail message sent\n");
                } else{
                    printf("The file has been successfully sent to %d!", sbuf.mesg_type);
                    fflush(stdout);
                }
                return 0;
            }
        }
    }
    return 0;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: open_queue
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: int open_queue (key_t keyval)
--                        key_t keyval: it's a key value to create a message queue
--
-- RETURNS: int qid //returns the id of the message queue created
--
-- NOTES:
-- This function opens a message queue according the passed key value, if it doesn't exist, it creates a new queue.
----------------------------------------------------------------------------------------------------------------------*/
int open_queue (key_t keyval)
{
    int qid;
    if( (qid = msgget (keyval, IPC_CREAT | 0660 ) ) == -1 )
    {
        return (-1);
    }
    return qid;
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
-- INTERFACE: int read_message (int qid, int sid, long type, Mesg *rbuf)
--            int qid: message queue id
--            int sid: semaphore id
--            long type: message type //the message type from client is 1
--            Mesg *rbuf: reading buffer
--
-- RETURNS: int receive_result //it return 0 when success, otherwise -1
--
-- NOTES:
-- This function read the message from the message queue.
-- The message function is blocking function since the main process will be just in charge of reading the message.
----------------------------------------------------------------------------------------------------------------------*/
int read_message (int qid, int sid, long type, Mesg *rbuf)
{
    int result, length;
    length = sizeof(Mesg) - sizeof(long);
    result = msgrcv ( qid, rbuf, length, type, 0);
    return(result);
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: send_file
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: int send_file(int qid, int sid,  Mesg *sbuf, char * filepath)
--            int qid: message queue id
--            int sid: semaphore id
--            Mesg *sbuf: sending buffer
--            char * filepath: the file path received
--
-- RETURNS: int send_result //it return 0 when success, otherwise -1
--
-- NOTES:
-- This function read tries opening the file first and return -1 if it fails.
-- When it successfully opens the file, it reads the file up to the specified message size and send the message.
-- This function also varies the sending buffer size depending on the priority of clients by deviding the full buffer
-- size by predefined numbers;
----------------------------------------------------------------------------------------------------------------------*/
int send_file(int qid, int sid,  Mesg *sbuf, char * filepath) {
    int         result = -1;
    int         priority = sbuf->priority;
    long        flength;
    int         bufSize = MAXMESSAGEDATA;
    char        tempBuf[bufSize];
    switch(priority)
    {
        case 1:
            bufSize = bufSize/PRIORITY1;
            break;
        case 2:
            bufSize = bufSize/PRIORITY2;
            break;
        case 3:
            bufSize = bufSize/PRIORITY3;
            break;
    }

    ifstream    is(filepath);
    memset(tempBuf, 0, sizeof(tempBuf));
    sbuf->mesg_len = bufSize;

    if(!is) return -1;

    printf("file opened\n");

    //file length check
    is.seekg(0,is.end);
    flength = is.tellg();
    is.seekg(0,is.beg);

    while(is.tellg() != flength && is.tellg()!= -1){
        is.read(tempBuf, bufSize);
        strncpy(sbuf->mesg_data, tempBuf,bufSize);
        while(result == -1){
            result = send_message(qid, sid, sbuf);
        }
        memset(tempBuf, 0, sizeof(tempBuf));
        memset(sbuf->mesg_data,0, MAXMESSAGEDATA);
        result = -1;
    }
    sbuf->mesg_len = -1;
    result = send_message(qid, sid, sbuf);
    return result;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: send_message
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: int send_message( int qid, int sid, Mesg *sbuf)
--            int qid: message queue id
--            int sid: semaphore id
--            Mesg *sbuf: sending buffer
--
-- RETURNS: int send_result //it return 0 when success, otherwise -1
--
-- NOTES:
-- This function simply sends the buffer passed as a parameter to message queue.
-- Since the message queue should be accessed by one process at a time, a semaphore is being used, so the process has to
-- wait first and send the message to the client, then signals the semaphore.
----------------------------------------------------------------------------------------------------------------------*/
int send_message( int qid, int sid, Mesg *sbuf)
{
    int result, length;
    length = sizeof (Mesg) - sizeof(long);

    wait_s(sid);
    if( (result = msgsnd ( qid, sbuf, length, 0) ) == -1)
    {
        printf("error\n");
        perror("(get status)");
    }
    signal_s(sid);

    //printf("%d\n", sbuf->priority);
    fflush(stdout);
    return (result);
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: remove_ipc
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: void remove_ipc (int signo)
--            int signo: signal number
--
-- RETURNS: void
--
-- NOTES:
-- This function terminates the created message queue and semaphore when ctrl+c is typed on terminal.
----------------------------------------------------------------------------------------------------------------------*/
void remove_ipc (int signo)
{
    if (msgctl (qid, IPC_RMID, 0) < 0)
        perror ("msgctl");
    if (semctl (sid, 0, IPC_RMID, 0) < 0)
        perror ("semctl");
}
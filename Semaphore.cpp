#include "Semaphore.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: semaphore.cpp - semaphore helper function to wait, signal and initialize it.
--
-- PROGRAM: server, client
--
-- FUNCTIONS:
-- int initsem (key_t key);
-- void wait_s(int sid);
-- void signal_s(int sid);
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- NOTES:
-- This helper file provides functions to initialize a semaphore, wait and signal by manipulating semaphore integer value.
----------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: initsem
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: int initsem (key_t key)
--            key_t key: semaphore key.
--
-- RETURNS: int //semaphore id
--
-- NOTES:
-- This function creates a semaphore and set the initial value to 0. If a semaphore already exists, it gets the id of it.
----------------------------------------------------------------------------------------------------------------------*/
int initsem (key_t key)
{
    int sid, errno, status = 0;

    if ((sid = semget((key_t)key, 1, 0666|IPC_CREAT)) == -1)
    {
        if (errno == EEXIST)
        sid = semget ((key_t)key, 1, 0);
    }
    else   /* if created */
    status = semctl (sid, 0, SETVAL, 0);

    if ((sid == -1) || status == -1)
    {
        perror ("initsem failed\n");
        return (-1);
    }
    else
        return (sid);
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: wait_s
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: wait_s(int sid)
--            int sid: semaphore id
--
-- RETURNS: void
--
-- NOTES:
-- This function decrease the semaphore value by 1 for a process to wait for its turn to access to the critical section.
----------------------------------------------------------------------------------------------------------------------*/
void wait_s(int sid)     /* acquire semophore */
{
    struct sembuf *sembuf_ptr;

    sembuf_ptr= (struct sembuf *) malloc (sizeof (struct sembuf *) );
    sembuf_ptr->sem_num = 0;
    sembuf_ptr->sem_op = -1;
    sembuf_ptr->sem_flg = SEM_UNDO;

    if ((semop(sid,sembuf_ptr,1)) == -1)
        printf("semop error\n");
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: signal_s
--
-- DATE: March 03, 2020
--
-- DESIGNER: Sam Lee
--
-- PROGRAMMER: Sam Lee
--
-- INTERFACE: signal_s(int sid)
--            int sid: semaphore id
--
-- RETURNS: void
--
-- NOTES:
-- This function increases the semaphore value by 1 for a process to be able to access to the critical section.
----------------------------------------------------------------------------------------------------------------------*/
void signal_s(int sid)     /* release semaphore */
{
    struct sembuf *sembuf_ptr;

    sembuf_ptr= (struct sembuf *) malloc (sizeof (struct sembuf *) );
    sembuf_ptr->sem_num = 0;
    sembuf_ptr->sem_op = 1;
    sembuf_ptr->sem_flg = SEM_UNDO;

    if ((semop(sid,sembuf_ptr,1)) == -1)
        printf("semop error\n");
}

/*
 * MemSemHandle.c
 *
 *  Created on: Feb 18, 2014
 *      Author: gioulisapo
 */
#include "MemSemHandle.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <sys/sem.h>

#define MAXSIZE 5000
#define SEMKEYPATH "/dev/null"  /* Path used on ftok for semget key  */
#define SEMKEYID 1              /* Id used on ftok for semget key    */


void RemoveSem(int semid)
{
	union semun
	{
		int val;                /* value for SETVAL */
		struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
		ushort *array;          /* array for GETALL & SETALL */
		struct seminfo *__buf;  /* buffer for IPC_INFO */
		void *__pad;
	}Mysemun;
	if((semctl(semid, 0, IPC_RMID, Mysemun)) < 0)
	{
		printf(/*"ERROR in semctl()\n\aSYSTEM: %s\n\n", strerror(errno)*/"Error in remove sems");
		return;
	}

}

int CreateSems()
{
	int semid;
	key_t semkey;
	struct sembuf operations[1];
	short  sarray[1];

	/* Generate an IPC key for the semaphore set and the shared      */
	/* memory segment.  Typically, an application specific path and  */
	/* id would be used to generate the IPC key.                     */
	semkey = ftok(SEMKEYPATH,SEMKEYID);
	if ( semkey == (key_t)-1 )
	  {
		printf("main: ftok() for sem failed\n");
		return -1;
	  }

	/* Create a semaphore set using the IPC key.  The number of      */
	/* semaphores in the set is two.  If a semaphore set already     */
	/* exists for the key, return an error. The specified permissions*/
	/* give everyone read/write access to the semaphore set.         */

	semid = semget( semkey, 1, 0666 | IPC_CREAT | IPC_EXCL );
	if ( semid == -1 )
	  {
		printf("main: semget() failed\n");
		return -1;
	  }

	/* Initialize the first semaphore in the set to 0 and the        */
	sarray[0] = 0;

	if(semctl( semid, 0, SETALL, sarray) == -1)		//Set all sem values to 1
	{
		printf("main: semctl() initialization failed\n");
		return -1;
	}
		return semid;
}


void down(int semid, int numOfSem)
{
	struct sembuf down={numOfSem, -1, 0};

	if((semop(semid, &down, 1)) < 0)
		printf("ERROR in down %d",errno);
}

void up(int semid, int numOfSem)
{
	struct sembuf up={numOfSem, 1, 0};

	if((semop(semid, &up, 1)) < 0)
		printf("ERROR in up");
}

int GetSems()
{
	struct sembuf operations[1];
	int semid;
	key_t semkey;

	/* Generate an IPC key for the semaphore set
	* Typically, an application specific path and  */
	/* id would be used to generate the IPC key.                     */
	semkey = ftok(SEMKEYPATH,SEMKEYID);
	if ( semkey == (key_t)-1 )
	{
		printf("main: ftok() for sem failed\n");
		return -1;
	}
	/* Get the already created semaphore ID associated with key.     */
	/* If the semaphore set does not exist, then it will not be      */
	/* created, and an error will occur.                            */
	semid = semget( semkey, 1, 0666);
	if ( semid == -1 )
	{
		//printf("main: semget() failed\n");
		return -1;
	}
	return semid;
}


/*
 * MemSemHandle.h
 *
 *  Created on: Feb 18, 2014
 *      Author: gioulisapo
 */

#ifndef MEMSEMHANDLE_H_
#define MEMSEMHANDLE_H_

/*void RemoveSharedMem(int shmid);*/
void RemoveSem(int semid);
void up(int semid, int numOfSem);
void down(int semid, int numOfSem);
int CreateSems();
int GetSems();


#endif /* MEMSEMHANDLE_H_ */

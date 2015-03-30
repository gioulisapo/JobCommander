/*
 * queue.h
 *
 *  Created on: Jun 11, 2014
 *      Author: gioulisapo
 */

#ifndef QUEUE_H_
#define QUEUE_H_
#define MAXLEN 400
#define BUF_SIZE 15


typedef struct qNode* queuePtr;
typedef struct qNode
{
	long jobId;
	char * job;
	queuePtr next;
}queueNode;

queuePtr AddItem (queuePtr head, long JobId, char* job);//Adds item to queue
char *ReturnQueue(queuePtr head);//Returns as a string the selected queue in a printable format
int DeleteItem(queuePtr *p, int JobId);//deletes item with id JobId
int ReturnFirstId(queuePtr head);//Returns the jobId of the first element of the queue
int CountItems (queuePtr head);//Counts # items in queue
char *ReturnDeleteFirstItem(queuePtr *head);//Returns the first item as a String "$JobId $job" and deletes it from the queue
#endif /* QUEUE_H_ */

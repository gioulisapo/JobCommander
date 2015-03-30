/*
 * queue.c
 *
 *  Created on: Jun 11, 2014
 *      Author: gioulisapo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"


queuePtr AddItem(queuePtr p, long JobId, char* job)
{
	int cond;
	if (p == NULL)
	{
		p = malloc(sizeof(queueNode));
		p->job = malloc((strlen(job) + 1) * sizeof(char));
		strcpy(p->job, job);
		p->jobId = JobId;
		p->next = NULL;
	}
	else
		p->next = AddItem(p->next, JobId, job);
	return p;
}
int DeleteItem(queuePtr *p, int JobId)
/* Delete element v from list, if it exists
 */
{
	queuePtr tempqueue;
	while ((*p) != NULL)
		if ((*p)->jobId == JobId)
		{
			tempqueue = *p;
			*p = (*p)->next;
			free(tempqueue);
			return 1;
		}
		else
			p = &((*p)->next);
	return 0;
}

int CountItems(queuePtr head)
{
	int i = 0;
	queuePtr p = NULL;
	for (p = head; p != NULL; p = p->next)
		i++;
	return i;
}

char *ReturnQueue(queuePtr head)
{
	char *reString = (char*)malloc(sizeof(char)*MAXLEN);
	reString[0] = '\0';
	queuePtr tempqueue = head;
	char buf[BUF_SIZE];
	while (tempqueue != NULL)
	{
		snprintf(buf, BUF_SIZE, "%ld", (long)tempqueue->jobId);
		strcat(reString,buf);
		strcat(reString," ");
		strcat(reString,tempqueue->job);
		strcat(reString,"\n");
		tempqueue = tempqueue->next;
	}
	return reString;
}

int ReturnFirstId(queuePtr head)
{
	queuePtr tempqueue = head;
	if (tempqueue!=NULL)
	{
		return tempqueue->jobId;
	}
	else
		return 0;
}
char *ReturnDeleteFirstItem(queuePtr *head)
{
	char *reString = (char*)malloc(sizeof(char)*MAXLEN);
	reString[0] = '\0';
	queuePtr tempqueue = *head;
	if (tempqueue != NULL)
	{
		strcat(reString,tempqueue->job);
		DeleteItem(head,tempqueue->jobId);
	}
	return reString;

}



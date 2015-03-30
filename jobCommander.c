/*
 * jobCommander.c
 *
 *  Created on: Jun 9, 2014
 *      Author: gioulisapo
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "queue.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include "MemSemHandle.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define MAX_ARGS 8
#define JOBCOMMANDER 0

int flag = 0;
int SemID;
void f();
void JobDone();
int Concurrency = 1;
const char *fifo = "mypipe";
const char * serverinfo = "./serverinfo";
queuePtr pendingQueue, runningQueue = NULL;

int main(int argc, char **argv)
{
	int fd, pd, i;
	char buf[BUF_SIZE];
	pid_t pid, childPid;
	/*-------------------------------Open pipe if it doesn't exist create it-------------------------------*/
	if ((pd = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
	{
		if (mkfifo(fifo, 0666) == -1)
		{
			if ( errno != EEXIST)
			{
				perror(" receiver : mkfifo ");
				exit(6);
			}
		}
		if ((pd = open(fifo, O_RDWR | O_NONBLOCK)) < 0)
		{
			perror("pipe problems");
			exit(6);
		}
	}
	/*-----------------------------------------------------------------------------------------------------*/
	/*-------------------------------If Semaphore set hasn't been created create them-------------------------------*/
	if ((SemID = GetSems()) < 0)
		SemID = CreateSems(); //Create sems for interprocess communication
	/*-----------------------------------------------------------------------------------------------------*/
	/*-------------------------------Open server file if it doesn't exist Server Process and file-------------------------*/
	if ((fd = open(serverinfo, O_RDONLY, 0644)) == -1)
	{
		if ((pid = fork()) < 0)                       //Create jobExecutorServer
		{
			perror("Error while creating jobExecutorServer: ");
			exit(1);
		}
		if ((fd = open(serverinfo, O_RDWR | O_CREAT, 0644)) == -1) //create serverinfo file containing pid
		{
			perror("Error while creating serverinfo file: ");
			exit(1);
		}
		//-------------------------------Job Executor Process---------------------------------------
		/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
		if (pid == 0)
		{
			signal(SIGCHLD, JobDone); //Install signal handler for when a process is finished
			signal(SIGCONT, f);	//Install signal handler for when jobCommander sends request
			char readbuf[MAXLEN];
			int pipe;
			int N = 1; //A custom ID for processes waitng to be executed
			snprintf(buf, BUF_SIZE, "%ld\n", (long) getpid()); //write pid in serverinfo.txt
			if (write(fd, buf, strlen(buf)) != strlen(buf))
				perror("Error while writing in serverinfo file: ");
			if ((pipe = open(fifo, O_RDWR| O_NONBLOCK) < 0)) //open connection to fifo mypipes
			{
				perror("pipe problems");
				exit(6);
			}
			fclose(stdout); //make sure that server can't print
			fclose(stderr); //make sure that server can't print
			for (;;) //Run until told not to
			{
				pause(); //wait for signal
				if (flag == 0) //make sure that signal comes from server and not from after completing JobDone
				{
					if (read(pd, readbuf, MAXLEN + 1) < 0) //read pipe to determine job issued by jobCommander
					{
						perror("problem in first reading ");
						exit(5);
					}
					char *tok = strtok(readbuf, " "); //now in tok I have one of the 5 available options
					//printf("\ntok: %s", tok);
					if (!(strcmp(tok, "setConcurrency")))
					{
						int ConcFlag = 0;//ConcFlag is 0 when concurrency was not increased
						tok = strtok(NULL, " ");
						if (atoi(tok) > Concurrency)//if Concurrency was increased raise ConcFlag
							ConcFlag = 1;
						Concurrency = atoi(tok);
						pid_t jobpid;
						char *argvs[MAX_ARGS];
						if (ConcFlag)					//
						{
							while (Concurrency > CountItems(runningQueue))//While running queue is not full
							{
								if (CountItems(pendingQueue) == 0)//If no jobs are waiting then we are done!
									break;
								int i = 0;//else get first item from pending queue
								char *tok = strtok(
										ReturnDeleteFirstItem(&pendingQueue), " ");//Note it will be deleted instantly from pending queue
								do//add command and parameters in *argvs[] array
								{
									argvs[i] = (char*) malloc(strlen(tok) + 1);
									strcpy(argvs[i], tok);
									i++;
								} while ((tok = strtok(NULL, " ")));
								argvs[i] = NULL;//first unused cell is NULL (for execvp)
								if ((jobpid = fork()) < 0) //fork exec
								{
									perror(
											"Error while creating jobExecutorServer: ");
									exit(1);
								}
								else if (jobpid == 0) //if Child vis a vis job I want to execute
								{
									fclose(stdout); //make sure process cant print
									fclose(stderr); //make sure process cant print
									if (execvp(argvs[0], argvs) < 0)
									{ /* execute the command  */
										printf("*** ERROR: exec failed\n");
										exit(1);
									}
									exit(1);
								}
								else
									//Server process
									runningQueue = AddItem(runningQueue, jobpid,
											argvs[0]); //add item to running queue
							}
						}
					}
					else if (!(strcmp(tok, "issuejob")))
					{
						if (CountItems(runningQueue) < Concurrency) //If I am permitted to execute
						{
							int jobpid, signal;
							char *argvs[MAX_ARGS];
							i = 0;
							while ((tok = strtok(NULL, " ")) != '\0')
							{
								argvs[i] = (char*) malloc(strlen(tok) + 1);
								strcpy(argvs[i], tok);
								i++;
							}
							argvs[i] = NULL;
							if ((jobpid = fork()) < 0) //Create job to be executed
							{
								perror(
										"Error while creating jobExecutorServer: ");
								exit(1);
							}
							else if (jobpid == 0)
							{
								fclose(stdout); //make sure process cant print
								fclose(stderr); //make sure process cant print
								if (execvp(argvs[0], argvs) < 0)
								{ /* execute the command  */
									printf("*** ERROR: exec failed\n");
									exit(1);
								}
								//pause(5);
								exit(1);
							}
							else
								runningQueue = AddItem(runningQueue, jobpid,
										argvs[0]); //add to currently running queue
						}
						else //If due to small Concurrency i can't run the command add it to pending queue to
						{	//be executed later
							char cmd[MAXLEN];
							cmd[0] = '\0';
							while ((tok = strtok(NULL, " ")) != '\0')
							{
								strcat(cmd, tok);
								strcat(cmd, " ");
							}
							cmd[strlen(cmd) - 1] = '\0';
							pendingQueue = AddItem(pendingQueue, N++, cmd);
							char* pending = ReturnQueue(pendingQueue);
						}
					}
					else if (!(strcmp(tok, "poll")))
					{
						tok = strtok(NULL, " ");
						char requested[8];//Since available options are either running or queued
						strcpy(requested, tok);
						char *results;//running or pending processes will be stored here in a printable format
						if (!strcmp(requested, "running"))
							results = ReturnQueue(runningQueue);
						if (!strcmp(requested, "queued"))
							results = ReturnQueue(pendingQueue);
						if ((write(pd, results, MAXLEN + 1)) == -1)	//write reults in fifp
						{
							perror(" Error in Writing ");
							exit(2);
						}
						up(SemID, JOBCOMMANDER);//notify jobCommander that the results are ready
					}
					else if (!(strcmp(tok, "stop")))//send -KILL signal to process to be terminated
					{
						int jobpid;
						tok = strtok(NULL, " ");
						kill(atoi(tok), SIGKILL);
						DeleteItem(&runningQueue, atoi(tok));//remove from running queue
					}
					else if (!(strcmp(tok, "exit")))
					{
						int garbage;
						free(pendingQueue);	//free pendingQueue
						while ((garbage = ReturnFirstId(runningQueue)) != 0)//Kill all processes running
						{
							printf("Gonna delete :%d\n", garbage);
							kill(garbage, SIGKILL);
							DeleteItem(&runningQueue, garbage);
						}
						RemoveSem(SemID);	//Delete sem set
						free(runningQueue);
						close(pd);	//close file descriptor
						unlink(fifo);	//delete fifo file
						unlink(serverinfo);	//delete serverinfo file
						exit(0);	//exit successfully
					}
				}
				else
					flag = 0;
			}
		}
		//---------------------------------------------------End Server!!!------------------------------------.........
		/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
		usleep(300); //If Server is not up wait for it to write pid in serverinfo file
	}
	int pollflag = 0; //flag rises if user wants poll
	char buffer[BUF_SIZE];
	read(fd, buffer, BUF_SIZE); //read Pid of Server running
	childPid = atoi(buffer);
	close(fd); //Close file
	/*-----------------------------------------------------------------------------------------------------*/
	if (argc == 1)
	{
		fprintf(stderr, "jobCommander: missing operand\n");
		fprintf(stderr, "./jobCommander [OPTION] [ARGUMENT]\n");
		fprintf(stderr, "\tissuejob [Job]:\tissue a job\n");
		fprintf(stderr, "\tsetConcurrency [int]:\tSet How many processes can run Concurrently\n");
		fprintf(stderr, "\tpoll [running | waitning] :\tshow running processes or processes waitnig to be executed\n");
		fprintf(stderr, "\tstop [Pid] :\tStop an issued Job\n");
		fprintf(stderr, "\texit :\tStop jobCommander and kill all running processes\n");
		exit(1);
	}
	if (argc == 0)
		exit(0);
	if (!(strcmp(argv[1], "issuejob")))
	{
		if (argc == 2)
		{
			fprintf(stderr, "jobCommander: job must be specified\n");
			exit(1);
		}
	}
	else if (!(strcmp(argv[1], "setConcurrency")))
	{
		if (argc == 2)
		{
			fprintf(stderr, "jobCommander: specify Concurrency\n");
			exit(1);
		}
		else if (argc > 3)
		{
			fprintf(stderr,
					"jobCommander: too many arguments in setConcurrency\n");
			exit(1);
		}
		else
		{
			int N;
			if ((N = atoi(argv[2])) < 1)
			{
				fprintf(stderr,
						"jobCommander: Concurrency selected not acceptable\n");
				exit(1);
			}
		}
	}
	else if (!(strcmp(argv[1], "stop")))
	{
		if (argc == 2)
		{
			fprintf(stderr, "jobCommander: specify process to be stopped\n");
			exit(1);
		}
		else if (argc > 3)
		{
			fprintf(stderr, "jobCommander: too many arguments in stop\n");
			exit(1);
		}
		else
		{
			long N;
			if ((N = atoi(argv[2])) < 1)
			{
				fprintf(stderr,
						"jobCommander: job ID selected not acceptable\n");
				exit(1);
			}
		}
	}
	else if (!(strcmp(argv[1], "poll")))
	{
		if (argc == 2)
		{
			fprintf(stderr,
					"jobCommander poll: acceptable options: running or queued\n");
			exit(1);
		}
		else if (argc > 3)
		{
			fprintf(stderr,
					"jobCommander poll: acceptable options: running OR queued\n");
			exit(1);
		}
		else
		{
			if (((strcmp(argv[2], "running")) != 0)
					&& ((strcmp(argv[2], "queued")) != 0))
			{
				fprintf(stderr,
						"jobCommander poll: acceptable options: running or queued\n");
				exit(1);
			}
			else
			{
				pollflag = 1; //pollflag risen to make sure we get feedback from server!
			}
		}
	}
	else if (!(strcmp(argv[1], "exit")))
	{
		if (argc > 2)
		{
			fprintf(stderr, "jobCommander exit: too many arguments\n");
			exit(1);
		}
	}
	else
	{
		fprintf(stderr, "jobCommander: unknown operand\n");
		exit(1);
	}
	char writebuf[MAXLEN]; //Create a string containing all the arguments passed, to write in pipe
	writebuf[0] = '\0';
	for (i = 1; i < argc - 1; i++)
	{
		strcat(writebuf, argv[i]);
		strcat(writebuf, " ");
	}
	strcat(writebuf, argv[argc - 1]);
	if (pollflag) //Make sure I get results
	{
		char results[MAXLEN];
		if ((write(pd, writebuf, MAXLEN + 1)) == -1) //notify Server for command
		{
			perror(" Error in Writing ");
			exit(2);
		}
		kill(childPid, SIGCONT); //send signal to Server that pipe has data
		down(SemID, JOBCOMMANDER); //wait until server has placed results wanted in pipe
		if (read(pd, results, MAXLEN + 1) < 0) //read from pipe
		{
			perror(" problem in reading ");
			exit(5);
		}
		printf("Processes %s in system\n%s\n", argv[2], results);
	}
	else //if any other option except poll
	{
		if ((write(pd, writebuf, MAXLEN + 1)) == -1) //write to pipe
		{
			perror(" Error in Writing ");
			exit(2);
		}
		kill(childPid, SIGCONT); //notify Server
	}
	return 0; //job Commander exits successfully
}
void f() //Due to problems f() is empty and Server process just gets the SIGCONT signal
{
}
void JobDone() //When a process is terminated!
{
	int status;
	pid_t pid;
	pid = waitpid(WAIT_ANY, &status, WNOHANG); //get status of finished processs
	DeleteItem(&runningQueue, pid); //delete finished job from running queue
	signal(SIGCHLD, JobDone);               //reinstall handler
	if (CountItems(pendingQueue) > 0 && CountItems(runningQueue) < Concurrency) //If there are items pending
	{		//and if I am have space to execute the with the current Concurrency
		/*Once again we do fork exec*/
		pid_t jobpid;
		char *argvs[MAX_ARGS];
		int i = 0;
		char *tok = strtok(ReturnDeleteFirstItem(&pendingQueue), " ");
		do
		{
			argvs[i] = (char*) malloc(strlen(tok) + 1);
			strcpy(argvs[i], tok);
			i++;
		} while ((tok = strtok(NULL, " ")));
		argvs[i] = NULL;
		if ((jobpid = fork()) < 0)
		{
			perror("Error while creating jobExecutorServer: ");
			exit(1);
		}
		else if (jobpid == 0)
		{
			fclose(stdout);
			fclose(stderr);
			if (execvp(argvs[0], argvs) < 0)
			{ /* execute the command  */
				printf("*** ERROR: exec failed\n");
				exit(1);
			}
			exit(1);
		}
		else
			runningQueue = AddItem(runningQueue, jobpid, argvs[0]);
	}
	flag = 1;//notify Server that if he gets a cont signal it is not valid and he should ignore it
}


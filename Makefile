#
# In order to execute this "Makefile" just type "make"
#	Apostolos Gioulis
#

OBJCOMMANDER 	= jobCommander.o queue.o MemSemHandle.o
SOURCE    	= jobCommander.c queue.c MemSemHandle.c
HEADER          = queue.h MemSemHandle.h
OUTCOMMANDER  	= jobCommander
CC	= gcc
FLAGS   = -g -c
# -g option enables debugging mode 
# -c flag generates object code for separate files

jobCommander: $(OBJCOMMANDER)
	$(CC) -g $(OBJCOMMANDER) -o $(OUTCOMMANDER)

jobCommander.o: jobCommander.c
	$(CC) $(FLAGS) jobCommander.c

queue.o: queue.c
	$(CC) $(FLAGS) queue.c

MemSemHandle.o: MemSemHandle.c
	$(CC) $(FLAGS) MemSemHandle.c

# clean house
clean:
	rm -f *.o mypipe serverinfo jobCommander
	sh rmsem.sh
	pkill -KILL jobCommander

# JobCommander
<h2>Description</h2>
A unix job executer interface written in C. Implementing a Server-Client architecture.<br>
When first called jobCommander Creates a jobExecutorServer. Job Commander then communicates with the Server through a named pipe using signals and semaphores. The Server executes jobs using fork-exec and Stores the jobs waiting to be executed in a FIFO. JobCommander can ask the server for the running|queued processes. 
<h2>Usage</h2>
<code>./jobCommander [OPTION] [ARGUMENT]</code><br>
<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;issuejob [Job]:	issue a job</code></code><br>
<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;setConcurrency [int]:	Set How many processes can run Concurrently</code><br>
<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;poll [running | waiting]: Show running processes or processes waiting to be executed</code><br>
<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;stop [Pid]:	Stop an issued Job</code><br>
<code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;exit:	Stop jobCommander and kill all running processes</code><br>
<h2>Files</h2>
<ul>
<li><strong>allJobsStop.sh</strong><p>Bash script that recieves all processes running by jobCommander and then using JobCommader again it kills them</p></li>
<li><strong>multijob.sh</strong><p>Bash script that issues JobCommander to execute every job written in a file passed as an argument (Commands.txt)</p></li>
<li><strong>MemSemHandle.h/MemSemHandle.c</strong><p>All functions necessary by semaphores</p></li>
<li><strong>queue.c/queue.h</strong><p>Implemetation of fifo queue</p></li>
<li><strong>jobCommander.c</strong><p>Job Commander implementation</p></li>
<li><strong>rmsem.sh</strong><p>Community bash script, deleting all semaphores in case of a unhealthy program termination</p></li>
</ul>

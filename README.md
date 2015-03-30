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

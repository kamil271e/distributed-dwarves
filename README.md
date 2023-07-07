# distributed-dwarves
Assignment for Distributed Processing at PUT. The objective is to develop a solution that allows for concurrent access to a critical section by multiple processes using the Open MPI framework.

### Algorithm (dwarves).

- dwarf waits in a loop for a job
- tavern generates a job
- tavern sends the job to all dwarves
- each dwarf receives the job and sends REQ to other dwarves
- the dwarves receive the REQ regarding the job from their brothers:
    - if the dwarf does not apply for the same job, it sends ACK
    - if a dwarf applies for the job, but has a lower priority he sends ACK and abandons the task
    - if a dwarf applies for the job and has a higher priority then he does not send an ACK
- dwarf who has received ACK from all brothers moves on to occupy the portal
- the dwarf who is 2nd in order decides the order of the next jobs (according to the order in which they come to him) and sends information about them to the dwarves according to the order of REQs, which they sent to each other earlier
- this dwarf divides into 2 threads - 1. responds ACK to all REQs about other orders; 2. moves on
- The dwarves receive REQs regarding portals from their brothers:
    - if the dwarf does not apply for a portal it sends an ACK
    - if the dwarf applies for a portal but has a lower priority then he sends an ACK
    - if the dwarf applies for a portal and has a higher priority then it adds the process that sent the REQ to the list (it will send him an ACK after leaving the critical section)
- a dwarf who received an ACK from (D-1-P) brothers goes on (D is the nember of dwarves, P is the number of portals)
- dwarf who has a job and portal performs the job
- the dwarf who has completed the job releases the portal (exits the critical section) and sends ACKs to the dwarves that have accumulated on his list
- the dwarf goes back to waiting for the job

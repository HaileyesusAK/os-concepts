#ifndef __PID_MANAGER_H__
#define __PID_MANAGER_H__

/*
	Creates and initializes a data structure for representing pids;
	returns −1 if unsuccessful, 1 if successful.
*/
int allocate_map(void);

/*
	Allocates and returns a pid; returns −1 if unable to allocate
	a pid (all pids are in use).
*/
int allocate_pid(void);

/* Releases a pid */
void release_pid(int pid); 

#endif

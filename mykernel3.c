/* mykernel3.c: your portion of the kernel
 *
 *   	Below are procedures that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these procedures.  You may
 *  	modify the bodies of these procedures any way you wish (however,
 *  	you cannot change the interfaces).  
 */

#include "aux.h"
#include "sys.h"
#include "mykernel3.h"

#define FALSE 0
#define TRUE 1

/*
 *
 */

typedef struct {
	int valid;              // is this entry valid: 1 = yes, 0 = no
        int pid;                // process ID (as provided by kernel)
} proc;

/* 	A sample semaphore table. You may change this any way you wish.  
 */
static struct {
	int valid;			// Is this a valid entry (was sem allocated)?
	int value;			// value of semaphore
	proc blockedProcs[MAXPROCS];    // Queue of blocked processes for each semaphore
	int head;			// Head and Tail of the queue
	int tail;
	int isFull;
} semtab[MAXSEMS];


/*
 * Checks if the queue for the specific semaphore is empty
 */
int isEmpty(int s) {
	return (semtab[s].head == semtab[s].tail) && !semtab[s].isFull;
}


/*
 * Adds process to the blocked queue of given semaphore
 */
void addProc(int p, int s) {

	/*if(((semtab[s].tail+1) % MAXPROCS) == semtab[s].head)  {
		return;
	}*/
	if(semtab[s].isFull) return;

	proc tmp;
	tmp.valid = TRUE;
	tmp.pid = p;
	semtab[s].blockedProcs[semtab[s].tail] = tmp;

	semtab[s].tail++; 
	semtab[s].tail = semtab[s].tail % MAXPROCS;

	if(semtab[s].tail == semtab[s].head) semtab[s].isFull = TRUE;
}


/*
 * Removes the next process in blocked queue of this sempahore if any
 */
int removeProc(int s) {
	
	if(isEmpty(s)) {
		return -1;
	}
	
	int head = semtab[s].head;
	
	if(!semtab[s].blockedProcs[head].valid) return -1;
	
	semtab[s].blockedProcs[head].valid = FALSE;
	proc tmp = semtab[s].blockedProcs[head++];

	semtab[s].head = head % MAXPROCS;
	semtab[s].isFull = FALSE;

	return tmp.pid;
}



/*  	InitSem () is called when kernel starts up. Initialize data
 * 	structures (such as the semaphore table) and call any initialization
 *   	procedures here. 
 */

void InitSem ()
{
	int s;
	int i;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {		// mark all sems free
		semtab[s].valid = FALSE;
		semtab[s].isFull = FALSE;
		semtab[s].value = 0;
		semtab[s].head = 0;
		semtab[s].tail = 0;
		
		for(int i=0; i < MAXPROCS; i++) {
			semtab[s].blockedProcs[i].valid = FALSE;
			semtab[s].blockedProcs[i].pid = -1;
		}	
	}
}

/* 	MySeminit (p, v) is called by the kernel whenever the system
 *  	call Seminit (v) is called.  The kernel passes the initial
 *  	value v, along with the process ID p of the process that called
 * 	Seminit.  MySeminit should allocate a semaphore (find a free entry
 *  	in semtab and allocate), initialize that semaphore's value to v,
 * 	and then return the ID (i.e., index of the allocated entry). 
 */

int MySeminit (int p, int v)
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {
		if (semtab[s].valid == FALSE) {
			break;
		}
	}
	if (s == MAXSEMS) {
		DPrintf ("No free semaphores\n");
		return (-1);
	}

	semtab[s].valid = TRUE;
	semtab[s].value = v;

	return (s);
}

/*   	MyWait (p, s) is called by the kernel whenever the system call
 * 	Wait (s) is called.  
 */

void MyWait (p, s)
	int p;				// process
	int s;				// semaphore
{
	/* modify or add code any way you wish */
	
	semtab[s].value--;
	if(semtab[s].value < 0) {
		addProc(p, s);
		Block(p);
	}
}

/*  	MySignal (p, s) is called by the kernel whenever the system call
 *  	Signal (s) is called. 
 */

void MySignal (p, s)
	int p;				// process
	int s;				// semaphore
{
	/* modify or add code any way you wish */

	semtab[s].value++;
	if(!isEmpty(s)) {
		int tmpProc = removeProc(s);
		Unblock(tmpProc);	
	}
}


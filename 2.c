#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"

#define BUFFER_SIZE 60

// Message box
typedef struct _Message{
	char msg[41];
	unsigned int from;
	unsigned int to;
	/* 0 - Sent message
	 * 1 - Replied message
	*/
	int type;
}Message;
// STATE
typedef enum{
	RUNNING,
	READY,
	BLOCKED
}STATE;
// PCB structure
typedef struct _PCB{
	unsigned int pid;
	int priority;
	STATE state ;
	// store a message
	Message *msg;
	// check whether this process has sent any message
	bool sent;
	// check time(quantum) passed
	int timePassed;
}PCB;
// PCB's pid
unsigned int PID = 0;
// Initialization of PCB
PCB *initPCB(int priority){
	PCB *ret = (PCB*)malloc(sizeof(PCB));
	if (ret != NULL){
		ret->pid = PID;
		ret->priority = priority;
		ret->state = READY;
		ret->msg = NULL;
		ret->sent = false;
		ret->timePassed = 0;
		PID++;
	}
	return ret;
}

// Message queue
List *msg_queue;

/* Queues
 * [0-2]  --- readyQ
 * [3]    --- sendQ
 * [4]    --- recvQ
 * [5-9] --- semQ
 */
List *QUEUES[10];

// Semaphore structure
typedef struct _sem{
	int value;
	List *plist;

}SEM;
SEM *sem[5] = {NULL};
// SEM initialize
SEM *initSEM(int pid){
	SEM *ret = (SEM*)malloc(sizeof(SEM));
	if (ret != NULL){
		ret->value = 1;
		ret->plist = QUEUES[pid+5];
	}
	return ret;
}

// running/init processes
PCB *running, *init;

// Functions
int pidSearch(void *, void *);
int pidSearchforM(void *, void*);
void *processSearch(List *, int);
void *messageSearch(List *, int);
void printProcInfo(PCB *);
PCB *nextProcess();
void printMessage();
void freePCB(void*);

int main(){
	// Initialize lists
	for (int i = 0; i < 10; i++){
		QUEUES[i] = ListCreate();
		if (QUEUES[i] == NULL){
			printf("Initialization of a list failed...\n");
			return -1;
		}
	}
	msg_queue = ListCreate();
	if (msg_queue == NULL){
		printf("Initialization of a list failed...\n");
		return -1;
	}
	// Initialize the init process
	init = initPCB(-1);
	if (init == NULL){
		printf("PCB initialization failed...\n");
		return -1;
	}
	// the init process is now running
	running = init;
	running->state = RUNNING;
	// Say Hello
	printf("CMPT 300 Assignment 3\nName: Jun Goo Kim\nStudentID: 301046893\n");

	// Start the simulation
	char command, msg[41], buffer[BUFFER_SIZE];
	int num, i, s;
	unsigned int parameter;
	
	while(1){
		// Take inputs from a user
		printf("Enter a command: ");
		fgets(buffer, BUFFER_SIZE, stdin);
		num = sscanf(buffer, "%c %d %40[^\n]s", &command, &parameter, msg);
		// Create a new process
		if (command == 'C' || command == 'c'){
			if (num != 2){
				printf("Failed --> Correct format is: 'C number'\n");
				continue;
			}
			if (parameter < 0 || parameter > 2){
				printf("Failed --> Priority number must be between 0 and 2!\n");
				continue;
			}
			PCB *newProcess = initPCB(parameter);
			if (newProcess == NULL){
				printf("NULL returned --> Creating a new process has failed\n");
			}
			else{
				s = ListPrepend(QUEUES[parameter], newProcess);
				if (s == -1){
					printf("ListPrepend failed\n");
					continue;
				}
				printf("A new process has been created successfully.\n");
				printf("PID: %d\n", (PID-1));
			}
		}
		// Fork
		else if (command == 'F' || command == 'f'){
			if (num != 1){
				printf("Failed --> Correct format is: F\n");
				continue;
			}
			// Check the running process
			if (running == init){
				printf("Failed --> Attempting to Fork the init process!!!\n");
			}
			else{
				PCB *newProcess = initPCB(running->priority);
				if (newProcess == NULL){
					printf("Creating a new process has failed\n");
				}
				else{
					s = ListPrepend(QUEUES[running->priority], newProcess);
					if (s == -1){
						printf("ListPrepend failed\n");
						continue;
					}
					printf("Fork completed\n");
					printf("PID: %d\n", (PID-1));
				}
			}
		}
		// Kill
		else if (command == 'K' || command == 'k'){
			if (num != 2){
				printf("Failed --> Correct format is: K number\n");
				continue;
			}
			if (parameter == 0){
				printf("Failed --> Trying to kill the init process\n");
				continue;
			}
			else{
				bool found = false;
				// if the input pid is the pid of the running process, kill the running process
				if (running->pid == parameter){
					printf("The process is currently running...\n");
					printf("Killing the currently running process...\n");
					// kill the process
					free(running);
					running = nextProcess();
					printf("The pid of the process that gets control of the CPU is %d\n", running->pid);
					printMessage();
					found = true;
				}
				else{
					PCB *item = NULL;
					for (int i = 0; i < 10; i++){
						// Start search
						if (ListCount(QUEUES[i]) != 0){
							item = (PCB *) processSearch(QUEUES[i], parameter);
							// Successfully found the pid
							if (item != NULL){
								item = (PCB *) ListRemove(QUEUES[i]);
								// kill the process
								free(item->msg);
								free(item);
								found = true;
							}
						}
						if (found){
							break;
						}
					}
					if (found){
						printf("Successfully killed the process\n");
					}
					else{
						printf("Failed --> No such the pid was found\n");
					}
				}
			}
		}
		// Exit
		else if (command == 'E' || command == 'e'){
			if (num != 1){
				printf("Failed --> Correct command is: E\n");
				continue;
			}
			if (running == init){
				bool processExist = false;
				printf("The running process is the init process\n");
				printf("Searching for any remaining process in queues.....\n");
				for (int i = 0; i < 10; i++){
					if (ListCount(QUEUES[i]) != 0){
						printf("Failed --> At least one process still exists\n");
						processExist = true;
						break;
					}
				}
				if (!processExist){
					// free init process and terminate the program
					printf("There is only init process.... Terminating the program\n");
					free(init);
					break;
				}
			}
			else{
				// kill the process
				printf("The proccess of pid = %d has been killed\n", running->pid);
				free(running);
				running = nextProcess();
				printf("The pid of the process that now gets control of the CPU is %d\n", running->pid);
				printMessage();				
			}
		}
		// Quantum
		else if (command == 'Q' || command == 'q'){
			if (num != 1){
				printf("Failed --> Correct command is: Q\n");
				continue;
			}
			if (running != init){
				s = ListPrepend(QUEUES[running->priority], running);
				if (s == -1){
					printf("ListPrepend failed...\n");
				}
			}
			running->state = READY;
			running = nextProcess();
			printf("The pid of the process that now gets control of the CPU is %d\n", running->pid);
			printMessage();	

		}
		// Send
		else if (command == 'S' || command == 's'){
			if (num != 3){
				printf("The correct command is: S pid msg\n");
				continue;
			}
			msg[40] = '\0';
			running->sent = true;
			if (running != init){
				// Put the process in the sendQ if it's not init
				running->state = BLOCKED;
				s = ListPrepend(QUEUES[3], (void *)running);
				if (s == -1){
					printf("ListPrpend failed....\n");
				}
			}
			// Create a message
			Message *A = (Message *) malloc(sizeof(Message));
			strcpy(A->msg, msg);
			A->from = running->pid;
			A->to = parameter;
			A->type = 0;
			if (parameter == 0){
				init->msg = A;
				printf("The message was sent to the init process\n");
			}
			else if (parameter == running->pid){
				running->msg = A;
				printf("The message was sent to itself\n");
			}
			else{
				// check whether the named process is in the recvQ
				PCB *pcb = (PCB *) processSearch(QUEUES[4], parameter);
				if (pcb != NULL){
					// remove it from the recvQ
					pcb = ListRemove(QUEUES[4]);
					pcb->msg = A;
					pcb->state = READY;
					// put it back to a readyQ
					s = ListPrepend(QUEUES[pcb->priority], pcb);
					if (s == -1){
						printf("ListPrepend failed...\n");
					}
				}
				else{
					s = ListPrepend(msg_queue, (void *) A);
					if (s == -1){
					printf("ListPrepend failed...\n");
					}
					printf("The message has been put in the message queue\n");
				}
			}
			// Next process
			running = nextProcess();
			printf("The pid of the process that now gets control of the CPU is %d\n", running->pid);
			printMessage();	
		}
		// Receive
		else if (command == 'R' || command == 'r'){
			if (num != 1){
				printf("Correct command is: R\n");
				continue;
			}
			// check the message box
			Message *A = (Message *) messageSearch(msg_queue, running->pid);
			if (A != NULL){
				A = ListRemove(msg_queue);
				printf("A message from %d has been received: %s\n", A->from, A->msg);
				free(A);
			}
			else{
				running->state = BLOCKED;
				// Put it in the recvQ
				s = ListPrepend(QUEUES[4], (void *)running);
				if (s == -1){
					printf("ListPrpend failed....\n");
				}
				printf("The process has been queued in the recvQ...\n");
				running = nextProcess();
				printf("The pid of the process that now gets control of the CPU is %d\n", running->pid);
				printMessage();					
			}
		}
		// Reply
		else if (command == 'Y' || command == 'y'){
			if (num != 3){
				printf("Correct command is: Y id msg\n");
				continue;
			}
			// Create a message
			Message *A = (Message *) malloc(sizeof(Message));
			strcpy(A->msg, msg);
			A->from = running->pid;
			A->to = parameter;
			A->type = 1;
			if (parameter == 0){
				init->msg = A;
				printf("The reply message was delievered to the init process\n");
			}
			else if (parameter == running->pid){
				running->msg = A;
				printf("The reply message was delievered to itself\n");
			}
			else{
				// check whether the named process is in the sendQ
				PCB *pcb = (PCB *) processSearch(QUEUES[3], parameter);
				if (pcb != NULL){
					// remove it from the recvQ
					pcb = ListRemove(QUEUES[3]);
					pcb->msg = A;
					pcb->state = READY;
					// put it back to a readyQ
					s = ListPrepend(QUEUES[pcb->priority], pcb);
					if (s == -1){
						printf("ListPrepend failed...\n");
					}
					printf("The reply message was delivered to %d\n", parameter);
				}
				else{
					printf("Failed --> The process is not in the sending queue\n");
				}
			}						
		}
		// New semaphore
		else if (command == 'N' || command == 'n'){
			if (num != 2){
				printf("The correct command is: N id\n");
				continue;
			}
			if (parameter < 0 || parameter > 4){
				printf("The pid must be between 0 and 4\n");
				continue;
			}
			if (sem[parameter] != NULL){
				printf("Failed --> There is already the semaphore of id %d\n", parameter);
			}
			else{
				sem[parameter] = initSEM(parameter);
				if (sem[parameter] == NULL){
					printf("Failed --> initialization failed...\n");
				}
				else{
					printf("A semaphore has been successfully created!\n");
				}
			}

		}
		// P(S)
		else if (command == 'P' || command == 'p'){
			if (num != 2){
				printf("The correct command is: P id\n");
				continue;
			}
			if (parameter < 0 || parameter > 4){
				printf("The pid must be between 0 and 4\n");
				continue;
			}
			if (sem[parameter] == NULL){
				printf("Failed --> There is no semaphore of id %d\n", parameter);
			}
			else{
				sem[parameter]->value--;
				if (sem[parameter]->value < 0){
					if (running == init){
						printf("S.value is %d. The running process is the init process... NOT BLOCKED\n", sem[parameter]->value);
					}
					else{
						printf("S.value is %d. The process was blocked\n", sem[parameter]->value);
						running->state = BLOCKED;
					}
					PCB *temp = nextProcess();
					if (running == init && temp == init){
						printf("Only the init process is available...\n");
						printf("No need to put it in a plist. V is performed automatically\n");
						sem[parameter]->value++;
						printf("S.value is %d\n", sem[parameter]->value);

					}
					else{
						s = ListPrepend(sem[parameter]->plist, (void *) running);
						if (s == -1){
							printf("ListPrepend failed...\n");
						}
					}
					running = temp;
					printf("The pid of the process that now gets control of the CPU is %d\n", running->pid);
				}
				else{
					printf("S.value is %d! Successfully performed\n",sem[parameter]->value);
				}
			}

		}
		// V(S)
		else if (command == 'V' || command == 'v'){
			if (num != 2){
				printf("The correct command is: V id\n");
				continue;
			}
			if (parameter < 0 || parameter > 4){
				printf("The pid must be between 0 and 4\n");
				continue;
			}
			if (sem[parameter] == NULL){
				printf("Failed --> The semaphore with id %d has not been created yet\n", parameter);
			}
			else{
				sem[parameter]->value++;
				if (sem[parameter]->value <= 0){
					PCB *temp = (PCB *) ListTrim(sem[parameter]->plist);
					temp->state = READY;
					if (temp != init){
						s = ListPrepend(QUEUES[temp->priority], (void*) temp);
						if (s == -1){
							printf("ListPrepend failed..\n");
						}
					}
					printf("S.value is %d. The process of id %d has been take out from the plist and put in a readyQ.\n", sem[parameter]->value, temp->pid);
				}
				else{
					printf("S.value is %d! Successfully performed\n",sem[parameter]->value);
				}
			}

		}
		// ProcInfo
		else if (command == 'I' || command == 'i'){
			if (num != 2){
				printf("The correct command is: I id\n");
				continue;
			}
			if (parameter < 0){
				printf("The pid must be larger than 0\n");
				continue;
			}
			PCB *item;
			bool found = false;
			if (parameter == 0){
				printf("This is the init process\n");
				item = init;
				found = true;
			}
			else if (parameter == running->pid){
				printf("This is the running process\n");
				item = running;
				found = true;
			}
			else{
				for (i = 0; i < 10; i++){
					// Start search
					if (ListCount(QUEUES[i]) != 0){
						item = (PCB *) processSearch(QUEUES[i], parameter);
						// Successfully found the pid
						if (item != NULL){
							found = true;
							break;
						}
					}
				}
			}
			if(!found){
				printf("Failed --> There is no such the pid in the queues...\n");
			}
			else{
				printProcInfo(item);
			}
		}
		// TotalInfo
		else if (command == 'T' || command == 't'){
			if (num != 1){
				printf("The correct command is: T\n");
				continue;
			}
			// print the init process
			printf("INIT PROCESS@@@\n");
			printProcInfo(init);
			// print the running process
			printf("RUNNING PROCESS@@@\n");
			printProcInfo(running);
			// print processes in queues
			PCB *temp;
			for (i = 0; i < 10; i++){
				// Start search
				if (ListCount(QUEUES[i]) != 0){
					temp = (PCB*) ListFirst(QUEUES[i]);
					printProcInfo(temp);
					for (int j = 1; j < ListCount(QUEUES[i]); j++){
						temp = ListNext(QUEUES[i]);
						printProcInfo(temp);
					}
				}
			}
		}
		else{
			printf("Invalid Command!\n");
		}
	}
	freeFunc itemFree = freePCB;
	ListFree(msg_queue, itemFree);
	printf("Simulation Done!!!!!\n");
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void freePCB(void * item){
	PCB *temp = (PCB *)item;
	free(temp->msg);
	free(temp);
}
int pidSearch(void *item, void *comparisonArg){
	PCB *temp = (PCB *) item;
	if (temp->pid == *(int *)comparisonArg){
		return 1;
	}
	else{
		return 0;
	}
}
int pidSearchforM(void *item, void *comparisonArg){
	Message *temp = (Message *) item;
	if (temp->to == *(int *)comparisonArg){
		return 1;
	}
	else{
		return 0;
	}	
}
void *processSearch(List *queue, int pid){
	compFunc comparator = pidSearch;
	// Make the current node head, so ListSearch can search the list from the head
	int *pass = &pid;
	ListFirst(queue);
	return ListSearch(queue, comparator, (void *) pass);
}
void *messageSearch(List *queue, int pid){
	compFunc comparator = pidSearchforM;
	// Make the current node head, so ListSearch can search the list from the head
	int *pass = &pid;
	ListFirst(queue);
	return ListSearch(queue, comparator, (void *) pass);
}
void printProcInfo(PCB *proc){
	printf("#################\n");
	printf("PROCESS PID: %d\n", proc->pid);
	printf("PRIORITY: %d\n", proc->priority);
	switch(proc->state){
	case RUNNING: printf("STATE: RUNNING\n"); break;
	case READY: printf("STATE: READY\n"); break;
	case BLOCKED: printf("STATE: BLOCKED\n"); break;
	default: printf("WHAT???\n");
	}
	if (proc->msg != NULL){
		printf("MSG: %s\n", proc->msg->msg);
	}
	else{
		printf("MSG: NULL\n");
	}
	if (proc->sent){
		printf("SENT: YES\n");
	}
	else{
		printf("SENT: NO\n");
	}
	printf("#################\n");
}

PCB *nextProcess(){
	PCB *ret = NULL;
	int i, s;
	for (i = 0; i < 3; i++){
		if (ListCount(QUEUES[i]) != 0){
			ret = (PCB *) ListTrim(QUEUES[i]);
			ret->timePassed = 0;
			break;
		}
	}
	// No process is in readyQs. The next process is the init process
	if (ret == NULL){
		ret = init;
	}
	ret->state = RUNNING;
	// Scheduling algorithm to avoid the starvation of lower prority processes
	// Update timePassed values of the first process in med/low priorities' queues
	// so when they reach 10, their priority values decrease by 1
	PCB *temp;
	for (i = 1; i <3; i++){
		if (ListCount(QUEUES[i]) != 0){
			temp = (PCB *) ListLast(QUEUES[i]);
			temp->timePassed++;
			if (temp->timePassed == 10){
				temp = (PCB *) ListTrim(QUEUES[i]);
				temp->timePassed = 0;
				temp->priority--;
				s = ListPrepend(QUEUES[temp->priority], (void *) temp);
				if (s == -1){
					printf("ListPrepend failed....\n");
				}
			}
		}
	} 

	return ret;
}

void printMessage(){
	if (running->msg != NULL){
		if (running->sent == true && running->msg->type == 1){
			printf("This process has a message!\n");
			printf("The replied message from %d: %s\n", running->msg->from, running->msg->msg);
			free(running->msg);
			running->sent = false;
		}
		else if (running->msg->type == 0){
			printf("This process has a message!\n");
			printf("The sent message from %d: %s\n", running->msg->from, running->msg->msg);
			free(running->msg);
		}
		running->msg = NULL;		
	}
}
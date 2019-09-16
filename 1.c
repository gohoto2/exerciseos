#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "list.h"

#define BUFFER_SIZE 128

// Threads
pthread_t writeMsg_thread, readMsg_thread, sendMsg_thread, recvMsg_thread;
// All mutexes and condition variables are initialized statically. NO need to destroy
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

// Lists
List *ReadList;
List *WriteList;

// Functions
void *readMsg(void*);
void *writeMsg(void*);
void *sendMsg(void*);
void *recvMsg(void*);
// For ListFree
void HeapFree(void *);
// Sokets
struct sockaddr_in Myaddr, claddr;
int sfd;

// Main Function
int main(int argc, char *argv[]){

	int s;
	ReadList = ListCreate();
	WriteList = ListCreate();
    struct hostent *h;

    // Create a socket
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1){
        perror("socket Error: ");
        exit(1);
    }
    Myaddr.sin_family = AF_INET;
    Myaddr.sin_addr.s_addr = INADDR_ANY;
    Myaddr.sin_port = htons(atoi(argv[1]));
    memset(&Myaddr.sin_zero, 0, 8);

    // Bind 
    if (bind(sfd, (struct sockaddr *)&Myaddr, sizeof(struct sockaddr_in)) == -1){
        perror("bind Error: ");
        exit(1);
    }

    // Connect to the other hostname
    claddr.sin_family = AF_INET;
    claddr.sin_port = htons(atoi(argv[3]));
    memset(&claddr.sin_zero, 0, 8);
    h = gethostbyname(argv[2]);
    if (h == NULL){
        perror("gethostbyname error: ");
        close(sfd);
        exit(1);
    }
    memcpy(&claddr.sin_addr, h->h_addr_list[0], h->h_length);
	// Creating threads
	s = pthread_create(&readMsg_thread, NULL, readMsg, NULL);	
	if (s != 0){
        perror("pthread_create error: ");		
        exit(1);
	}
	s = pthread_create(&writeMsg_thread, NULL, writeMsg, NULL);
	if (s != 0){
		perror("pthread_create error: ");	
        exit(1);
	}
	s = pthread_create(&sendMsg_thread, NULL, sendMsg, NULL);	
	if (s != 0){
		perror("pthread_create error: ");	
        exit(1);
	}
	s = pthread_create(&recvMsg_thread, NULL, recvMsg, NULL);
	if (s != 0){
		perror("pthread_create error: ");	
        exit(1);
	}
    // free the lists in case there is anything left on them 
    freeFunc itemFree = HeapFree;
    ListFree(WriteList, itemFree);
    ListFree(ReadList, itemFree);
    // join threads
	s = pthread_join(writeMsg_thread, NULL);
	if (s != 0){
		perror("pthread_join error: ");	
        exit(1);
	}
	s = pthread_join(readMsg_thread, NULL);
	if (s != 0){
		perror("pthread_join error: ");	
        exit(1);
	}
	s = pthread_join(sendMsg_thread, NULL);
	if (s != 0){
		perror("pthread_join error: ");	
        exit(1);
	}
	s = pthread_join(recvMsg_thread, NULL);
	if (s != 0){
		perror("pthread_join error: ");	
        exit(1);
	}
	printf("Done\n");

	return 0;
}

// Read message from stdin
void *readMsg(void *arg){
	int s;
	char lastMsg = 'A';
	while(true){
		char *buffer = (char*) malloc(BUFFER_SIZE*sizeof(char));	
		int numRead = read(STDIN_FILENO, buffer, BUFFER_SIZE);
		if (numRead == -1){
			perror("read() error: ");
			break;		
		}
		buffer[numRead] = '\0';
		s = pthread_mutex_lock(&mtx);
		if (s != 0){
			perror("pthread_mutex_lock error: ");
			break;	
		}
		s = ListAppend(WriteList, buffer);
		if (s == -1){
			printf("ListAppend() failed...\n");
			// Sleep for 3 sec and let the other thread consume list
			s = pthread_mutex_unlock(&mtx);
			if (s != 0){
				perror("pthread_mutex_unlock error: ");
				break;	
			}
			s = pthread_cond_signal(&cond);
			if (s != 0){
				perror("pthread_cond_signal error: ");
				break;	
			}
			sleep(3);
			s = pthread_mutex_lock(&mtx);
			if (s != 0){
				perror("pthread_mutex_lock error: ");
				break;	
			}
			// Append it again
			s = ListAppend(WriteList, buffer);
			// if it fails again, terminate program
			if (s == -1){
				lastMsg = '!';
			}						
		}
		// save the cancel request
		if (buffer[0] == '!' && numRead == 2){
			lastMsg = '!';	
		}			
		s = pthread_mutex_unlock(&mtx);
		if (s != 0){
			perror("pthread_mutex_unlock error: ");
			break;	
		}
		s = pthread_cond_signal(&cond);
		if (s != 0){
			perror("pthread_cond_signal error: ");
			break;	
		}
		if (lastMsg == '!'){
			break;
		}		
	}
	// sleep the thread for a bit to let any remaining messages in the list(s) be processed 
	sleep(1);
	// cancel the sending thread 
	pthread_cancel(sendMsg_thread);
	// send a last message containing the ! character to the receiver
	if (lastMsg == '!'){
	    if (sendto(sfd, &lastMsg, 1, 0, (struct sockaddr *)&claddr, sizeof(struct sockaddr_in)) == -1){
	        perror("send error: ");
	    }			
	} 
    // close the socket 
    close(sfd);
    // cancel the receiver and screen thread 
    pthread_cancel(recvMsg_thread);
    pthread_cancel(writeMsg_thread);
	pthread_exit(NULL);
}

// Write messages to screen 
void *writeMsg(void *arg){
	int s;
	while(true){
		s = pthread_mutex_lock(&mtx2);
		if (s != 0){
			perror("pthread_mutex_lock error: ");
		    // close socket
		    close(sfd);	
			pthread_cancel(sendMsg_thread);
		    pthread_cancel(recvMsg_thread);
		    pthread_cancel(readMsg_thread);		    
		    pthread_exit(NULL);
		}

		while (ListCount(ReadList) == 0){
			s = pthread_cond_wait(&cond2, &mtx2);
			if (s != 0){
				perror("pthread_cond_wait error: ");
			    // close socket
			    close(sfd);	
				pthread_cancel(sendMsg_thread);
			    pthread_cancel(recvMsg_thread);
			    pthread_cancel(readMsg_thread);			    
			    pthread_exit(NULL);
			}
		}
		// consume everything
		while (ListCount(ReadList) > 0){
			char *buffer = (char *) ListRemove(ReadList);
			if(buffer == NULL){
				printf("ListRemove error\n");
				break;
			}
			int numWrite = write(STDOUT_FILENO, buffer, strlen(buffer));
			free(buffer);			
			if (numWrite == -1){
				perror("write() error: ");
				break;
			}
		}
		s = pthread_mutex_unlock(&mtx2);
		if (s != 0){
			perror("pthread_mutex_unlock error: ");
		    // close socket
		    close(sfd);	
			pthread_cancel(sendMsg_thread);
		    pthread_cancel(recvMsg_thread);
		    pthread_cancel(readMsg_thread);			    
		    pthread_exit(NULL);
		}
	}
}

// send messages
void *sendMsg(void *arg){
	char *buf;
	int s;
    while(true){
    	s = pthread_mutex_lock(&mtx);
    	if (s != 0){
			perror("pthread_mutex_lock error: ");
		    // close socket
		    close(sfd);	
			pthread_cancel(writeMsg_thread);
		    pthread_cancel(recvMsg_thread);
		    pthread_cancel(readMsg_thread);			    	
		    pthread_exit(NULL);
		}
		// wait for something to consume
		while (ListCount(WriteList) == 0){
			s = pthread_cond_wait(&cond, &mtx);
			if (s != 0){
				perror("pthread_cond_wait error: ");
			    // close socket
			    close(sfd);	
				pthread_cancel(writeMsg_thread);
			    pthread_cancel(recvMsg_thread);
			    pthread_cancel(readMsg_thread);		    	
			    pthread_exit(NULL);
			}
		}
		while (ListCount(WriteList) > 0){
			buf = (char *) ListRemove(WriteList);
			if(buf == NULL){
	            printf("ListRemove error\n");
				break;		
			}
	        if (sendto(sfd, buf, strlen(buf), 0, (struct sockaddr *)&claddr, sizeof(struct sockaddr_in)) == -1){
	            perror("send error: ");
	            free(buf);
	            break;
	        }
	        free(buf);
    	}
    	s = pthread_mutex_unlock(&mtx);
		if (s != 0){
			perror("pthread_mutex_unlock error: ");
		    // close socket
		    close(sfd);	
			pthread_cancel(writeMsg_thread);
		    pthread_cancel(recvMsg_thread);
		    pthread_cancel(readMsg_thread);			    
		    pthread_exit(NULL);
		}
    }
}

void *recvMsg(void *arg){
    ssize_t numBytes;
    int s;
    int lastMsg = 0;
    while(true){
    	char *buf = (char*) malloc(BUFFER_SIZE*sizeof(char));	   	
        numBytes = recvfrom(sfd, buf, BUFFER_SIZE, 0, NULL, NULL);
        buf[numBytes] = '\0';
        if(numBytes == -1){
            perror("recvfrom Error: ");
            break;
        }
    	s = pthread_mutex_lock(&mtx2);
		if (s != 0){
			perror("pthread_mutex_lock error: ");
		    break;
		}

		s = ListAppend(ReadList, buf);
		if (s == -1){
			printf("ListAppend() failed...\n");
	       	// sleep 3 sec and let the other thread work
	       	s = pthread_mutex_unlock(&mtx2);
			if (s != 0){
				perror("pthread_mutex_unlock error: ");
				break;
			}
			s = pthread_cond_signal(&cond2);
			if (s != 0){
				perror("pthread_cond_signal error: ");
				break;
			}
			sleep(3);
	    	s = pthread_mutex_lock(&mtx2);
			if (s != 0){
				perror("pthread_mutex_lock error: ");
			    break;
			}
			// Append it again
			s = ListAppend(ReadList, buf);
			if (s == -1){
				// terminate program
				lastMsg = 1;
			}						
		}
		// save the cancel request
		if (buf[0] == '!' && numBytes == 2){
			lastMsg = 1;
		}	
        s = pthread_mutex_unlock(&mtx2);
		if (s != 0){
			perror("pthread_mutex_unlock error: ");
			break;
		}
		s = pthread_cond_signal(&cond2);
		if (s != 0){
			perror("pthread_cond_signal error: ");
			break;
		}
		if (lastMsg == 1){
			break;
		}	      
    }
	// sleep the thread for a bit to let any remaining messages in the list(s) be processed 
	sleep(1);
	pthread_cancel(writeMsg_thread);
	// close socket
	close(sfd);
	// cancel input and send threads
    pthread_cancel(readMsg_thread);
    pthread_cancel(sendMsg_thread);
 	pthread_exit(NULL);   	
}

// Free List
void HeapFree(void *item){
	free(item);
}
#ifndef _MESSAGE_H
#define _MESSAGE_H


struct process;

#include <stddef.h>
#include <stdint.h>  

typedef int pid_t;  


enum msg_type {
    MSG_NONE = 0,
    MSG_OPEN,    
    MSG_READ,    
    MSG_WRITE,   
    MSG_CLOSE,   
    MSG_STAT,    
    MSG_BIND,    
    MSG_MOUNT,   
    MSG_FORK,    
    MSG_EXEC,    
    MSG_WAIT,    
    MSG_PIPE,    
    MSG_READ_DIR,
    MSG_UNBIND,  
};

#define MSG_NONBLOCK 0x01

struct Message {
    uint64_t type;           
    char *path;         
    char **argv;        
    void *data;         
    size_t size;        
    int flags;          
    int fd;             
    pid_t pid;          
    int status;         
    unsigned long entry;  
    struct dirent *dirents;  
    size_t dirent_count;     
};


int send_message(struct Message *msg);
int receive_message(struct Message *msg);

#define MAX_MESSAGES 32

struct message_queue {
    struct Message messages[MAX_MESSAGES];
    int head;
    int tail;
    int count;
};


int queue_message(struct process *proc, struct Message *msg);

#endif 
#include "message.h"
#include "process.h"
#include "uart.h"
#include "vfs.h"
#include "string.h"


int queue_message(struct process *proc, struct Message *msg) {
    struct message_queue *queue = &proc->msg_queue;
    
    if (queue->count >= MAX_MESSAGES) {
        return -1;  
    }

    memcpy(&queue->messages[queue->tail], msg, sizeof(struct Message));
    queue->tail = (queue->tail + 1) % MAX_MESSAGES;
    queue->count++;
    
    
    if (proc->msg_blocked) {
        proc->msg_blocked = 0;
        
    }
    
    return 0;
}

int send_message(struct Message *msg) {
    
    switch(msg->type) {
        case MSG_OPEN: {
            int fd = vfs_open(msg->path);
            msg->fd = fd;  
            return fd;
        }
        case MSG_READ: {
            uart_puts("Reading from fd: ");
            uart_hex(msg->fd);
            uart_puts("\n");
            
            char buf[256];
            ssize_t bytes = vfs_read(msg->fd, buf, sizeof(buf));
            if (bytes > 0) {
                memcpy(msg->data, buf, bytes);
                msg->size = bytes;
            }
            return bytes;
        }
        case MSG_FORK: {
            struct process *new = create_process();
            if (!new) return -1;
            
            
            msg->pid = new->pid;  
            
            
            if (new == get_current_process()) {
                return 0;
            }
            
            
            return 0;
        }
        case MSG_EXEC: {
            return handle_exec_message(msg);
        }
        case MSG_WAIT: {
            struct process *current = get_current_process();
            uart_puts("Parent (PID ");
            uart_hex(current->pid);
            uart_puts(") waiting for children\n");
            
            
            struct process *p = process_list;
            while (p) {
                if (p->parent_pid == current->pid && p->state == PROC_ZOMBIE) {
                    msg->status = p->exit_status;
                    uart_puts("Found zombie child, status: ");
                    uart_hex(msg->status);
                    uart_puts("\n");
                    return 0;
                }
                p = p->next;
            }
            
            
            current->state = PROC_BLOCKED;
            schedule();
            
            
            p = process_list;
            while (p) {
                if (p->parent_pid == current->pid && p->state == PROC_ZOMBIE) {
                    msg->status = p->exit_status;
                    uart_puts("Child exited with status: ");
                    uart_hex(msg->status);
                    uart_puts("\n");
                    return 0;
                }
                p = p->next;
            }
            
            return -1;
        }
        case MSG_PIPE: {
            
            
            return 0;
        }
        case MSG_READ_DIR:
            return handle_read_dir_message(msg);
        case MSG_UNBIND:
            return unbind(msg->path);
        default:
            return -1;
    }
}

int receive_message(struct Message *msg) {
    struct process *current = get_current_process();
    struct message_queue *queue = &current->msg_queue;
    
    if (queue->count == 0) {
        
        if (!(msg->flags & MSG_NONBLOCK)) {
            current->msg_blocked = 1;
            
            schedule();  
        }
        return -1;
    }
    
    
    memcpy(msg, &queue->messages[queue->head], sizeof(struct Message));
    queue->head = (queue->head + 1) % MAX_MESSAGES;
    queue->count--;
    
    return 0;
}

int handle_message(struct Message *msg) {
    switch (msg->type) {
        case MSG_OPEN:
            return send_message(msg);
        case MSG_READ:
            return send_message(msg);
        case MSG_FORK:
            return send_message(msg);
        case MSG_EXEC:
            return send_message(msg);
        case MSG_WAIT:
            return send_message(msg);
        case MSG_PIPE:
            return send_message(msg);
        default:
            uart_puts("Unknown message type: ");
            uart_hex(msg->type);
            uart_puts("\n");
            return -1;
    }
} 
#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <stdbool.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "filesys/filesys.h"

#include "devices/shutdown.h" /* For halt. */
#include "filesys/filesys.h"  /* For filesys ops. */

/* Handler function. */
static void syscall_handler(struct intr_frame *);

/* Helper functions. */
static uint32_t get_arg(struct intr_frame *f, int offset);
static void ret_arg(struct intr_frame *f, uint32_t retval);

/* Handlers for Project 4. */
static void     halt(struct intr_frame *f);
static void     exit(struct intr_frame *f);
static void     exec(struct intr_frame *f);
static void     wait(struct intr_frame *f);
static void   create(struct intr_frame *f);
static void   remove(struct intr_frame *f);
static void     open(struct intr_frame *f);
static void filesize(struct intr_frame *f);
static void     read(struct intr_frame *f);
static void    write(struct intr_frame *f);
static void     seek(struct intr_frame *f);
static void     tell(struct intr_frame *f);
static void    close(struct intr_frame *f);


void syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

uint32_t* verify_pointer(uint32_t* p) {
    if (!is_user_vaddr(p))
        return NULL;
    // uint32_t* vp = (uint32_t*)pagedir_get_page(thread_current()->pagedir, p);
    // if (vp == NULL)
    //     return NULL;
    return p;

}


static void syscall_handler(struct intr_frame *f) {
    uint32_t *stack = verify_pointer((uint32_t*)f->esp);
    // TODO Handle
    if (stack == NULL) 
        thread_exit();
    int syscall_num =  *(stack);
    // hex_dump(0, stack-128, 256, true);
    printf("system call %d!\n", syscall_num);

    switch(syscall_num) {
        case SYS_HALT :
            halt(f);
            break;

        case SYS_EXIT :
            exit(f);
            break;

        case SYS_EXEC :
            exec(f);
            break;

        case SYS_WAIT :
            wait(f);
            break;

        case SYS_CREATE :
            create(f);
            break;

        case SYS_REMOVE :
            remove(f);
            break;

        case SYS_OPEN :
            open(f);
            break;

        case SYS_FILESIZE :
            filesize(f);
            break;

        case SYS_READ :
            read(f);
            break;

        case SYS_WRITE :
            write(f);
            break;

        case SYS_SEEK :
            seek(f);
            break;

        case SYS_TELL :
            tell(f);
            break;

        case SYS_CLOSE :
            close(f);
            break;

    }
}


static uint32_t get_arg(struct intr_frame *f, int offset) {
    /* We only handle syscalls with <= 3 arguments. */
    ASSERT(offset <= 3);
    ASSERT(offset >= 0);
    ASSERT(f);

    /* Obtain stack pointer. */
    uint32_t *stack = (uint32_t*)f->esp;
    // hex_dump(0, stack, 100, true);

    /* Move to offset. */
    return *(stack + offset);
}


static void halt(struct intr_frame *f UNUSED) {
    /* Terminate Pintos. */
    shutdown_power_off();
}


static void exit(struct intr_frame *f) {
    /* Parse arguments. */
    int status = get_arg(f, 1);

    /* Status code returned to kernel; TODO when writing wait. */
    f->eax = status;
    thread_exit();
}


static void exec(struct intr_frame *f) {
    /* Parse arguments. */
    const char* file = (const char*) get_arg(f, 1);
    

    f->eax = process_execute(file);

    process_wait(f->eax);

    // Temp
    // thread_exit();
}


static void wait(struct intr_frame *f) {
    /* Parse arguments. */
    pid_t pid = get_arg(f, 1);

    // Temp
    (void)pid;
    thread_exit();
}


static void create(struct intr_frame *f) {
    /* Parse arguments. */
    const char* file = (const char*) get_arg(f, 1);
    unsigned initial_size = get_arg(f,2);

    if (file) {
        /* Create file, return boolean value. */
        f->eax = filesys_create(file, initial_size);
    } else {
        /* Invalid file name. */
        thread_exit();
    }

}


static void remove(struct intr_frame *f) {
    /* Parse arguments. */
    const char* file = (const char*) verify_pointer((uint32_t*)get_arg(f, 1));

    // Temp
    (void)file;
    thread_exit();
}


static void open(struct intr_frame *f) {
    /* Parse arguments. */
    // const char* file = (const char*) verify_pointer((uint32_t*)get_arg(f, 1));
    // hex_dump(0, 0xc0275ec4, 256, true);
    const char* file = (const char*) get_arg(f, 1);
    // printf("%s\n", file);

    struct file* file_s = filesys_open("ls");
    // printf("%p\n", file_s);

    if (file_s == NULL) {
        printf("404 FILE NOT FOUND\n");
        thread_exit();
    }

    /* Use the address of the file object as the fd. */
    uint32_t fd = (uint32_t) file_s;
    ASSERT(fd != STDIN_FILENO || fd != STDOUT_FILENO);

    printf("fd: %u\n", fd);

    f->eax = fd;

    // TODO record fd in process

}


static void filesize(struct intr_frame *f) {
    /* Parse arguments. */
    uint32_t fd = get_arg(f, 1);

    // Temp
    (void)fd;
    thread_exit();
}


static void read(struct intr_frame *f) {
    /* Parse arguments. */
    uint32_t fd = get_arg(f, 1);
    void* buffer = (void *) get_arg(f, 2);
    unsigned size = get_arg(f, 3);

    // Temp
    (void)fd;
    (void)buffer;
    (void)size;
    thread_exit();
}


static void write(struct intr_frame *f) {
    /* Parse arguments. */
    uint32_t fd = get_arg(f, 1);
    const char* buffer = (char *) get_arg(f, 2);
    uint32_t size = get_arg(f, 3);

    
    ASSERT(fd != STDIN_FILENO);
    // printf("fd: %u, size: %d\n", fd, size);
    if (fd == STDOUT_FILENO)
        putbuf(buffer, size);
    // printf("DONE\n");

    // Temp
    (void)fd;
    (void)buffer;
    (void)size;
    // thread_exit();
}


static void seek(struct intr_frame *f) {
    /* Parse arguments. */
    uint32_t fd = get_arg(f, 1);
    unsigned position = get_arg(f, 2);

    // Temp
    (void)fd;
    (void)position;
    thread_exit();
}


static void tell(struct intr_frame *f) {
    /* Parse arguments. */
    uint32_t fd = get_arg(f, 1);

    // Temp
    (void)fd;
    thread_exit();
}


static void close(struct intr_frame *f) {
    /* Parse arguments. */
    uint32_t fd = get_arg(f, 1);

    // Temp
    (void)fd;
    thread_exit();
}


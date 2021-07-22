#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
// #include <semaphore.h>

#define IPC_RESULT_ERROR -1
#define KEY_RANGE 10000
#define NULL_MEMORY 0
#define POINTER_LENGTH 0
#define BLOCK_LENGTH 200

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

template <typename T, std::size_t N> struct my_object {
    // sem_t  sem1;            /* POSIX unnamed semaphore */
    // sem_t  sem2;            /* POSIX unnamed semaphore */
    // char * datetime;
    T data1[N]; // 4
    T data2[N];
    time_t raw_time[N]; //4
    // char datetime[30];
    bool complete; // 1
};


struct pointer_object {
    int index;
    bool complete;
};
typedef struct pointer_object pointer_object;
// // attach a shared memory block, associated with filename, create if doesnt exist

// my_object<double>* attach_memory_block(int size,int id,int random_key);

// bool detach_memory_block(my_object<double>* block);

// bool free_memory_block(int id, int random_key);

// all of the programs will share these values

// change this to variable


#endif

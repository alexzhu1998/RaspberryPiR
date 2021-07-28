#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h> //for time
#include <chrono> // for measuring real execution time
#include <thread> // for sleep

// #include <semaphore.h>

#define IPC_RESULT_ERROR -1
#define NULL_MEMORY 0
#define POINTER_LENGTH 0
#define BLOCK_LENGTH 200
#define DATE_STRING_SIZE 30


typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define intervalDuration(a) std::chrono::duration_cast<std::chrono::milliseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
#define millisleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)



template <typename T, std::size_t N> struct data_obj {
    // sem_t  sem1;            /* POSIX unnamed semaphore */
    // sem_t  sem2;            /* POSIX unnamed semaphore */
    // char * datetime;
    T data1[N]; // 4
    T data2[N];
    time_t raw_time[N]; //4
    // char datetime[30];
    bool complete; // 1
};


struct data_ptr {
    int index;
    bool complete;
};
typedef struct data_ptr data_ptr;




time_t get_raw_time();

void convert_to_string(char*datetime, time_t rawtime);

void check_interrupt_fn(void *dummy);

int pending_interrupt();



// // attach a shared memory block, associated with filename, create if doesnt exist

// data_obj<double>* attach_memory_block(int size,int id,int random_key);

// bool detach_memory_block(data_obj<double>* block);

// bool free_memory_block(int id, int random_key);

// all of the programs will share these values

// change this to variable


#endif

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdbool.h>
#include <string>
#include <vector>

#define IPC_RESULT_ERROR -1
#define KEY_RANGE 10000
#define NULL_MEMORY 0
#define BLOCK_SIZE 50
#define NUM_BLOCKS 100


template <typename T, typename U> struct my_object {
    // char * datetime;
    T data1; // 4
    U data2;
    time_t raw_time; //4
    // char datetime[30];
    int cur_id; // 4
    bool complete; // 1
};


// attach a shared memory block, associated with filename, create if doesnt exist
template <typename T, typename U>
void attach_memory_block(my_object<T,U>* object, int size,int id,int random_key);

template <typename T, typename U>
bool detach_memory_block(my_object<T,U>* block);

bool free_memory_block(int id, int random_key);

// all of the programs will share these values

// change this to variable


#endif
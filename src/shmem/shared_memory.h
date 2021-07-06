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

struct my_object {

    // char * datetime;
    double data1; // 4
    double data2;
    time_t raw_time; //4
    // char datetime[30];
    int cur_id; // 4
    bool complete; // 1
};


// attach a shared memory block, associated with filename, create if doesnt exist

my_object* attach_memory_block(int size,int id,int random_key);

bool detach_memory_block(my_object* block);

bool free_memory_block(int id, int random_key);

// all of the programs will share these values

// change this to variable


#endif
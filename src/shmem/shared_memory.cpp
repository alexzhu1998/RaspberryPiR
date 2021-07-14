#include <stdlib.h>
#include <sys/types.h> //key_t
#include <sys/ipc.h>
#include <sys/shm.h>
#include <R.h>


#include "shared_memory.h"

// attach a shared memory block, associated with filename, create if doesnt exist
static int get_shared_block(int size, int id, int random_key) {
    
    key_t key = random_key%KEY_RANGE+id;
    if (key == IPC_RESULT_ERROR) return IPC_RESULT_ERROR;

    return shmget(key,size,IPC_CREAT |0666);
}

template <typename T>
void attach_memory_block(my_object<T>* object, int size,int id,int random_key) {
    int shared_block_id = get_shared_block(size,id, random_key);

    if (shared_block_id == IPC_RESULT_ERROR) perror("IPC_RESULT_ERROR: ");

    object = (my_object<T> *)shmat(shared_block_id, NULL,0);
    if (object == (void *) IPC_RESULT_ERROR) perror("shmat: ");
    return;
}

template <typename T>
bool detach_memory_block(my_object<T>* block) {
    return (shmdt(block) != IPC_RESULT_ERROR);
}


bool free_memory_block(int id,int random_key) {
    int shared_block_id = get_shared_block(NULL_MEMORY,id,random_key);

    if (shared_block_id == IPC_RESULT_ERROR) return NULL;
    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}
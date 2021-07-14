#include <stdlib.h>
#include <sys/types.h> //key_t
#include <sys/ipc.h>
#include <sys/shm.h>
#include <R.h>


#include "shared_memory.h"

static int get_shared_block(int size, int id, int random_key) {
    
    key_t key = random_key%KEY_RANGE+id;
    if (key == IPC_RESULT_ERROR) return IPC_RESULT_ERROR;

    return shmget(key,size,IPC_CREAT |0666);
}

template <typename T, typename U>
void attach_memory_block(my_object<T,U>* object, int size,int id,int random_key) {
    int shared_block_id = get_shared_block(size,id, random_key);
    

    if (shared_block_id == IPC_RESULT_ERROR) error("IPC_RESULT_ERROR: ");

    object = (my_object<T,U> *)shmat(shared_block_id, NULL,0);
    if (object == (void *) IPC_RESULT_ERROR) error("shmat: ");
    return;
}

template <typename T, typename U>
bool detach_memory_block(my_object<T,U>* block) {
    return (shmdt(block) != IPC_RESULT_ERROR);
}


bool free_memory_block(int id,int random_key) {
    int shared_block_id = get_shared_block(NULL_MEMORY,id,random_key);

    if (shared_block_id == IPC_RESULT_ERROR) return NULL;
    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}
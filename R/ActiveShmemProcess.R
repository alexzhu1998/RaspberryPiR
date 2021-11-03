listActive <- function () { 
    print("Files at Mmap Location: ")
    ExistingMmapFiles <- system("ls /dev/shm/")
    print("===========")
    print("Memory Consumed by Shared Memory: ")
    MemoryConsumed <- system("df -h | grep shm")
    print("===========")

    print("Active Process: ")
    ActiveProcess <- system('pgrep -a "R" | grep "R.*write"')
    print("===========")
    
}
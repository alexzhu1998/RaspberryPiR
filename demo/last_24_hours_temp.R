library(RaspberryPiR)
library(mmap)
DHT11TempHours <- "/dev/shm/RDHT11TempHours"
DHT11HumHours <- "/dev/shm/RDHT11HumHours"
DHT11TimeHours <- "/dev/shm/RDHT11TimeHours"
DHT11Pointer <- "/dev/shm/RDHT11Pointer"
temp<- as.mmap(1:100L,mode = real32(),file = DHT11TempHours)
hum <-as.mmap(1:100L,mode = real32(),file = DHT11HumHours)
dates <- as.mmap(1:100L,mode = int8(),file = DHT11TimeHours)
pointer <- as.mmap(1:5,mode = int8(),file= DHT11Pointer)

i = 1
infloop <- function () {
    while (1) {
        if (i == 101) {
            i = 1
        }
        var <- DHT11_readMemory(1)
        dates[i] <- as.integer(substr(Sys.time(),12,13))
        temp[i] <- var$temperature[1]
        hum[i] <- var$humidity[1]
        pointer[1] = i
        print(i)
        i = i+1
        Sys.sleep(3600)
    }
    
}


tryCatch(
    infloop(),
    interrupt = function(e) "unlinking"
)
print("unmapping")
munmap(temp); munmap(hum); munmap(dates);
unlink(DHT11TempHours); unlink(DHT11HumHours); unlink(DHT11TimeHours)


# Read mmap by mmap(DHT11TempHours)
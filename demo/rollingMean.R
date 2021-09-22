library(RaspberryPiR)
# library(datastructures)
mov_avg_temp = 0
mov_avg_hum = 0
mov_d2_temp = 0

last_index= -1
EMA <- function(new_dat, prev_mov_avg, alpha = 0.5) {
    return (prev_mov_avg + (new_dat - prev_mov_avg)*alpha)
}

SMA <- function (new_dat, prev_mov_avg, n) {
    return (prev_mov_avg + (new_dat-prev_mov_avg)/n)
}

d2 <- function(new_dat,prev_d2,cur_mov_avg,prev_mov_avg) {
    return (prev_d2 + (new_dat-cur_mov_avg)*(new_dat-prev_mov_avg))
}






counter = 1
var <- DHT11_readMemory(1)
cur_index <- DHT11_scanPointer()
mov_avg_temp <- var$temperature[1]

prev_mov_avg_temp <- mov_avg_temp
len = 10

while (1) {
    Sys.sleep(0.001)
    cur_index <- DHT11_scanPointer()
    if (cur_index == last_index) {
        next
    } else {
        
        var <- DHT11_readMemory(1)
        last_index = cur_index
        prev_mov_avg_temp <- mov_avg_temp
        mov_avg_temp <- SMA(var$temperature[1], mov_avg_temp,counter+1)
        mov_d2_temp <- d2(var$temperature[1],mov_d2_temp,mov_avg_temp,prev_mov_avg_temp)
        
        print(paste("SMA: Temperature", mov_avg_temp, "Mov_Std",sqrt(mov_d2_temp/(len-1))))
        counter = counter + 1
    }
}


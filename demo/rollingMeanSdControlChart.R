library(RaspberryPiR)
library(shiny)
# library(datastructures)
cur_SMA = 0
cur_s2 = 0
W <- 10
last_index= -1
EMA <- function(new_dat, prev_mov_avg, alpha = 0.5) {
    return (prev_mov_avg + (new_dat - prev_mov_avg)*alpha)
}

s2_EMA <- function(new_dat, prev_s2_EMA, prev_EMA,cur_EMA, alpha = 0.5) {
    return((1-alpha)*prev_s2_EMA + alpha*(new_dat-prev_EMA)*(new_dat-cur_EMA))
}

SMA_windowed <- function (new_dat, prev_mov_avg, n, pop_dat, window_len = W) {
    if (n > window_len) {
        return (prev_mov_avg + (new_dat-pop_dat)/window_len)
    }
    return (prev_mov_avg + (new_dat-prev_mov_avg)/n)
}

s2_windowed <- function(new_dat,prev_s2,cur_mov_avg,prev_mov_avg, n,pop_dat,window_len = W) {
    if (n > window_len) {
        return (prev_s2 +(new_dat-pop_dat)*(new_dat+pop_dat-cur_mov_avg-prev_mov_avg)/(window_len-1))
    } 
    return ((prev_s2*(n-2) + (new_dat-cur_mov_avg)*(new_dat-prev_mov_avg))/(n-1))
    
    
}


q <- rep(0,W)

counter = 2
q_index <- counter
var <- DHT11_readMemory(1)

cur_index <- DHT11_scanPointer()
cur_SMA <- var$temperature[1]
q[1] = cur_SMA

prev_SMA <- cur_SMA

f <- 3

while (1) {
    cur_index <- DHT11_scanPointer()
    if (cur_index == last_index) {
        next
    } else {
        var <- DHT11_readMemory(1)
        cur <- var$temperature
        ### Queue usage
        if (counter > W) {
            
            if (q_index == W+1) {
                q_index <- 1
            }
            q_popped<- q[q_index]
            
        }
        q[q_index] <- cur
        
        last_index = cur_index
        prev_SMA <- cur_SMA
        cur_SMA <- SMA_windowed(cur, prev_SMA,counter,q_popped)
        cur_s2 <- s2_windowed(cur,cur_s2,cur_SMA,prev_SMA,counter,q_popped)
        upper <- cur_SMA + f*sqrt(cur_s2)
        lower <- cur_SMA - f*sqrt(cur_s2)
        
        print(paste("SMA: Temperature", round(cur_SMA,2), "Mov_Std",round(sqrt(cur_s2),2), "Lower Upper Limit", round(lower,2), round(upper,2)))
        if (cur < lower || cur > upper) print("Drift!")
        counter = counter + 1
        q_index = q_index + 1
    }
}


library(RaspberryPiR)

# univariate Kalman Filter

# Guesses
x_i_i <- 15
x_ip1_i <- x_i_i
i<- 1
dt <- 1
p_i_i <- 10000
q <- 0.15
p_ip1_i <- p_i_i + q
K<- 1
r <- 0.1

counter = 1
cur_index <- PhotoRes_scanPointer() # Change this for different sensors
last_index= -1



while (1) {
    Sys.sleep(0.001)
    cur_index <- PhotoRes_scanPointer()
    if (cur_index == last_index) {
        next
    } else {
        var <- PhotoRes_readMemory(1) # Change this for different sensors
        z_i <- var$time_to_charge[1]
        last_index <- cur_index
        K <- p_ip1_i/(p_ip1_i+r)
        x_i_i <- x_ip1_i + K*(z_i - x_ip1_i)
        p_i_i <- (1-K)*p_ip1_i

        x_ip1_i <- x_i_i
        p_ip1_i <- p_i_i + q
        print(var$datetime)
        print(paste("Measured Value: ",z_i))
        print(paste("Predicted Value: ",x_ip1_i))
        print(paste("Estimated Variance: ",p_ip1_i))
        
        counter <- counter+1
    }
}

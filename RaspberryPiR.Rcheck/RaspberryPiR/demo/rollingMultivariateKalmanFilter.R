library(RaspberryPiR)

n = 3
x_i_i <- c(0,0,0)
F <- diag(n)
x_ip1_i <- crossprod(F,x_i_i)
p_i_i <- diag(c(10000,10000,10000),n,n)

Q <- diag(rep(0.15,n))
p_ip1_i <- crossprod(t(F),tcrossprod(p_i_i,F)) + Q
K <- diag(rep(1,n))
R_i <- diag(rep(0.1,n))
H <- diag(rep(1,n))

counter = 1
cur_index_PhotoRes <- PhotoRes_scanPointer()
last_index_PhotoRes <- -1
cur_index_DHT11 <- DHT11_scanPointer()
last_index_DHT11 <- -1 

while (1) {
    Sys.sleep(0.001) 
    cur_index_PhotoRes <- PhotoRes_scanPointer()
    cur_index_DHT11 <- DHT11_scanPointer()
    if (cur_index_DHT11 == last_index_DHT11 || cur_index_PhotoRes == last_index_PhotoRes) {
        next
    } else {
        var_PhotoRes <- PhotoRes_readMemory(1)
        var_DHT11 <- DHT11_readMemory(1)
        z_i <- c(var_DHT11$temperature[1],var_DHT11$humidity[1],var_PhotoRes$time_to_charge[1])

        K <- crossprod(t(p_ip1_i),crossprod(H,solve(crossprod(t(H),tcrossprod(p_ip1_i,H))+R_i)))
        x_i_i <- x_ip1_i + crossprod(K,(z_i - crossprod(H,x_ip1_i)))
        p_i_i <- crossprod((diag(n)-crossprod(t(K),H)),tcrossprod(p_ip1_i,(diag(n)-crossprod(t(K),H)))) + crossprod(t(K),tcrossprod(R_i,K))
        
        x_ip1_i <-crossprod(F,x_i_i)
        p_ip1_i <- crossprod(t(F),tcrossprod(p_i_i,F)) + Q

        print(var_PhotoRes$datetime)
        print(paste("Measured Value: ", z_i))
        print(paste("Predicted Value: ", x_ip1_i))
        print(paste("Estimate Variance: ", diag(p_ip1_i)))

        last_index_PhotoRes = cur_index_PhotoRes
        last_index_DHT11 = cur_index_DHT11
        counter = counter + 1
    }
}


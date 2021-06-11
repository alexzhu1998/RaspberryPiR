source("R/DHT_setup.R")

# res <- DHTInitiate()
# print(res)
temp = c()
humid = c()
time = c()
sampTime = 0
rD = 100
for (count in 1:10) {
    print(paste("Current Counter",count))
    start = Sys.time()
    res <- DHTCFunction(sampTime,rD)
    end = Sys.time()
    print(end-start)
    print(paste("Temperature",res$tempOut))
    print(paste("Humidity",res$humidOut))
    temp = c(temp,res$tempOut)
    humid = c(humid,res$humidOut)
    time = c(time,end)
    # plot(time,res$tempOut, type ='l', main = 'Realtime Data from Raspberry Pi')
}




# print(store)




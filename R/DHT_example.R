source("R/DHT_setup.R")

# res <- DHTInitiate()
# print(res)
count = 0
temp = c()
humid = c()
for (count in 1:10) {
    print(paste("Current Counter",count))
    res <- DHTCFunction()
    store = c(store,res$tempOut)
    humid = c(humid,res$humidOut)
    print(paste("Temperature",res$tempOut))
    print(paste("Humidity",res$humidOut))
}

print(store)




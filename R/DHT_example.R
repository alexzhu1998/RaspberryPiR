source("R/DHT_setup.R")

# res <- DHTInitiate()
# print(res)
count = 0
store = c()
while (count < 10) {
  print(paste0("Current Counter",count))
  res <- DHTCFunction()
  store = c(store,res)
  print(paste0("Temperature",res))
  count = count+ 1
}

print(store)




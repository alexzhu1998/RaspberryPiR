source("R/DHT_setup.R")

longitude = c(-110)
latitude = c(30)

curDir = "R/data/"
sensorID = c("DHT11")
sampTime = 0
readDelay = 100
NIter = 100
for (count in 1:NIter) {
  print(paste("Current Counter",count))
  start = Sys.time()
  res <- DHTCFunction(sampTime,readDelay)
  end = Sys.time()
  print(end-start)
  print(paste("Temperature",res$tempOut))
  print(paste("Humidity",res$humidOut))
  # temp = c(temp,res$tempOut)
  # humid = c(humid,res$humidOut)
  time = c(time,end)
  dat = data.frame(time=as.numeric(end),
                   sensorID=sensorID,
                   longitude=longitude,
                   latitude=latitude,
                   temperature = as.numeric(res$tempOut))
  write.csv(dat,
            paste0(curDir,"sensorData",gsub("[^0-9]","",Sys.time()),".csv"),
            row.names = FALSE)
}

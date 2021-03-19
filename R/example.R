
source("fibonacci_main.R")
wiringPiSetup()
ledPin = "GPIO17"
setPin("GPIO17","OUTPUT")
while (1) {
    output(ledPin,1)
    Sys.sleep(1)
    output(ledPin,0)
    Sys.sleep(1)
}

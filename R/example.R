
source("base_functions.R")
wiringPiSetup()
ledPin = "GPIO17"
setPin(ledPin,"OUTPUT")
while (1) {
    output(ledPin,1)
    Sys.sleep(1)
    output(ledPin,0)
    Sys.sleep(1)
}

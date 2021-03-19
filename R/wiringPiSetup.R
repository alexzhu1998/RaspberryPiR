# rinternals <- file.path(R.home("include"), "Rinternals.h")
# file.show(rinternals)
dyn.load("RaspberryPiR/src/wiringPiSetup.so")

MySetup <- function() .C("MySetup")

MyPinMode <- function(pin, mode) .C("MyPinMode",as.integer(pin),as.integer(mode))

# MyDigitalRead <- function(a) .C("MyDigitalRead", as.integer(a))
print("Hello")
print("Set up")
MySetup()
MyPinMode(0,1)
# MyDigitalRead(0)



library(RaspberryPiR)
LL_wiringPiSetup()
ledpin = 1
LL_pinMode(ledpin,"OUTPUT")
while (1) {
    LL_digitalWrite(ledpin,1)
    print("led turned on >>>")
    Sys.sleep(1)
    LL_digitalWrite(ledpin,0)
    print("led turned off >>>")
    Sys.sleep(1)
}



##### Tutorial 2
# library(RaspberryPiR)
# LL_wiringPiSetup()

# ledpin = 1 #GPIO18
# buttonpin = 2 #GPIO27
# LL_pinMode(ledpin,"OUTPUT")
# LL_pinMode(buttonpin,"INPUT")

# LL_pullUpDnControl(buttonpin,"PUD_UP")
# while (1) {
#     if (LL_digitalRead(buttonpin) == 0) {
#         LL_digitalWrite(ledpin,1)
#         print("Button is pressed, LED ON")
#     }else {
#         LL_digitalWrite(ledpin,0)
#         print("Button is released, LED OFF")
#     }
# }



##### Tutorial 2.2
# library(RaspberryPiR)
# ledPin = 1 #GPIO18
# buttonPin = 2 #GPIO27
# ledState = 0
# buttonState = 1
# lastButtonState=1
# captureTime = 50
# reading = 0
# lastChangeTime = 0

# LL_wiringPiSetup()

# LL_pinMode(ledPin,"OUTPUT")
# LL_pinMode(buttonPin,"INPUT")

# LL_pullUpDnControl(buttonPin,"PUD_UP")
# while (1) {
#     reading = LL_digitalRead(buttonPin)
#     if (reading != lastButtonState) {
#         lastChangeTime = LL_millis()
#     }
#     if (LL_millis()- lastChangeTime > captureTime) {
#         if (reading != buttonState) {
#             buttonState = reading
#             if (buttonState == 0) {
#                 print("Button is pressed")
#                 ledState = !ledState
#                 if (ledState) {
#                     print("Turn on LED")
#                 } else {
#                     print("Turn off LED")
#                 }
#             }else {
#                 print("Button is released")
#             }
#         }
#     }
#     LL_digitalWrite(ledPin,ledState)
#     lastButtonState = reading
# }
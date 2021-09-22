library(shiny)
library(RaspberryPiR)
ui <- basicPage(
    plotOutput(outputId = "image", width = "25%")
)

server <- function(input, output, session) {
    width <- 320
    height <- 240
    channels <- 1
    last_value <- -1
    
    autoInvalidate <- reactiveTimer(100)

    computation<- eventReactive (curPointer(),{
        x <- RPiCam_readMemory(1)$image
        matrix(x,height,width,byrow = T)
        
    })
    curPointer <- eventReactive(autoInvalidate(),{
        RPiCam_scanPointer()
    })
    
    session$onSessionEnded(function() {
        stopApp()
    })
    output$image<-renderPlot({
        image(computation(),col=grey(seq(0,1,length=256)))   
    })
}
shinyApp(ui, server)

# Use the following if you want to visualise the pictures on local shiny servers
# xvfb-run

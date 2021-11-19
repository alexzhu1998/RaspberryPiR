library(RaspberryPiR)
library(TukeyRegion)
library(shiny)
library(MASS)
# Make sure that DHT11


ui <- basicPage(
    splitLayout(
        cellWidths = c("50%"),    
        plotOutput(outputId = "image1", width = "75%")
    ),
    mainPanel(
        textOutput("selected_var")
    )
)

server <- function(input, output, session) {
    
    N <- 50
    s<- 1e-5
    p<-2
    # cur_index_DHT11 <- DHT11_scanPointer()
    # cur_index_MQ2 <- MQ2_scanPointer()
    # var_DHT11 <- DHT11_readMemory(N)
    # var_MQ2 <- MQ2_readMemory(N)

    
    DHT11_curPointer <- eventReactive(autoInvalidate(),{
        DHT11_scanPointer()
    })
    # MQ2_curPointer <- eventReactive(autoInvalidate(),{
    #     MQ2_scanPointer()
    # })

    autoInvalidate <- reactiveTimer(2000)

    read_bivariate<- eventReactive((DHT11_curPointer()),{
        x <- DHT11_readMemory(N)
        # y <- MQ2_readMemory(N)
        m <- matrix(c(x$temperature,x$humidity),nrow = N, ncol = 2)+ MASS::mvrnorm(N,rep(0,p),s*diag(p))

        # colnames(m) <- c("Temperature", "Smoke Level")
        y <- TukeyRegion(m,
            5,"bfs",retFacets = TRUE, retVolume= TRUE,retBarycenter= TRUE
        )
        return (y)
        
    })
    output$selected_var <- renderText({ 
        read_bivariate()$volume
    })
    # # Checking if each component is working 
    # read_DHT11 <- eventReactive(DHT11_curPointer(),{
    #     x <- DHT11_readMemory(N)
    #     # x$datetime[11:18]
    #     matrix(c(seq(N),x$temperature),nrow = N,ncol = 2)
    # })

    # read_MQ2 <- eventReactive(MQ2_curPointer(),{
    #     x <- MQ2_readMemory(N)
    #     matrix(c(seq(N),rev(x$time_to_charge)),nrow = N,ncol = 2)
    # })
    
    session$onSessionEnded(function() {
        stopApp()
    })
    
    output$image1<-renderPlot({
        plot(read_bivariate(), colorFacets = "red", colorRidges = "red",
     colorPoints = "blue", alpha = 0.35)
        a <- DHT11_readMemory(1)
        abline(h = a$humidity,lty = 2)
        abline(v = a$temperature,lty = 2)
        # mat <- read_DHT11()
        # mat <- read_MQ2()
        # plot(mat[,1],mat[,2])
    })

    
}
shinyApp(ui, server)
# Use the following if you want to visualise the pictures on local shiny servers
# xvfb-run
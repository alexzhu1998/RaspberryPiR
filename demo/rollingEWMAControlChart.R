library(RaspberryPiR)
library(shiny)
library(ggplot2)


# Following is the implementation of EWMA Control Chart from the Thesis.


EMA <- function(new_dat, prev_mov_avg, alpha = 0.5) {
    return (prev_mov_avg + (new_dat - prev_mov_avg)*alpha)
}

s2_EMA <- function(new_dat, prev_s2_EMA, prev_EMA,cur_EMA, alpha = 0.5) {
    return((1-alpha)*prev_s2_EMA + alpha*(new_dat-prev_EMA)*(new_dat-cur_EMA))
}

ui <- fluidPage(
        sidebarLayout(
            sidebarPanel(
                sliderInput("forgetting_fac", "Forgetting Factor",
                            min = 0, max = 1,
                            value = 0.5)
            ),
            mainPanel(
                splitLayout(
                    cellWidths = c("75%"),    
                    plotOutput(outputId = "image2", width = "75%")
                )
            )
        )
    
)

server <- function(input, output, session) {
       
    L <- 30
    ema <- rep(0,L)
    ema_var <- rep(0,L)
    data <- rep(0,L)
    counter <- 1
    data[1] <- ema[1] <- MQ2_readMemory(1)$time_to_charge
    ema_var[1] <- 0
    cur_index <- 2
    last_index <- 1
    
    autoInvalidate <- reactiveTimer(2000)
    session$onSessionEnded(function() {
        stopApp()
    })
    read<- eventReactive (curPointer(),{
        x <- MQ2_readMemory(1)$time_to_charge
    }) 
    
    
    curPointer <- eventReactive(autoInvalidate(),{
        MQ2_scanPointer()
    })

    computation2 <- eventReactive(read(),{
        dat <- read()
        if (cur_index == L+1) {
            cur_index <<- 1
        }
        data[cur_index] <<- dat
        ema[cur_index] <<- EMA(dat,ema[last_index],alpha = input$forgetting_fac)
        ema_var[cur_index] <<- s2_EMA(dat,ema_var[last_index],ema[last_index],ema[cur_index],alpha = input$forgetting_fac)
        last <- last_index
        last_index <<- cur_index
        cur_index <<- cur_index + 1
        if (last_index == L) {
            return(list(x = ema[c(1:last_index)], y_s = ema_var[last], z = last, y = ema[last], w = data[c(1:last_index)]))
        } else {
            return(list(x = ema[c((last_index+1):L,1:last_index)], y_s = ema_var[last],z = last, y = ema[last], w = data[c((last_index+1):L,1:last_index)]))
        }
        
    })
    
    
    
    output$image2<-renderPlot({
        lis <- computation2()
        ggplot() + 
        geom_line(aes(x= seq(1:L),y = lis$x)) + 
        geom_point(aes(x = seq(L), y = lis$w))+
        xlab("Index")+
        ylab("Observed Values") + 
        geom_hline(yintercept = lis$y + 3*sqrt(lis$y_s)) + 
        geom_hline(yintercept = lis$y - 3*sqrt(lis$y_s)) + 
        ggtitle("EWMA Control Chart")
        # abline(h= lis$x + 3*sqrt(lis$y))
        # abline(h= lis$x - 3*sqrt(lis$y))
    })
}
shinyApp(ui, server)
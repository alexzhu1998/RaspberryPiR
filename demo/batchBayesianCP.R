library(shiny)
library(RaspberryPiR)
library(tictoc)
library(pryr)

ui <- fluidPage(
    
        mainPanel(
            splitLayout(
                cellWidths = c("50%", "50%"),    
                plotOutput(outputId = "image1", width = "75%"),
                plotOutput(outputId = "image2", width = "75%")
            )
        )
    
    
)

# Following is the implementation of Bayesian Online Changepoint Detection from the Thesis.

SMA_windowed <- function (new_dat, prev_mov_avg, n) {
    # if (n > window_len) {
    #     return (prev_mov_avg + (new_dat-pop_dat)/window_len)
    # }
    if (prev_mov_avg == -1) return(new_dat)
    return (prev_mov_avg + (new_dat-prev_mov_avg)/n)
}
Len <- seq(50,1000,by = 50) #time keeping

server <- function(input, output, session) {
    time_keep <- rlang::env(
        j = 1,
        mean_val = -1,
        L = Len[1],
        k = 1,
        mean_vals = rep(0,length(Len)), #time keeping
        mem_vals = rep(0,length(Len)) #time keeping
    ) 
    
    
    
    autoInvalidate <- reactiveTimer(2000)

    read<- eventReactive (curPointer(),{
        x <- PhotoRes_readMemory(time_keep$L)$time_to_charge
    }) 
    

    computation <- eventReactive(read(), {

        dat <- read()
        tic("Run OBCP Analysis")
        H <- 1/60
        
        mean0 <- 0 # prior mean for mu
        var0 <- 0.0001 # prior variance for mu
        varx = 0.00005 # likelihood variance
        
        
        # plot(sim_data)
        log_R <- matrix(-Inf,nrow = (time_keep$L+1),ncol = (time_keep$L+1))
        log_R[1,1] = 0

        mean_params <- c(mean0)
        prec_params <- c(1/var0)

        log_message <- c(0)
        log_H <- log(H)
        log_1mH <- log(1-H)
        new_log_joint <- NULL

        # parametric model, uses O(N) space to determine change point
        
        for (t in seq(time_keep$L)) {
            # t <- t+1
            x <- dat[t] # Step 2

            log_pis <- log(dnorm(x,mean_params[1:t],sqrt(1/prec_params[1:t]+varx))) # Step 3

            log_growth_probs <- log_pis + log_message[1:t] + log_1mH # Step 4 Growth Probabilities

            log_cp_prob <- log(sum(exp(log_pis + log_message[1:t] + log_H))) # Step 5 Changepoint probabilities


            new_log_joint <- c(log_cp_prob,log_growth_probs) # Step 6 Calculate evidence

            # Step 7 Determine Run Length Distribution
            log_R[(t+1),1:(t+1)] <- new_log_joint- log(sum(exp(new_log_joint)))

            # Step 8 Update Params
            new_prec_params <- prec_params + (1/varx)
            prec_params <- c(1/var0,new_prec_params) # prepending parameters

            new_mean_params <- (mean_params*prec_params[1:t] + x/varx)/new_prec_params
            mean_params <- c(mean0,new_mean_params) # prepending parameters
            

            log_message <- new_log_joint

        }
        # print(new_log_joint)
        # print(mean_params)
        # print(log_message)

        R <- exp(log_R)
        rownames(R) <- seq(time_keep$L+1)
        colnames(R) <- seq(time_keep$L+1)
        # image(R,col=gray.colors(256))
        res <- dim(R)[1]
        for (i in seq(res)) {
            res[i] = which.max(R[i,])
        }
        
        
        tmp <- toc() #time keeping
        time_keep$mean_val <- SMA_windowed(unname(tmp$toc-tmp$tic),time_keep$mean_val,time_keep$j)#time keeping
        time_keep$j <- time_keep$j+1 #time keeping
        ####### time keeping #######
        if (time_keep$j == 5) {
            # Store current Runs
            print(time_keep$mem_vals[time_keep$k] <- mem_used())
            time_keep$mean_vals[time_keep$k] <- time_keep$mean_val
            print(paste("Mean",time_keep$L,time_keep$mean_val)) #time keeping
            
            
            # reset
            time_keep$mean_val <- -1
            time_keep$j <- 1
            time_keep$k <- time_keep$k + 1
            if (time_keep$k == length(Len)+1) {
                print("Mean")
                print(time_keep$mean_vals)
                print("Memory")
                print(time_keep$mem_vals)
                stopApp()
            }
            time_keep$L <- Len[time_keep$k]
        }
        ####### time keeping #######
        R
    }) 

    
    curPointer <- eventReactive(autoInvalidate(),{
        PhotoRes_scanPointer()
    })
    session$onSessionEnded(function() {
        stopApp()
    })
    
    output$image1<-renderPlot({
        image(computation(),col = gray.colors(256))
    })
    
    output$image2<-renderPlot({
        plot(read(),ylab = "Observed Value")
    })
}
shinyApp(ui, server)
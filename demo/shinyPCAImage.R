library(shiny)
library(RaspberryPiR)
library(onlinePCA)





ui <- basicPage(
    splitLayout(
        cellWidths = c("50%", "50%"),    
        plotOutput(outputId = "image1", width = "75%"),
        plotOutput(outputId = "image2", width = "75%")
    )
)

server <- function(input, output, session) {
    width <- 320
    height <- 240
    channels <- 1
    im_size <- width*height*channels
    i = 10+1
    last_value <- -1
    q <- 10
    n0 <- 10

    cur_index <- RPiCam_scanPointer()
    im <- matrix(RPiCam_readMemory(n0)$image,n0,im_size,byrow = T)
    # im <- t(apply(im, 1, cumsum))
    pca <- prcomp(im[(1:n0),])
    xbar <- pca$center
    # print(all(xbar!= FALSE))
    # print(xbar[1:50])
    pca <- list(values = (pca$sdev[1:q])^2, vectors = pca$rotation[,(1:q)])

    
    autoInvalidate <- reactiveTimer(1000)

    read<- eventReactive (curPointer(),{
        x <- RPiCam_readMemory(1)$image
    })
    im_ori <- reactive(matrix(read(),height,width,byrow = T))

    computation <- eventReactive(read(), {
        im2 <- matrix(read(),1,im_size,byrow = T)
        xbar <- updateMean(xbar,im2[1,],i-1)
        pca <- incRpca(pca$values,pca$vectors,im2[1,],i-1,q=q,center = xbar)
        x_hat <- tcrossprod(sqrt(pca$values),pca$vectors)
        x_hat <- scale(x_hat,center= -xbar,scale = F)
        i <- i + 1
        
        
        matrix(x_hat,height,width,byrow=T)
    }) 
    
    curPointer <- eventReactive(autoInvalidate(),{
        RPiCam_scanPointer()
    })
    session$onSessionEnded(function() {
        stopApp()
    })
    
    output$image1<-renderPlot({
        image(im_ori(),col=grey(seq(0,1,length=256)))
    })

    output$image2 <- renderPlot({
        image(computation(),col=grey(seq(0,1,length=256)))
    })
}
shinyApp(ui, server)

# Use the following if you want to visualise the pictures on local shiny servers
# xvfb-run

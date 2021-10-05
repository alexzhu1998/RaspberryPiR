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
    q <- 50
    n0 <- 50

    cur_index <- RPiCam_scanPointer()
    im <- matrix(RPiCam_readMemory(n0)$image,n0,im_size,byrow = T)
    # im <- t(apply(im, 1, cumsum))
    pca <- prcomp(im[(1:n0),])
    xbar <- pca$center
    # print(all(xbar!= FALSE))
    # print(xbar[1:50])
    pca <- list(values = (pca$sdev[1:n0])^2, vectors = pca$rotation[,(1:n0)])
    pca$values = rep(1,q)
    pca$rotation = matrix(xbar,im_size,q)
    
    autoInvalidate <- reactiveTimer(1000)

    read<- eventReactive (curPointer(),{
        x <- RPiCam_readMemory(1)$image
    })
    im_ori <- reactive(matrix(read(),height,width,byrow = T))

    computation <- eventReactive(read(), {
        im2 <- matrix(read(),1,im_size,byrow = T)
        xbar <- updateMean(xbar,im2[1,],i-1)
        pca <- incRpca(lambda = pca$values,U = pca$vectors,x = im2[1,],n = i-1,f = 0.5,center = xbar)
        print("PCA values: ")
        print(pca$values)
        x_hat <- tcrossprod(sqrt(pca$values),pca$vectors) #square root eigenvalues squared
        # x_hat <- scale(x_hat,center= -xbar,scale = F)
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
        image(im_ori(),col=grey(seq(0,1,length=256))) # use package imageR interpolation default argument
    })

    output$image2 <- renderPlot({
        image(computation(),col=grey(seq(0,1,length=256)))
    })
}
shinyApp(ui, server)

# Use the following if you want to visualise the pictures on local shiny servers
# xvfb-run

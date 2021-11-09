library(shiny)
library(RaspberryPiR)
library(onlinePCA)






ui <- fluidPage(
    
    sidebarLayout(
        sidebarPanel(
            sliderInput("pc_num", "Principal Component Number:",
                        min = 5, max = 30,
                        value = 10)
        ),
        mainPanel(
            splitLayout(
                cellWidths = c("50%", "50%"),    
                plotOutput(outputId = "image1", width = "75%"),
                plotOutput(outputId = "image2", width = "75%")
            )
        )
    )
    
)

server <- function(input, output, session) {
    width <- 320
    height <- 240
    channels <- 1
    im_size <- width*height*channels
    
    last_value <- -1
    # q <- 2
    n0_U <- 320
    n0_V <- 240
    min_UV<- min(n0_U,n0_V)
    
    i <- n0_U

    cur_index <- RPiCam_scanPointer()
    
    im <- matrix(RPiCam_readMemory(1)$image,height,width,byrow = T) # 240 x 320
    svd_V <- svd(im)
    # pc <- prcomp(im, center = F)
    # xbar <- colMeans(im)
    # pca <- list(values = (pc$sdev[1:max_pc])^2, vectors = (pc$rotation[,1:max_pc]))

    # Y <- crossprod(pca$vectors,im)

    # pc_vectors <- svd$v
    # pca <- prcomp(im, center = F)
    # xbar <- pca$center
    # print(all(xbar!= FALSE))
    # print(xbar[1:50])
    ##### 2 rows * 76800 columns  # SVD of this: 240 rows * 320 columns
    # Compression using Reduced SVD method, need to reduce the number of eigenvalues to observe
    pca_V <- list(values = (svd_V$d)^2, vectors = svd_V$v) # U 240 x 240  S 240 x 240 t(V) 240 x 320
    # pca_U <- list(values = (svd_V$d)^2, vectors = svd_V$u) # U 320 x 240  S 240 x 240 t(V) 240 x 240
    
    # svd_V

    # x_hat <- tcrossprod(tcrossprod(im,t(pca$vectors)),pca$vectors)
    # pca$values = rep(1,q)
    # pca$rotation = matrix(xbar,im_size,q)
    
    autoInvalidate <- reactiveTimer(2000)

    read<- eventReactive (curPointer(),{
        x <- RPiCam_readMemory(1)$image
    }) 
    im_ori <- reactive(matrix(read(),height,width,byrow = T))

    computation <- eventReactive(read(), {
        im2 <- matrix(read(),height,width,byrow = T)
        # xbar <- updateMean(xbar,im2[1,],i-1)
        
        pca_V <<- incRpca.block(x = im2,lambda = pca_V$values,U =pca_V$vectors,f = 1-(1/(i+1)), byrow = T)
        # pca_U <<- incRpca.block(x = im2,lambda = pca_U$values,U =pca_U$vectors,f = 1, byrow = F)
        
        print("PCA values: ")
        print(pca_V$values[1:10])
        # print(pca_U$values[1:10])
        # print(i)
        US <- im2 %*% pca_V$vectors[,1:input$pc_num]

        V <- pca_V$vectors[,1:input$pc_num]
        x_hat <- tcrossprod(US,V)
        
        
        # x_hat <- scale(x_hat,center= -xbar,scale = F)
        i <<- i + 1
        x_hat
    }) 
    
    curPointer <- eventReactive(autoInvalidate(),{
        RPiCam_scanPointer()
    })
    session$onSessionEnded(function() {
        stopApp()
    })
    
    output$image1<-renderPlot({
        image(im_ori(),col=grey.colors(256)) # use package imageR interpolation default argument
    })
    output$image2 <- renderPlot({
        image(computation(),col=grey.colors(256))
    })
}
shinyApp(ui, server)

# Use the following if you want to visualise the pictures on local shiny servers
# xvfb-run


##### Just Doing Filtering with PCs
# width <- 320
#     height <- 240
#     channels <- 1
#     im_size <- width*height*channels
    
#     last_value <- -1
#     # q <- 2
#     max_pc <- 50
#     n0_U <- 320
#     n0_V <- 240
#     min_UV<- min(n0_U,n0_V)
    
#     i <- n0_U

#     cur_index <- RPiCam_scanPointer()
    
#     im <- matrix(RPiCam_readMemory(1)$image,height,width,byrow = T) # 240 x 320
#     # svd_V <- svd(im)
#     pc <- prcomp(im, center = F)
#     # xbar <- colMeans(im)
#     pca <- list(values = (pc$sdev[1:max_pc])^2, vectors = (pc$rotation[,1:max_pc]))

#     # Y <- crossprod(pca$vectors,im)

#     # pc_vectors <- svd$v
#     # pca <- prcomp(im, center = F)
#     # xbar <- pca$center
#     # print(all(xbar!= FALSE))
#     # print(xbar[1:50])
#     ##### 2 rows * 76800 columns  # SVD of this: 240 rows * 320 columns
#     # Compression using Reduced SVD method, need to reduce the number of eigenvalues to observe
#     # pca_V <- list(values = (svd_V$d[1:max_pc])^2/n0_U, vectors = svd_V$v[,1:max_pc]) # U 240 x 240  S 240 x 240 t(V) 240 x 320
#     # pca_U <- list(values = (svd_V$d[1:max_pc])^2/n0_V, vectors = svd_V$u[,1:max_pc]) # U 320 x 240  S 240 x 240 t(V) 240 x 240
    
#     # svd_V

#     # x_hat <- tcrossprod(tcrossprod(im,t(pca$vectors)),pca$vectors)
#     # pca$values = rep(1,q)
#     # pca$rotation = matrix(xbar,im_size,q)
    
#     autoInvalidate <- reactiveTimer(2000)

#     read<- eventReactive (curPointer(),{
#         x <- RPiCam_readMemory(1)$image
#     })
#     im_ori <- reactive(matrix(read(),height,width,byrow = T))

#     computation <- eventReactive(read(), {
#         im2 <- matrix(read(),height,width,byrow = T)
#         # xbar <- updateMean(xbar,im2[1,],i-1)
        
#         # pca <<- incRpca.block(lambda = pca$values,U =pca$vectors, x = im2,n0=nrow(pca$vectors)*i,f = 0,byrow = T)
#         # pca_U <<- incRpca.block(lambda = pca_U$values,U =pca_U$vectors, x = im2,n0=nrow(pca_U$vectors),f = 0.9,byrow = F)
        
#         print("PCA values: ")
#         print(pca$values[1:10])
#         # print(pca_U$values[1:10])
#         # print(i)
#         x_hat <- tcrossprod(tcrossprod(im2,t(pca$vectors)),pca$vectors) #square root eigenvalues squared abs value to invert the colors 
        
#         # x_hat <- scale(x_hat,center= -xbar,scale = F)
#         i <<- i + 1
#         x_hat
#     }) 

#### Not Dimension Reducing anything
# server <- function(input, output, session) {
#     width <- 320
#     height <- 240
#     channels <- 1
#     im_size <- width*height*channels
    
#     last_value <- -1
#     # q <- 2
#     max_pc <- 10
#     n0 <- 2
#     i <- n0

#     cur_index <- RPiCam_scanPointer()
#     im <- matrix(RPiCam_readMemory(n0)$image,n0,im_size,byrow = T)
#     # im <- t(apply(im, 1, cumsum))
#     pca <- prcomp(im[(1:n0),], center = F)
#     # xbar <- pca$center
#     # print(all(xbar!= FALSE))
#     # print(xbar[1:50])
#     pca <- list(values = (pca$sdev[1:n0])^2, vectors = pca$rotation[,(1:n0)]) # 2 rows * 76800 columns  # SVD of this: 240 rows * 320 columns
    
#     # pca$values = rep(1,q)
#     # pca$rotation = matrix(xbar,im_size,q)
    
#     autoInvalidate <- reactiveTimer(2000)

#     read<- eventReactive (curPointer(),{
#         x <- RPiCam_readMemory(1)$image
#     })
#     im_ori <- reactive(matrix(read(),height,width,byrow = T))

#     computation <- eventReactive(read(), {
#         im2 <- matrix(read(),1,im_size,byrow = T)
#         # xbar <- updateMean(xbar,im2[1,],i-1)
#         pca <<- incRpca(lambda = pca$values,U = pca$vectors,x = im2[1,],f = 0.5, q= min(i+1,10))
#         print("PCA values: ")
#         print(pca$values)
#         # print(i)
#         x_hat <- abs(tcrossprod(sqrt(pca$values)[1],pca$vectors[,1])) #square root eigenvalues squared abs value to invert the colors 
#         # x_hat <- scale(x_hat,center= -xbar,scale = F)
#         i <<- i + 1
#         matrix(x_hat,height,width,byrow=T)
#     }) 
    
#     curPointer <- eventReactive(autoInvalidate(),{
#         RPiCam_scanPointer()
#     })
#     session$onSessionEnded(function() {
#         stopApp()
#     })
    
#     output$image1<-renderPlot({
#         image(im_ori(),col=grey.colors(256)) # use package imageR interpolation default argument
#     })

#     output$image2 <- renderPlot({
#         image(computation(),col=grey.colors(256))
#     })
# }
# shinyApp(ui, server)
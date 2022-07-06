# install.packages("eventstream")
library(RaspberryPiR)

width <- 320
height <- 240
channels <- 1
im_size <- width*height*channels
q <- 10
n0 <- 10
i = 10+1
last_index= -1

im <- matrix(RPiCam_readMemory(n0)$image,n0,im_size,byrow = T)
# im <- t(apply(im, 1, cumsum))
pca <- prcomp(im[(1:n0),])

# reconstruct matrix
restr <- tcrossprod(pca$x[,1:n0], pca$rotation[,1:n0])

# unscale and uncenter the data
if(pca$scale != FALSE){
  restr <- scale(restr, center = FALSE , scale=1/pca$scale)
}
if(all(pca$center != FALSE)){
  restr <- scale(restr, center = -1 * pca$center, scale=FALSE)
}

# plot your original image and reconstructed image
# par(mfcol=c(1,2), mar=c(1,1,2,1))
# im <- matrix(data=rev(im.train[2,]), nrow=96, ncol=96)
# image(1:96, 1:96, im, col=gray((0:255)/255))

# rst <- matrix(data=rev(restr[2,]), nrow=96, ncol=96)
# image(1:96, 1:96, rst, col=gray((0:255)/255))

print(restr[2,1:50])
rst <- matrix(data = restr[2,],height,width,byrow = T)

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
    
    
    
    
    output$image<-renderPlot({
        image(rst,col=grey(seq(0,1,length=256)))   
    })
}
shinyApp(ui, server)

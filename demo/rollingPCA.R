library(onlinePCA)
library(RaspberryPiR)

# Need a quick Cpp function that stores attributes of the sensors
# Store the data into a common mmap location, for easier display later on

width <- 320
height <- 240
channels <- 1
im_size <- width*height*channels
q <- 10
n0 <- 10
i = 10+1
last_index= -1

cur_index <- RPiCam_scanPointer()
im <- matrix(RPiCam_readMemory(n0)$image,n0,im_size,byrow = T)
# im <- t(apply(im, 1, cumsum))
pca <- prcomp(im[(1:n0),])
xbar <- pca$center
# print(all(xbar!= FALSE))
# print(xbar[1:50])
pca <- list(values = pca$sdev[1:q]^2, vectors = pca$rotation[,(1:q)])



while (1) {
    Sys.sleep(0.001)
    cur_index <- RPiCam_scanPointer()
    if (cur_index == last_index) {
        next
    } else {
        last_index = cur_index
        im <- matrix(RPiCam_readMemory(1)$image,1,im_size,byrow=T)
        
        xbar <- updateMean(xbar,im[1,],i-1)
        # print(xbar[1:50])
        pca <- incRpca(pca$values,pca$vectors,im[1,],i-1,q=q,center = xbar)
        print(paste("PCA values: ",pca$values))
        x_hat<- tcrossprod(sqrt(pca$values),pca$vectors)
        x_hat <- scale(x_hat,center= -1*xbar,scale = F)
        # print(paste("x_hat values: ",x_hat[,1:50]))
        print(x_hat[1:50])
        i = i + 1
    }
}

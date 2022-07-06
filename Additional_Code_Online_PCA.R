library(onlinePCA)
library(tictoc)
library(pryr)
scale = 2
w <- 320*scale
h <- 240*scale
c <- 1
im_size <- w*h*c

# Rectangle
# image1 <- matrix(0,nrow = w,ncol = h)
# image1[50:(w-50),50:(h-50)] <- 1
# image(image1,col = grey.colors(256))

# Horizontal Cross
rec_width <- 50
rec_height <- 100
image1 <- matrix(0,nrow= w,ncol=h)
for (i in seq(w)) {
  for (j in seq(h)) {
    if (abs(i - j) < 20 || abs((w-i-j)) < 20) {
      image1[i,j] <- 1
    }
  }
}
# image(image1,col=grey.colors(256))



# Circle
image2 <- matrix(0, nrow = w,ncol = h)
for (i in (seq(w))) {
    for (j in seq(h)) {
        if ((i-w/2)^2/(w^2)+(j-h/2)^2/(h^2) < 0.1) {
            image2[i,j] <- 1
        }
    }
}
# image(image2,col = grey.colors(256))

# # Vertical Cross
# rec_width <- 125
# rec_height <- 50
# image2 <- matrix(0,nrow= w,ncol=h)
# image2[rec_width:(w-rec_width),rec_height:(h-rec_height)] <- 1
# image(image2,col=grey.colors(256))




princomp <- 10
n0 <- 2

im <- matrix(as.vector(image2),nrow=n0,ncol=im_size,byrow = T)
pca_val <- prcomp(im[(1:n0),], center = F)






# im <- t(apply(im, 1, cumsum))
# pca_val <- prcomp(im[(1:n0),], center = F)
# xbar <- pca_val$center
# print(all(xbar!= FALSE))
# print(xbar[1:50])
# pca$sdev[1:n0]
# pca$rotation[,(1:n0)]
pca <- list(values = pca_val$sdev[1:n0]^2, vectors = pca_val$rotation[,(1:n0)])

# pca$values <- c(init$values,rep(0,q-1))
# pca$rotation = matrix(0,im_size,q)

im2 <- matrix(as.vector(image2),nrow = 1, ncol= im_size,byrow=T)
i = 2

# for (i in 2:10) {
#   pca <- incRpca(pca$values,pca$vectors,as.vector(image2),f = 1-(1/(i+1)) ,q = i+1)
#   x_hat <- tcrossprod(sqrt(pca$values),pca$vectors)
#   image(abs(matrix(x_hat,h,w,byrow = T)),col = grey.colors(256))
#   i = i+1
# }


# incremental PCA Vectorised

pca <- incRpca(pca$values,pca$vectors,as.vector(image2),f = 1-(1/(i+1)) ,q = i+1)
x_hat <- tcrossprod(sqrt(pca$values),pca$vectors)
image(abs(matrix(x_hat,h,w,byrow = T)),col = grey.colors(256))
i = i+1


pca <- incRpca(pca$values,pca$vectors,as.vector(image2),i,q = i+1)
pca$values

new = 1
newim <- rbind(im,matrix(as.vector(image2),nrow=new ,ncol=im_size,byrow = T))
pca_val <- prcomp(newim,center = F)
x_hat <- tcrossprod(pca_val$sdev[1:(min(2+new,2))],pca_val$rotation[,(1:min((2+new),2))])
image(abs(matrix(x_hat,w,h)),col = grey.colors(256))
new = new+1
# }


# while (1) {
#     Sys.sleep(0.001)
#     cur_index <- RPiCam_scanPointer()
#     if (cur_index == last_index) {
#         next
#     } else {
#         last_index = cur_index
#         im <- matrix(RPiCam_readMemory(1)$image,1,im_size,byrow=T)
#
#         xbar <- updateMean(xbar,im[1,],i-1)
#         # print(xbar[1:50])
#         pca <- incRpca(pca$values,pca$vectors,im[1,],i-1,q=q,center = xbar)
#         print(paste("PCA values: ",pca$values))
#         x_hat<- tcrossprod(sqrt(pca$values),pca$vectors)
#         x_hat <- scale(x_hat,center= -1*xbar,scale = F)
#         # print(paste("x_hat values: ",x_hat[,1:50]))
#         print(x_hat[1:50])
#         i = i + 1
#     }
# }





# Block PCA with compression
scale = 2
w <- 320*scale
h <- 240*scale
c <- 1
im_size <- w*h*c

# Rectangle
# image1 <- matrix(0,nrow = w,ncol = h)
# image1[50:(w-50),50:(h-50)] <- 1
# image(image1,col = grey.colors(256))

# Horizontal Cross
rec_width <- 50
rec_height <- 100
image1 <- matrix(0,nrow= w,ncol=h)
for (i in seq(w)) {
  for (j in seq(h)) {
    if (abs(i - j) < 20 || abs((w-i-j)) < 20) {
      image1[i,j] <- 1
    }
  }
}
# image(image1,col=grey.colors(256))



# Circle
image2 <- matrix(0, nrow = w,ncol = h)
for (i in (seq(w))) {
  for (j in seq(h)) {
    if ((i-w/2)^2/(w^2)+(j-h/2)^2/(h^2) < 0.1) {
      image2[i,j] <- 1
    }
  }
}
X <- image2
tic("Incremental PCA")
svd <- svd(image2)
pca_block <- list(values = (svd$d), vectors = svd$u)
i = 2
max_pc <- 10

# US <- image2 %*% pca_block$vectors

# image(abs(svd$u %*% diag(svd$d) %*% t(svd$v)),col = grey.colors(256))


while (i < 10) {
  pca_block <<- incRpca.block(x = image1, lambda = pca_block$values, U = pca_block$vectors, f = 0.5)
  U <- pca_block$vectors[,1:max_pc]
  SV <- crossprod(U,image1)

  # print(pca_block$values[1:max_pc])
  x_hat<-abs(U%*%SV)
  image(x_hat,col = grey.colors(256))
  print(sqrt(sum((x_hat-image1)^2)))
  i = i+1
}
toc()


# Changed
tic("PCA original")
last_image <- image2
i <- 1
ff <- 0.9
col <- dim(image2)[2]
while(i < 10) {
  # last_image <- cbind(last_image,image1)
  res <- svd(image1)
  # res <- svd(image1)
  SV <- crossprod(res$u[,1:max_pc],image1)
  dim(res$u)

  # SV <- diag(res$d[1:max_pc])%*%res$v[1:col,]

  # Vprime <- res$v[1:col,]
  # V<-Vprime[,1:max_pc]
  # US <- res$u%*%diag(ff*res$d[1:max_pc])
  # SV <- crossprod(res$u[,1:max_pc],image1)
  x_hat <- abs(U%*%SV)
  image(x_hat,col=grey.colors(256))
  print(sqrt(sum((x_hat-image1)^2)))
  last_image <- image1
  i = i+1
}
toc()
image(abs(tcrossprod(US,V)),col = grey.colors(256))



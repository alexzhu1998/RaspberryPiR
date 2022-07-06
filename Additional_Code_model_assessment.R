library(ggplot2)
require(gridExtra)
# scaleFUN <- function(x) print(formatC(signif(x,digits=3), digits=3,format="fg", flag="#"))
library(grid)
library(plotly)

ggplotRun<- function (seq,mean,mem,recons = NULL,title, param) {
  plot1 <- ggplot(data = data.frame(s = seq,m = mean)) +
    geom_line(aes(x = s, y = m)) +
    theme(axis.title.x=element_blank(),
          axis.text.x=element_blank(),
          axis.ticks.x=element_blank())+
    # xlab(paste("Sequence of",param ,"from", seq[1], "to", seq[length(seq)])) +
    ylab("Avg Runtime (s)") +
    ggtitle(title)
  if (!is.null(recons)) {
    plot2 <- ggplot(data = data.frame(s = seq,m = mem/(10^6))) +
      geom_line(aes(s,m)) +
      theme(axis.title.x=element_blank(),
            axis.text.x=element_blank(),
            axis.ticks.x=element_blank())+
      ylab("Avg Memory (MB)")+
      ggtitle(title)

    plot3 <- ggplot(data = data.frame(s=seq,m=recons)) +
      geom_line(aes(x = s, y = m)) +
      xlab(paste("Sequence of",param ,"from", seq[1], "to", seq[length(seq)])) +
      ylab("Avg Error (SSE)") +
      ggtitle(title)

    grid.newpage()
    grid.draw(rbind(ggplotGrob(plot1),ggplotGrob(plot2),ggplotGrob(plot3),size = "last"))

  } else {
    plot2 <- ggplot(data = data.frame(s = seq,m = mem/(10^6))) +
      geom_line(aes(s,m)) +
      xlab(paste("Sequence of",param ,"from", seq[1], "to", seq[length(seq)])) +
      ylab("Avg Memory (MB)")
      ggtitle(title)

    grid.arrange(plot1, plot2, ncol=1)
  }

}

# Bayesian Changepoint Detection

Seq <- seq(50,1000,by = 50)

mean<-read.table(text =
"0.00525 0.01025 0.01850 0.03125 0.04300 0.05750 0.07425 0.09325 0.10775 0.12625 0.15975 0.18425 0.20950 0.28025 0.31025 0.36025 0.42775 0.46325 0.44150 0.48900"
)
mem <- read.table(text =
"31560664 31814160 32022392 32310868 32678812 33130332 33659744 34272612 34959268 35721940 36578448 37499824 38501152 39581656 40739180 41989892 43310804 44721812 46202484 47773228"
)

ggplotRun(Seq,stack(mean)$values,stack(mem)$values, recons = NULL,"Bayesian Changepoint Detection", param = "Length")

# Online PCA
Seq <- seq(10,200,by = 10)

mean <- read.table( text =
"1.08975 0.79500 0.75000 0.73775 0.75375 0.70825 0.70225 0.66750 0.64150 0.63275 0.59500 0.58100 0.54250 0.50700 0.45850 0.44375 0.38575 0.37225 0.35600 0.32650"
)

mem <- read.table (text =
"
75513004 75568788 76489720 76412568 76346484 76277892 76209268 76140424 76072016 76003312 75933340 75862880 75792480 75724100 75650608 75580128 75509648 75432984 75368704 74306084
")

recons_RSSE <- read.table (text =
"260.7126  226.9395  207.2743  273.5229  316.1717  330.0927  390.4870 511.8290  569.1922  657.0627  782.0243 1030.9080 1273.2122 1508.4260 1798.8773 1998.8538 2557.4304 3280.3043 4439.4792 6002.4220"
)

ggplotRun(Seq,rev(stack(mean)$values),rev(stack(mem)$values),rev(stack(recons_RSSE)$values),"Online Principal Components", param = "PC")



# RSVD Algorithm Test

library(ggplot2)
# Testing RSVD versus normal SVD
Len <- seq(50,1000,by = 50)
max_pc <- c(10,30,50)
RSVD <- SVD <- PCA <- array(0,dim=c(length(Len),3,length(max_pc)))
for (j in seq(length(max_pc))){
  print(paste("Max PC: ",max_pc[j]))
  for (k in seq(length(Len))) {
    set.seed(1);n <- m <- Len[k]
    X <- matrix(runif(n*m,0,256), nrow= n, ncol = m)


    gc1 <- gc(reset = TRUE)

    tic("RSVD")
    QR_X <- qr(X)
    Q <- qr.Q(QR_X)
    R <- qr.R(QR_X)[,1:max_pc[j]]
    svd_R <- svd(R)
    U1 <- svd_R$u
    U <- Q%*%U1
    tmp<- toc()

    gc2 <- gc()
    RSVD[k,3,j] <- sum(gc2[,6]-gc1[,2])

    RSVD[k,2,j] <- tmp$toc-tmp$tic
    RSVD[k,1,j]<- norm(U%*%t(U)%*%X-X, type = "F")



    gc1 <- gc(reset = TRUE)
    tic("SVD")
    svd_list <- svd(X,max_pc[j],0)
    tmp <- toc()
    gc2 <- gc()
    SVD[k,3,j] <- sum(gc2[,6]-gc1[,2])
    SVD[k,1,j] <- norm(svd_list$u %*% t(svd_list$u) %*% X-X,type= "F")
    SVD[k,2,j] <- tmp$toc - tmp$tic


    gc1 <- gc(reset = TRUE)
    tic("PCA")
    xbar <- colMeans(X)
    pca <- prcomp(X, center = T)
    tmp <- toc()
    gc2 <- gc()
    PCA[k,3,j] <- sum(gc2[,6]-gc1[,2])


    PCA[k,1,j] <- norm(scale(pca$x[,(1:max_pc[j])] %*% t(pca$rotation[,(1:max_pc[j])]),center= -xbar,scale=F) - X,type = "F")
    PCA[k,2,j] <- tmp$toc-tmp$tic
    # norm(U - svd(X)$u)
  }
}
n<-length(Len)
Len <- Len[2:(n-1)]
PCA <- PCA[2:(n-1),,]
SVD <- SVD[2:(n-1),,]
RSVD <- RSVD[2:(n-1),,]
SVD[,1,]
PCA[,1,]
RSVD[,1,]
performance <- rlang::env(
  RSVD = RSVD,
  SVD = SVD,
  PCA = PCA,
  RSVD.df = NULL,
  SVD.df = NULL,
  PCA.df = NULL
)

library(dplyr)
for (col in c("RSVD","SVD","PCA")) {
  temp_df<- as.data.frame.table(performance[[col]])
  levels(temp_df$Var1) <- Len
  levels(temp_df$Var2) <- c("Error (Frobenius) (10^4)","Time (s)","Memory (MB)")
  levels(temp_df$Var3) <- paste("Max PC:",max_pc)
  temp_df$name <- col
  performance[[paste0(col,".df")]] <- temp_df
}
plot_df <- rbind(performance[["RSVD.df"]],performance[["SVD.df"]],performance[["PCA.df"]])

colnames(plot_df) <- c("Length","Metric","Max_PC","Value","Method")
set.seed(1)
plot_df <- plot_df %>%
  mutate(Value = ifelse(Metric == "Memory (MB)",Value/1e6,Value)) %>%
  mutate(Value = ifelse(Metric == "Error (Frobenius) (10^4)",Value/1e4,Value))


plot_df
factor_var <- c("Length","Metric","Max_PC","Method")
plot_df[factor_var] <- lapply(plot_df[factor_var] ,  as.factor)

ggplot(data = plot_df, aes(col = Method)) +
  geom_point(aes(x = Length,y = Value),alpha=0.5)+
      facet_grid(cols = vars(Max_PC),rows = vars(Metric), scales="free")+
  scale_x_discrete(breaks = c(100,500,900))

#
# ind<-1
# plot_df <- data.frame(matrix(NA,1,3))
# colnames(plot_df) <- c("name","variable","value","")
# for (col in c("RSVD","SVD","PCA")) {
#   colnames(performance[[col]]) <- c("Frobenius","Time","Space")
#   plot_df <- rbind(plot_df,reshape2::melt(data.frame(performance[[col]], name = col)))
# # plot_df <- rbind(plot_df,)
# }
# plot_df<-plot_df[-1,]
#
#
# d <- data.frame(Len=Len,RSVD = RSVD, SVD = SVD, PCA = PCA)
# d <- reshape2::melt(d,id.vars = "Len")
#
# ggplot(data = d, aes(Len,value, col=variable)) +
#   geom_line()
# ind <- ind+1
#
#

# Control Charts

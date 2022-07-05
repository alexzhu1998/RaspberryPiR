library(RaspberryPiR)

# Initialise by storing the first 5 points in sorted order
counter = 1
last_index = -1
q_ord <- rep(0,5)
n_vec <- 1:5
p = 0.5
n_prime <- c(1,1+2*p,1+4*p,3+2*p,5)
dn_prime <- c(0,p/2,p,(1+p)/2,1)
while (counter <= 5) {
    cur_index <- DHT11_scanPointer()
    if (cur_index == last_index) {
        next
    } else {
        last_index = cur_index
        var <- DHT11_readMemory(1)
        q_ord[counter] = var$temperature[1]
        counter = counter + 1
    }
}
q_ord <- sort(q_ord)


P2_algo <- function (new_dat,q, n,n_p) {
    if (new_dat < q[1]) {
        q[1] = new_dat
        k <- 1
    } else if (new_dat < q[2]) {
        k <- 1
    } else if (new_dat < q[3]) {
        k <- 2
    } else if (new_dat < q[4]) {
        k <- 3
    } else if (new_dat < q[5]) {
        k <- 4
    } else {
        q[5] = new_dat
        k <- 4
    }
    for (i in ((k+1):5)) {
        n[i] = n[i]+1
    }
    # dn_prime is global
    for (i in (1:5)) {
        n_p[i] = n_p[i] + dn_prime[i]
    }
    
    for (i in (2:4)) {
        d <- n_p[i] - n[i]
        if ((d >= 1) && (n[i+1]-n[i] > 1) ||
            (d <= -1) && (n[i-1]-n[i] < -1)
        ) {
            d = sign(d)
            q_prime <- q[i] + (d/(n[i+1]-n[i-1])) * 
                ((n[i]-n[i-1]+d)*(q[i+1]-q[i])/(n[i+1]-n[i]) 
                 + (n[i+1]-n[i]-d)*(q[i]-q[i-1])/(n[i]-n[i-1]))
            if (!(q[i-1] < q_prime && q_prime < q[i+1])) {
                q_prime <- q[i] + d*(q[i+d]-q[i])/(n[i+d]-n[i])
            }
            q[i] <- q_prime
            n[i] = n[i] + d
        }
    }
    eval.parent(substitute(q_ord<-q))
    eval.parent(substitute(n_vec<-n))
    eval.parent(substitute(n_prime<-n_p))
    return (q[3])
}

while (1) {
    cur_index <- DHT11_scanPointer()
    if (cur_index == last_index) {
        next
    } else {
        var <- DHT11_readMemory(1)

        median <- P2_algo(var$temperature,q_ord,n_vec,n_prime)
        print(paste("Median: ", median))
        last_index = cur_index
        counter = counter+1
    }
}



countMinSketch <- function() {
    # Use of hash tables to store frequencies of data and measure the minimum collision 
    # for a specific value
}
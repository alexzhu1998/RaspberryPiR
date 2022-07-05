pkgname <- "RaspberryPiR"
source(file.path(R.home("share"), "R", "examples-header.R"))
options(warn = 1)
library('RaspberryPiR')

base::assign(".oldSearch", base::search(), pos = 'CheckExEnv')
base::assign(".old_wd", base::getwd(), pos = 'CheckExEnv')
cleanEx()
nameEx("RaspberryPiR-package")
### * RaspberryPiR-package

flush(stderr()); flush(stdout())

### Name: RaspberryPiR-package
### Title: A short title line describing what the package does
### Aliases: RaspberryPiR-package RaspberryPiR
### Keywords: package

### ** Examples

  ## Not run: 
##D      ## Optional simple examples of the most important functions
##D      ## These can be in \dontrun{} and \donttest{} blocks.   
##D   
## End(Not run)



cleanEx()
nameEx("rcpp_hello_world")
### * rcpp_hello_world

flush(stderr()); flush(stdout())

### Name: rcpp_hello_world
### Title: Simple function using Rcpp
### Aliases: rcpp_hello_world

### ** Examples

## Not run: 
##D rcpp_hello_world()
## End(Not run)



### * <FOOTER>
###
cleanEx()
options(digits = 7L)
base::cat("Time elapsed: ", proc.time() - base::get("ptime", pos = 'CheckExEnv'),"\n")
grDevices::dev.off()
###
### Local variables: ***
### mode: outline-minor ***
### outline-regexp: "\\(> \\)?### [*]+" ***
### End: ***
quit('no')

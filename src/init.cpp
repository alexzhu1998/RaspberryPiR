// Generated by the rstantools package


#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Visibility.h>
#include <Rversion.h>



void attribute_visible R_init_RaspberryPiR(DllInfo *dll) {
  // next line is necessary to avoid a NOTE from R CMD check
  R_registerRoutines(dll, NULL, NULL, NULL, NULL);
  R_useDynamicSymbols(dll, TRUE); // necessary for .onLoad() to work
}

// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// exec
std::string exec(const char* cmd);
RcppExport SEXP _RaspberryPiR_exec(SEXP cmdSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const char* >::type cmd(cmdSEXP);
    rcpp_result_gen = Rcpp::wrap(exec(cmd));
    return rcpp_result_gen;
END_RCPP
}
// fibonacci_main
int fibonacci_main(int x);
RcppExport SEXP _RaspberryPiR_fibonacci_main(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(fibonacci_main(x));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_hello_world
List rcpp_hello_world();
RcppExport SEXP _RaspberryPiR_rcpp_hello_world() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(rcpp_hello_world());
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_RaspberryPiR_exec", (DL_FUNC) &_RaspberryPiR_exec, 1},
    {"_RaspberryPiR_fibonacci_main", (DL_FUNC) &_RaspberryPiR_fibonacci_main, 1},
    {"_RaspberryPiR_rcpp_hello_world", (DL_FUNC) &_RaspberryPiR_rcpp_hello_world, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_RaspberryPiR(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}

#include <Rcpp.h>
#include <wiringPi.h>
#include <string>
#include <vector>


#define PinStart    0
#define PinEnd      31


void WarningHelperExceedRange(int val, int start, int end, std::string function_name) {
    if (val < start || val > end) {
        Rcpp::stop("Input %s greater than %i or less than %i",function_name,end,start);
    }
}

int WarningHelperStringNotMatch(Rcpp::StringVector& val, std::vector<std::string> v,std::string function_name) {
    int flag = 0;
    int i = 1;
    for (std::string s: v) {
        if (val[0] == s) {
            flag = i;
            break;
        }
        ++i;
    }
    if (flag) {
        return flag;
    } else {
        Rcpp::stop("Input %s does not match with source",function_name);
        return 0;
    }
}

void WarningHelperIsBoolean(int val, std::string function_name) {
    if (val != 0 && val != 1) 
        Rcpp::stop("Input %s is not Logical",function_name);
}

//' Dummy Docs
//' 
//' This function returns XYZ. 
//' BLAHBLAH
//'
//' @param BLAH
//' @export
// [[Rcpp::export]]
void LL_wiringPiSetup(){
    wiringPiSetup();
}

//' Dummy Docs
//' 
//' This function returns XYZ. 
//' BLAHBLAH
//'
//' @param BLAH
//' @export
// [[Rcpp::export]]
void LL_pinMode(Rcpp::IntegerVector pin, Rcpp::StringVector mode){
    std::string function_name = "pinMode";
    WarningHelperExceedRange(pin[0],PinStart,PinEnd,function_name);
    std::vector<std::string> string_map = {"OUTPUT","INPUT","PWM_OUTPUT","GPIO_CLOCK"};
    std::vector<int> output_map = {OUTPUT,INPUT,PWM_OUTPUT,GPIO_CLOCK};
    
    int res = WarningHelperStringNotMatch(mode,string_map,function_name);
    if (res != 0) {
        pinMode(pin[0],output_map[res-1]);
    }
    
}

//' Dummy Docs
//' 
//' This function returns XYZ. 
//' BLAHBLAH
//'
//' @param BLAH
//' @export
// [[Rcpp::export]]
void LL_digitalWrite(Rcpp::IntegerVector pin, Rcpp::LogicalVector mode){
    std::string function_name = "digitalWrite";
    WarningHelperExceedRange(pin[0],PinStart,PinEnd,function_name);
    WarningHelperIsBoolean(mode[0],function_name);
    digitalWrite(pin[0],mode[0]);
}

//' Dummy Docs
//' 
//' This function returns XYZ. 
//' BLAHBLAH
//'
//' @param BLAH
//' @export
// [[Rcpp::export]]
Rcpp::IntegerVector LL_digitalRead(Rcpp::IntegerVector pin){
    std::string function_name = "digitalRead";
    WarningHelperExceedRange(pin[0],PinStart,PinEnd,function_name);
    Rcpp::IntegerVector x = Rcpp::IntegerVector::create(-1);
    
    int y = digitalRead(pin[0]);
    x[0] = y;
    return x;
}

//' Dummy Docs
//' 
//' This function returns XYZ. 
//' BLAHBLAH
//'
//' @param BLAH
//' @export
// [[Rcpp::export]]
void LL_pullUpDnControl(Rcpp::IntegerVector pin, Rcpp::StringVector pud) {
    std::string function_name = "pullUpDnControl";
    WarningHelperExceedRange(pin[0],PinStart,PinEnd,function_name);
    std::vector<std::string> string_map = {"PUD_OFF","PUD_DOWN","PUD_UP"};
    std::vector<int> output_map = {PUD_OFF,PUD_DOWN,PUD_UP};

    int res = WarningHelperStringNotMatch(pud,string_map,function_name);
    if (res!= 0) {
        pullUpDnControl(pin[0],output_map[res-1]);
    }
}

//' Dummy Docs
//' 
//' This function returns XYZ. 
//' BLAHBLAH
//'
//' @param BLAH
//' @export
// [[Rcpp::export]]
Rcpp::NumericVector LL_millis() {
    double x = (double) millis();
    return Rcpp::NumericVector::create(x);
}

//' Dummy Docs
//' 
//' This function returns XYZ. 
//' BLAHBLAH
//'
//' @param BLAH
//' @export
// [[Rcpp::export]]
Rcpp::NumericVector LL_micros() {
    double x = (double) micros();
    return Rcpp::NumericVector::create(x);
}

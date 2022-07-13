#include "utils/DHT11.h"
#include "utils/PhotoRes.h"
#include "utils/RPiCam.h"
#include "utils/MQ2.h"

// [[Rcpp::export]]
Rcpp::List testingDHT (Rcpp::NumericVector pin = 0) {
    DHT11_Operator dht(pin[0]);
    int chk;
    Rcpp::Rcout << "Hello!, Program is starting..." << std::endl;

    Rcpp::CharacterVector t = Rcpp::CharacterVector::create(dht.to_time_string(dht.get_raw_time()));
    
    chk = dht.readDHT11();
    if (chk == DHTLIB_OK) {
        Rcpp::Rcout << "DHT11, OK!" << std::endl;
        Rcpp::Rcout << "Humidity is " << dht.humidity << "%," << "\t Temperature is " << dht.temperature << "*C" << std::endl;
    } else if (chk == DHTLIB_ERROR_CHECKSUM){
        Rcpp::Rcout << "DHT11,NOT OK! ERROR CHECKSUM" << std::endl;
    } else if (chk == DHTLIB_ERROR_TIMEOUT) {
        Rcpp::Rcout << "DHT11,NOT OK! ERROR TIMEOUT" << std::endl;
    } else {
        Rcpp::Rcout << "DHT11,NOT OK! ERROR UNKNOWN" << std::endl;
    }
    return Rcpp::List::create(
        Rcpp::Named("datetime") = t,
        Rcpp::Named("temperature") = dht.temperature,
        Rcpp::Named("humidity") = dht.humidity
    );
}

// [[Rcpp::export]]
Rcpp::List testingPhotoRes (Rcpp::NumericVector pin = 7) {
    PhotoRes_Operator phoR(pin[0]);
    int chk;
    Rcpp::Rcout << "Hello!, Program is starting..." << std::endl;
    
    Rcpp::CharacterVector t = Rcpp::CharacterVector::create(phoR.to_time_string(phoR.get_raw_time()));
    chk = phoR.readPhotoRes();
    if (chk == PHOR_SENSOR_OK) {
        Rcpp::Rcout << "Photo Resistor, OK!" << std::endl;
        Rcpp::Rcout << "Time to charge is " << phoR.PhoR_time_to_charge << " ms"<< std::endl;
    } else {
        Rcpp::Rcout << "Photo Resistor, NOT OK! ERROR UNKNOWN" << std::endl;
    }
    return Rcpp::List::create(
        Rcpp::Named("datetime") = t,
        Rcpp::Named("time to charge") = Rcpp::NumericVector::create(phoR.PhoR_time_to_charge)
    );
}

// [[Rcpp::export]]
Rcpp::List testingMQ2 (Rcpp::NumericVector pin = 0,Rcpp::NumericVector SPICLK = 14, Rcpp::NumericVector SPIMISO = 13, Rcpp::NumericVector SPIMOSI = 12, Rcpp::NumericVector SPICS = 10, Rcpp::NumericVector mq2_dpin= 25, Rcpp::NumericVector mq2_apin= 0) {
    MQ2_Operator mq2(SPICLK[0], SPIMISO[0], SPIMOSI[0], SPICS[0], mq2_dpin[0], mq2_apin[0]);
    int chk;
    Rcpp::Rcout << "Hello!, Program is starting..." << std::endl;

    Rcpp::CharacterVector t = Rcpp::CharacterVector::create(mq2.to_time_string(mq2.get_raw_time()));
    
    chk = mq2.readMQ2();
    if (chk == MQ2_SENSOR_OK) {
        Rcpp::Rcout << "MQ2, OK!" << std::endl;
        Rcpp::Rcout << "Time to charge is " << mq2.MQ2_time_to_charge << " ms"<< std::endl; 
        Rcpp::Rcout << "Gas Leak: (1 is leak)" << mq2.leak << std::endl;
    }
    return Rcpp::List::create(
        Rcpp::Named("datetime") = t,
        Rcpp::Named("time_to_charge") = Rcpp::NumericVector::create(mq2.MQ2_time_to_charge),
        Rcpp::Named("leak") = Rcpp::NumericVector::create(mq2.leak)
    );
}


// [[Rcpp::export]]
Rcpp::List simpleCapture(Rcpp::CharacterVector path = "/home/pi/Pictures/raspicam_image.ppm", Rcpp::NumericVector width = 1, Rcpp::NumericVector height = 1) {
    RPiCam_Operator rpicam = RPiCam_Operator(raspicam::RASPICAM_FORMAT_GRAY,width[0],height[0],Rcpp::as<std::string>(path[0]));
	raspicam::RaspiCam Camera;
    rpicam.initiate_camera(Camera);
    
    Rcpp::Rcout << "Camera Information:" << std::endl;
    Rcpp::Rcout << "Width: "<< Camera.getWidth() << " Height: " << Camera.getHeight() << std::endl;
	Rcpp::Rcout << "ImageBufferSize: " << rpicam.len << std::endl;
    
    rpicam.capture(Camera);
    
    
    Rcpp::CharacterVector t = Rcpp::CharacterVector::create(rpicam.to_time_string(rpicam.get_raw_time()));
    
    Rcpp::IntegerVector res (rpicam.v.begin(),rpicam.v.end());
    Rcpp::Rcout << rpicam.v.size() << std::endl;
    // rpicam.saveImage(Camera);
    
    delete rpicam.data;
    return Rcpp::List::create(
        Rcpp::Named("datetime") = t,
        Rcpp::Named("image") = res
    );
}
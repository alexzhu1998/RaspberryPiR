#include "MQ2.h"

MQ2_Operator::MQ2_Operator(int _SPICLK,
        int _SPIMISO,
        int _SPIMOSI,
        int _SPICS,
        int _mq2_dpin,
        int _mq2_apin
    ) : 
    SPICLK(_SPICLK),
    SPIMISO(_SPIMISO),
    SPIMOSI(_SPIMOSI),
    SPICS(_SPICS),
    mq2_dpin(_mq2_dpin),
    mq2_apin(_mq2_apin)
    {
    wiringPiSetup();
    pinMode(SPIMOSI,OUTPUT);
    pinMode(SPIMISO,INPUT);
    pinMode(SPICLK,OUTPUT);
    pinMode(SPICS,OUTPUT);
    pullUpDnControl(mq2_dpin,PUD_DOWN);
}

int readadc(int adcnum, int clockpin, int mosipin, int misopin, int cspin) {
    if ((adcnum > 7) || (adcnum < 0)) {
        return -1;
    }
    digitalWrite(cspin,HIGH);
    
    digitalWrite(clockpin,LOW);
    digitalWrite(cspin,LOW);
    
    int commandout = adcnum;
    commandout |= 0x18;
    commandout <<= 3;

    for (int i = 0 ; i < 5; i++) {
        if (commandout & 0x80) {
            digitalWrite(mosipin,HIGH);
        } else {
            digitalWrite(mosipin,LOW);
        }
        commandout <<= 1;
        digitalWrite(clockpin,HIGH);
        digitalWrite(clockpin,LOW);
    }
    int adcout = 0;

    for (int i = 0; i < 12; i++) {
        digitalWrite(clockpin,HIGH);
        digitalWrite(clockpin,LOW);
        adcout <<= 1;
        if (digitalRead(misopin)) {
            adcout |= 0x1;
        }
    }
    digitalWrite(cspin,HIGH);
    adcout >>= 1;
    return adcout;


}

int MQ2_Operator::readMQ2() {
    int COlevel = readadc(mq2_apin,SPICLK,SPIMOSI,SPIMISO,SPICS);
    if (digitalRead(mq2_dpin) == HIGH) {
        leak = 0;
    } else {
        leak = 1;
    }
    MQ2_time_to_charge = (double)COlevel/1024*3.3;
    delay(500);
    return MQ2_SENSOR_OK;
}

void MQ2::writeMemory(int _SPICLK,
        int _SPIMISO,
        int _SPIMOSI,
        int _SPICS,
        int _mq2_dpin,
        int _mq2_apin
) {
    SharedMemory sharedmem(MQ2_SHM_PATH,MQ2_SHM_PTR_PATH,SHM_WRITE,DATA_MQ2);
    DataPtr* data_ptr = sharedmem.dp;
    DataBlock2* data_obj = sharedmem.db2;

    int i = 0;
    int chk;
    TimeVar start;
    MQ2_Operator mq2(_SPICLK,_SPIMISO,_SPIMOSI, _SPICS, _mq2_dpin, _mq2_apin);

    for (;!pending_interrupt();++i) {
        if (i == data_ptr->block_length) {
            i = 0;
            data_ptr->complete = true;
        }
        Rcpp::Rcout <<  "Block No: " << data_ptr->cur_index << std::endl;
        start = timeNow();
        // assigning values
        chk = mq2.readMQ2();
        data_obj->raw_time[i] = get_raw_time();
        data_obj->data1[i] = static_cast<double>(mq2.MQ2_time_to_charge);
        data_obj->data2[i] = static_cast<double>(mq2.leak);
        
        double elapsed = intervalDuration(timeNow()-start);
        Rcpp::Rcout << "Time to charge is " << mq2.MQ2_time_to_charge << " ms"<< std::endl; 
        Rcpp::Rcout << "Gas Leak: (1 is leak)" << mq2.leak << std::endl;
        millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        data_ptr->cur_index = i;
        
    }
    Rcpp::Rcout << "End Writing" << std::endl;

    Rcpp::Rcout << "Freeing Memory" << std::endl;
    sharedmem.freeMemory();
}

Rcpp::List MQ2::readMemory(int n) {
    SharedMemory sharedmem(MQ2_SHM_PATH,MQ2_SHM_PTR_PATH,SHM_READ,DATA_MQ2);
    DataPtr* data_ptr = sharedmem.dp;
    DataBlock2* data_obj = sharedmem.db2;
    Rcpp::CharacterVector datetime;
    Rcpp::NumericVector time_to_charge;
    Rcpp::NumericVector leak;

    int cur = data_ptr->cur_index;
    int block_length = data_ptr->block_length;
    
    // Rcpp::Rcout << cur << std::endl;
    for (int i =0 ; i < n;++i) {
        if (cur-i < 0) {
            if (data_ptr->complete) {
                cur = block_length+i-1;
            } else {
                Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                break;
            }
        }
        
        std::string t = to_time_string(data_obj->raw_time[cur-i]);
        
        datetime.push_back(t);
        time_to_charge.push_back(static_cast<double>(data_obj->data1[cur-i]));
        leak.push_back(static_cast<double>(data_obj->data2[cur-i]));
    }

    // sharedmem.unmap_DataPtr(data_ptr);
    // sharedmem.unmap_DataBlock2(data_obj);
    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("time_to_charge") = time_to_charge,
        Rcpp::Named("leak") = leak
    ); 
}
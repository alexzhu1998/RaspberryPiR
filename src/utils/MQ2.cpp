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
    MQ2_time_to_charge = (((double)COlevel/1024)*3.3);
    delay(500);
    return MQ2_SENSOR_OK;
}

void MQ2::writeMemory(int _SPICLK,
        int _SPIMISO,
        int _SPIMOSI,
        int _SPICS,
        int _mq2_dpin,
        int _mq2_apin,
        int len
) {
    SharedMemory sharedmem(MQ2_SHM_PATH,MQ2_SHM_PTR_PATH,SHM_WRITE);
    int n_datapts = 2;
    DataPtr source_dp(n_datapts,len);
    DataBlock source_db(n_datapts,len,REGULAR_SENSOR_TYPE);

    sharedmem.map_data_ptr(&source_dp);
    sharedmem.map_data_obj(source_dp.allocated_memory, &source_db);
    int i = 0, chk;
    TimeVar start;
    MQ2_Operator mq2(_SPICLK,_SPIMISO,_SPIMOSI, _SPICS, _mq2_dpin, _mq2_apin);

    for (;!pending_interrupt();++i) {
        if (i == sharedmem.dp->block_length) {
            i = 0;
            sharedmem.dp->complete = 1;
        }
        Rcpp::Rcout <<  "Block No: " << sharedmem.dp->cur_index << std::endl;
        start = timeNow();
        // assigning values
        chk = mq2.readMQ2();
        sharedmem.db->raw_time[i] = static_cast<time_t>(get_raw_time());
        Rcpp::Rcout << "Time: " << to_time_string(sharedmem.db->raw_time[i]) << std::endl;
        sharedmem.db->sensor_data[n_datapts*i] = static_cast<double>(mq2.MQ2_time_to_charge);
        sharedmem.db->sensor_data[n_datapts*i+1] = static_cast<double>(mq2.leak);
        
        double elapsed = intervalDuration(timeNow()-start);
        Rcpp::Rcout << "Time to charge is " << mq2.MQ2_time_to_charge << " ms"<< std::endl; 
        Rcpp::Rcout << "Gas Leak: (1 is leak)" << mq2.leak << std::endl;
        millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        sharedmem.dp->cur_index = i;
        
    }
    Rcpp::Rcout << "End Writing" << std::endl;

    Rcpp::Rcout << "Freeing Memory" << std::endl;
    sharedmem.freeMemory();
}

Rcpp::List MQ2::readMemory(int n) {
    SharedMemory sharedmem(MQ2_SHM_PATH,MQ2_SHM_PTR_PATH,SHM_READ);
    
    DataBlock* dbRead = new DataBlock(sharedmem.dp->num_data_points,sharedmem.dp->block_length,REGULAR_SENSOR_TYPE);
    sharedmem.retrieve_data_ptr(sizeof(DataPtr));
    Rcpp::Rcout << "Block Length: " << sharedmem.dp->block_length << " Number of Data Pts: " << sharedmem.dp->num_data_points << std::endl;
    Rcpp::Rcout << "Allocated Memory: "<< sharedmem.dp->allocated_memory << std::endl;
    sharedmem.retrieve_data_obj(sharedmem.dp->allocated_memory, dbRead); // retrieve memory from data block (which is dynamically assigned)
    Rcpp::Rcout << "Sensor data offset is " << sharedmem.db->sensor_data_offset << std::endl;
    int n_datapts = sharedmem.dp->num_data_points;
    // Checking error
    if (n > sharedmem.dp->block_length) {
        Rcpp::Rcout << "Error: Read Memory Block Length exceeded Write Memory Block Length" << std::endl; 
        return Rcpp::List::create(
            Rcpp::Named("Error") = Rcpp::NumericVector(1)
        );
    } else if (n <= 0) {
        Rcpp::Rcout << "Error: Read Memory Block Length negative" << std::endl; 
        return Rcpp::List::create(
            Rcpp::Named("Error") = Rcpp::NumericVector(1)
        );
    }

    int cur = sharedmem.dp->cur_index;
    
    if (cur - n < 0 && sharedmem.dp->complete == 0) {
        Rcpp::Rcout << "Error: Length of Read Memory Block exceeded current position" << std::endl;
        return Rcpp::List::create(
            Rcpp::Named("Error") = Rcpp::NumericVector(1)
        );
    }
    Rcpp::CharacterVector datetime;
    Rcpp::NumericVector time_to_charge;
    Rcpp::NumericVector leak;

    int i = cur - n;
    i += cur - n < 0 ? sharedmem.dp->block_length : 0;
    for (; i != cur ;++i) {
        if (i == sharedmem.dp->block_length) {
            if (sharedmem.dp->complete) {
                i = 0;
            } else {
                Rcpp::Rcout << "Reach end of memory_block, terminating..." << std::endl;
                break;
            }
        }
        
        time_t x = dbRead->raw_time[i];
        std::string t = to_time_string(x);
        
        datetime.push_back(t);
        time_to_charge.push_back(static_cast<double>(dbRead->sensor_data[n_datapts*i]));
        leak.push_back(static_cast<double>(dbRead->sensor_data[n_datapts*i+1]));
    }

    
    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("time_to_charge") = time_to_charge,
        Rcpp::Named("leak") = leak
    ); 
}

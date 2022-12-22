#include "DHT11.h"



DHT11_Operator::DHT11_Operator(int _pin): pin(_pin) {
    wiringPiSetup();
}

int DHT11_Operator::readSensor(int wakeupDelay) {
    int mask = 0x80;
    int idx = 0;
    int32_t t,loopCnt;
    int chk;
    // 5 bytes of data
    for (int i = 0; i < 5;i++)
        bits[i] = 0;
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);
    delay(500);
    // start signal
    digitalWrite(pin,LOW);
    delay(wakeupDelay);
    digitalWrite(pin,HIGH);
    pinMode(pin,INPUT);

    loopCnt = DHTLIB_TIMEOUT;
    t = micros();

    while (1) {
        if (digitalRead(pin) == LOW) break;
        if ((micros()-t)>loopCnt) return DHTLIB_ERROR_TIMEOUT;
    }

    ////
    //Tell the Sensor to wait
    loopCnt = DHTLIB_TIMEOUT;
    t = micros();
    // Wait for echo low level to end
    while (digitalRead(pin) == LOW) {
        if ((micros()-t)>loopCnt) return DHTLIB_ERROR_TIMEOUT;
    }

    loopCnt = DHTLIB_TIMEOUT;
    t = micros();
    // Wait for echo high level to end
    while (digitalRead(pin) == HIGH) {
        if ((micros()-t)>loopCnt) return DHTLIB_ERROR_TIMEOUT;
    }
    //End of Wait
    ////

    for (int i = 0; i < 40; i++) {

        ////
        //Tell the Sensor to wait
        loopCnt = DHTLIB_TIMEOUT;
        t = micros();
        // Wait for echo low level to end
        while (digitalRead(pin) == LOW) {
            if ((micros()-t)>loopCnt) return DHTLIB_ERROR_TIMEOUT;
        }

        loopCnt = DHTLIB_TIMEOUT;
        t = micros();
        // Wait for echo high level to end
        while (digitalRead(pin) == HIGH) {
            if ((micros()-t)>loopCnt) return DHTLIB_ERROR_TIMEOUT;
        }
        //End of Wait
        ////

        // If the signal is pulled down
        if ((micros() - t) > 60) {
            bits[idx] |= mask;
        }

        mask >>= 1;
        if (mask == 0) {
            mask = 0x80;
            idx++;
        }

    }
    pinMode(pin,OUTPUT);
    digitalWrite(pin,HIGH);
    return DHTLIB_OK;
}

int DHT11_Operator::readDHT11Once(){
    int rv ;
    uint8_t checksum;
    rv = readSensor(DHTLIB_DHT11_WAKEUP);
    if(rv != DHTLIB_OK){
        humidity = DHTLIB_INVALID_VALUE;
        temperature = DHTLIB_INVALID_VALUE;
        return rv;
    }
    humidity = bits[0];
    temperature = bits[2] + bits[3] * 0.1;
    checksum = bits[0] + bits[1] + bits[2] + bits[3];
    if(bits[4] != checksum)
        return DHTLIB_ERROR_CHECKSUM;
    return DHTLIB_OK;
}

int DHT11_Operator::readDHT11(){
	int chk = DHTLIB_INVALID_VALUE;
	for (int i = 0; i < 15; i++){
		chk = readDHT11Once();	//read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
		if(chk == DHTLIB_OK){
			return DHTLIB_OK;
		}
		delay(100);
	}
	return chk;
}


void DHT11::writeMemory(int pin, int len) {
    SharedMemory sharedmem(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH,SHM_WRITE);
    int n_datapts = 2;
    DataPtr source_dp(n_datapts,len);
    DataBlock source_db(n_datapts,len,REGULAR_SENSOR_TYPE);
    
    sharedmem.map_data_ptr(&source_dp);
    sharedmem.map_data_obj(source_dp.allocated_memory,&source_db);

    int i = 0, chk;
    TimeVar start;
    DHT11_Operator dht(pin);
    for (;!pending_interrupt();i++) {
        if (i == sharedmem.dp->block_length) {
            i = 0;
            sharedmem.dp->complete = 1;
        }
        Rcpp::Rcout <<  "Block No: " << i << std::endl;
        start = timeNow();
        // assigning values
        chk = dht.readDHT11();
        sharedmem.db->raw_time[i] = static_cast<time_t>(get_raw_time());
        Rcpp::Rcout << "Time: " << to_time_string(sharedmem.db->raw_time[i]) << std::endl;
        sharedmem.db->sensor_data[n_datapts*i] = static_cast<double>(dht.temperature);
        sharedmem.db->sensor_data[n_datapts*i+1] = static_cast<double>(dht.humidity);
        double elapsed = intervalDuration(timeNow()-start);
        Rcpp::Rcout << "Humidity is " << sharedmem.db->sensor_data[n_datapts*i+1]  << "%," << "\t Temperature is " << sharedmem.db->sensor_data[n_datapts*i]  << "*C" << std::endl;
        millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        sharedmem.dp->cur_index = i;
        
    }

    sharedmem.freeMemory();
}



Rcpp::List DHT11::readMemory(int n) {
    
    SharedMemory sharedmem(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH,SHM_READ);
    

    

    DataBlock* dbRead = new DataBlock(sharedmem.dp->num_data_points,sharedmem.dp->block_length,REGULAR_SENSOR_TYPE); // no need to delete because this gets overwritten by mmap
    sharedmem.retrieve_data_ptr(sizeof(DataPtr)); //retrieve memory from data ptr (which is statically assigned) 
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
    Rcpp::NumericVector temp;
    Rcpp::NumericVector hum;


    // for (int i =0; i < 50; i++) {
    //     Rcpp::Rcout << i << "Data points: " <<  dbRead->sensor_data[i]<< " " << dbRead->sensor_data << std::endl;
    //     // Rcpp::Rcout << (double)(*(dbRead->sensor_data + 5)) << std::endl;
    // }
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
        
        // Rcpp::Rcout << dbRead->raw_time[i] << " " << to_time_string(dbRead->raw_time[i]) <<" " << dbRead->sensor_data[n_datapts*i] << " " <<  dbRead->sensor_data[n_datapts*i+1] << std::endl;
        datetime.push_back(t);
        temp.push_back(static_cast<double>(dbRead->sensor_data[n_datapts*i]));
        hum.push_back(static_cast<double>(dbRead->sensor_data[n_datapts*i+1]));
    }
    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("temperature") = temp,
        Rcpp::Named("humidity") = hum
    ); 
}

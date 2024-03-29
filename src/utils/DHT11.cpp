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

void DHT11::writeMemory(int pin) {
    SharedMemory sharedmem(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH,SHM_WRITE,DATA_DHT11);
    DataPtr* data_ptr = sharedmem.dp;
    DataBlock2* data_obj = sharedmem.db2;
    
    int i = 0;
    int chk;
    TimeVar start;
    DHT11_Operator dht(pin);
    for (;!pending_interrupt();++i) {
        if (i == data_ptr->block_length) {
            i = 0;
            data_ptr->complete = true;
        }
        Rcpp::Rcout <<  "Block No: " << i << std::endl;
        start = timeNow();
        // assigning values
        chk = dht.readDHT11();
        data_obj->raw_time[i] = get_raw_time();
        data_obj->data1[i] = static_cast<double>(dht.temperature);
        data_obj->data2[i] = static_cast<double>(dht.humidity);
        double elapsed = intervalDuration(timeNow()-start);
        Rcpp::Rcout << "Humidity is " << data_obj->data2[i]  << "%," << "\t Temperature is " << data_obj->data1[i]  << "*C" << std::endl;
        millisleep((unsigned int)std::max((timeBetweenAcquisition-elapsed),(double)0));
        Rcpp::Rcout <<  "Time Elapsed: " << intervalDuration(timeNow()-start) << " ms"<< std::endl;
        data_ptr->cur_index = i;
        
    }
    Rcpp::Rcout << "End Writing" << std::endl;

    Rcpp::Rcout << "Freeing Memory" << std::endl;
    sharedmem.freeMemory();
}

Rcpp::List DHT11::readMemory(int n) {
    SharedMemory sharedmem(DHT11_SHM_PATH,DHT11_SHM_PTR_PATH,SHM_READ,DATA_DHT11);
    DataPtr* data_ptr = sharedmem.dp;
    DataBlock2* data_obj = sharedmem.db2;
    Rcpp::CharacterVector datetime;
    Rcpp::NumericVector temp;
    Rcpp::NumericVector hum;

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
        temp.push_back(static_cast<double>(data_obj->data1[cur-i]));
        hum.push_back(static_cast<double>(data_obj->data2[cur-i]));
    }

    // sharedmem.unmap_DataPtr(data_ptr);
    // sharedmem.unmap_DataBlock2(data_obj);
    return Rcpp::List::create(
        Rcpp::Named("datetime") = datetime,
        Rcpp::Named("temperature") = temp,
        Rcpp::Named("humidity") = hum
    ); 
}
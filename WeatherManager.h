#ifndef __WEATHER_MANAGER_H__
#define __WEATHER_MANAGER_H__

// #include "../ValueTracker.h"
#include "SHTSensor.h"
#include "Wire.h"

class WeatherManager {
    public:
        WeatherManager(float h, float t, float hi);
        bool init();
        bool update();
        // bool setAccuracy();

    private:
        ////////////////// Sensor /////////////////////////
        float sensor_active = false;
        SHTSensor sensor;

        /////////////////// Humidity //////////////////////
        float humid = 0.0;
        float last_humid = 0.0;
        float humid_high_thresh;
        // this will indicate if the WeatherManager recommends
        // a shutdown due to high humidity
        bool  humid_shutdown = false;

        /////////////////// Temperature ///////////////////
        float temp = 0.0;
        float last_temp = 0.0;
        float temp_high_thresh;
        // indicates how much historesis is applied to the
        // high temp threshold to return to normal operation
        // this indicates if a temperature shutdown is recommended
        float temp_historesis;
        bool temp_shutdown = false;
};


WeatherManager::WeatherManager(float h, float t, float hi) {
    humid_high_thresh = h;
    temp_high_thresh = t;
    temp_historesis = hi;
}


bool WeatherManager::init() {
    Wire.begin();
    if (sensor.init()) {
        Serial.println("SHT temp/humid sensor was initialised");
        sensor_active = true;
        return true;
    } else {
        for (int i = 0; i < 1000; i++) {
            Serial.println("ERROR, SHT init() failed, there will be no active temp/humid sensor");
            delay(500);
        }
        sensor_active = false;
        return false;
    }
}


bool WeatherManager::update(){
    // return false if no update is made and true 
    // if an update is made
    // check to see if the sensor is ready for a new reading
    if (sensor.readSample()) {
        last_humid = humid;
        humid = sensor.getHumidity();
        if (humid >= humid_high_thresh) {
            humid_shutdown = true;
            return true;
        }
        last_temp = temp;
        temp = sensor.getTemperature();
        if (temp >= temp_high_thresh && temp_shutdown == false) {
            temp_shutdown = true;
            return true;
        } else if (temp <= (temp_high_thresh * (1.0 - temp_historesis))){ 
            temp_shutdown = false;
        }
    } else {
        Serial.println("SHT sensor is not ready for a new reading, exiting update");
    }

    // if we make it this far then there are no emergency shudown
    // conditions and we can exit the program
    return false;
}

#endif // __WEATHER_MANAGER_H__

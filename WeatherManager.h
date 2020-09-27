#ifndef __WEATHER_MANAGER_H__
#define __WEATHER_MANAGER_H__

#include <ValueTracker.h>
#include <SHTSensor.h>
#include <Wire.h>

class WeatherManager {
    public:
        WeatherManager(float humid_high, float temp_high, float historesis, unsigned long update);
        bool init();
        bool update();
        bool getTempShutdown(){return temp_shutdown;};
        bool getHumidityShutdown(){return humid_shutdown;};
        float getTemperature() {return temp;};
        float getHumidity() {return humid;};

        void print();
        void setPrintReadings(bool p){p_readings = p;};

    private:

        bool p_readings = false;

        elapsedMillis last_reading_time;
        unsigned long update_delay;
        ////////////////// Sensor /////////////////////////
        float sensor_active = false;
        SHTSensor sensor;

        /////////////////// Humidity //////////////////////
        float humid = 0.0;
        // value tracker is set to not conduct a rolling average
        ValueTrackerFloat humid_tracker = ValueTrackerFloat(&humid, 1.0);

        float humid_high_thresh;
        // this will indicate if the WeatherManager recommends
        // a shutdown due to high humidity
        bool  humid_shutdown = false;

        /////////////////// Temperature ///////////////////
        float temp = 0.0;
        // value tracker is set to not conduct a rolling average
        ValueTrackerFloat temp_tracker = ValueTrackerFloat(&temp, 1.0);

        float last_temp = 0.0;
        float temp_high_thresh;
        // indicates how much historesis is applied to the
        // high temp threshold to return to normal operation
        // this indicates if a temperature shutdown is recommended
        float temp_historesis;
        bool temp_shutdown = false;
};

WeatherManager::WeatherManager(float humid_high, float temp_high, 
                                float historesis, unsigned long update) {
    humid_high_thresh = humid_high;
    temp_high_thresh = temp_high;
    temp_historesis = historesis;
    update_delay = update;
}

void WeatherManager::print() {
    Serial.println("---------- WeatherManager Stats --------------");
    Serial.println("Printing temperature value tracker stats");
    temp_tracker.printStats();
    Serial.println("Printing humidity value tracker stats");
    humid_tracker.printStats();
}

bool WeatherManager::init() {
    Wire.begin();
    delay(5000); // let the bus stabilise
    if (sensor.init()) {
        Serial.println("SHT temp/humid sensor was initialised");
        sensor_active = true;
    } else {
        for (int i = 2; i < 20; i++) {
            if (sensor.init() == false) {
                Serial.print("ERROR, SHT init() failed attempt #");Serial.println(i);
                sensor_active = false;
                delay(1000);
            }
            else {
                Serial.print("Attempt #");Serial.print(i);
                Serial.println(" was a success, SHT is now initalised");
                sensor_active = true;
                break;
            }
        }
    }
    return sensor_active;
}

bool WeatherManager::update(){
    // return false if no update is made and true 
    // if an update is made
    // check to see if the sensor is ready for a new reading
    if (sensor_active == false) {
        dprint(p_readings, "ERROR - the SHTC3 temp/humid sensor is not active");
        return false;
    }
    if (last_reading_time > update_delay) {
        if (sensor.readSample()) {
            humid = sensor.getHumidity();
            humid_tracker.update();
            dprint(p_readings, "humid:\t");dprintln(p_readings, humid);
            if (humid >= humid_high_thresh) {
                dprint(p_readings, "this is higher than the humid_high_thresh of: ");dprintln(p_readings, humid_high_thresh);
                dprintln(p_readings, "FLAGGING A HUMIDITY SHUTDOWN CONDITION");
                humid_shutdown = true;
            }
            temp = sensor.getTemperature();
            temp_tracker.update();
            dprint(p_readings, "temp:\t");dprintln(p_readings, temp);
            if (temp >= temp_high_thresh && temp_shutdown == false) {
                dprint(p_readings, "this is higher than the temp_high_thresh of: ");dprintln(p_readings, temp_high_thresh);
                dprintln(p_readings, "FLAGGING A TEMPERATURE SHUTDOWN CONDITION");
                temp_shutdown = true;
            } else if (temp <= (temp_high_thresh * (1.0 - temp_historesis))){ 
                temp_shutdown = false;
            }
            last_reading_time = 0;
            return true;
        } else {
            dprintln(p_readings, "SHT sensor is not ready for a new reading, exiting update");
        }
        // if we make it this far then there are no emergency shudown
        // conditions and we can exit the program
    }
    return false;
}

#endif // __WEATHER_MANAGER_H__

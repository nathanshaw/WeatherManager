#ifndef __WEATHER_MANAGER_H__
#define __WEATHER_MANAGER_H__

#include <ValueTrackerDouble.h>
// #include <Wire.h>
#include <Adafruit_SHTC3.h>

class WeatherManager {
    public:
        WeatherManager(float humid_high, float temp_high, float hysteresis, unsigned long update);
        bool init();
        bool update();
        bool getTempShutdown(){return temp_shutdown;};
        bool getHumidityShutdown(){return humid_shutdown;};

        // getter functions
        float getTemperature() {return temp;};
        float getHumidity() {return humid;};
        float getScaledTemp() {return temp_tracker.getScaled();};
        float getScaledHumid() {return humid_tracker.getScaled();};

        void print();
        void setPrintReadings(bool p){p_readings = p;};

    private:

        bool p_readings = false;

        elapsedMillis last_reading_time;
        unsigned long update_delay;
        ////////////////// Sensor /////////////////////////
        float sensor_active = false;
        Adafruit_SHTC3 sensor = Adafruit_SHTC3();

        /////////////////// Humidity //////////////////////
        double humid = 0.0;
        // value tracker is set to not conduct a rolling average
        ValueTrackerDouble humid_tracker = ValueTrackerDouble("humidity", &humid, 1.0);

        float humid_high_thresh;
        // this will indicate if the WeatherManager recommends
        // a shutdown due to high humidity
        bool  humid_shutdown = false;

        /////////////////// Temperature ///////////////////
        double temp = 0.0;
        // value tracker is set to not conduct a rolling average
        ValueTrackerDouble temp_tracker = ValueTrackerDouble("temperature", &temp, 1.0);

        float temp_high_thresh;
        // indicates how much hysteresis is applied to the
        // high temp threshold to return to normal operation
        // this indicates if a temperature shutdown is recommended
        float temp_hysteresis;
        bool temp_shutdown = false;
};

WeatherManager::WeatherManager(float humid_high, float temp_high, 
                                float hysteresis, unsigned long update) {
    humid_high_thresh = humid_high;
    temp_high_thresh = temp_high;
    temp_hysteresis = hysteresis;
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
    // Serial.println("Initalising WeatherManager by first opening Wire bus");
    // Wire.begin();
    // Serial.println("Wire bus has began, now waiting a few seconds for it to settle");
    Serial.println("Attempting to initalise sensor...");
    delay(100); // let the bus stabilise
    if (sensor.begin()) {
        Serial.println("SHT temp/humid sensor was initialised");
        sensor_active = true;
    } else {
        for (int i = 2; i < 20; i++) {
            if (sensor.begin() == false) {
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
    Serial.print("Exiting WeatherManager.init() with a result of: ");
    Serial.println(sensor_active);
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
    if (last_reading_time < update_delay){
        dprintln(p_readings, "SHT sensor is not ready for a new reading, exiting update");
        return false;
    }
    // create temporary sensor "events" for temp and humid
    sensors_event_t _humidity, _temp;
    sensor.getEvent(&_humidity, &_temp);
    humid = _humidity.relative_humidity;
    humid_tracker.update();
    dprint(p_readings, "humid:\t");dprintln(p_readings, humid);
    if (humid >= humid_high_thresh) {
        dprint(p_readings, "this is higher than the humid_high_thresh of: ");dprintln(p_readings, humid_high_thresh);
        dprintln(p_readings, "FLAGGING A HUMIDITY SHUTDOWN CONDITION");
        humid_shutdown = true;
        return true;
    }
    temp = _temp.temperature;
    temp_tracker.update();
    dprint(p_readings, "temp:\t");dprintln(p_readings, temp);
    if (temp >= temp_high_thresh && temp_shutdown == false) {
        dprint(p_readings, "this is higher than the temp_high_thresh of: ");dprintln(p_readings, temp_high_thresh);
        dprintln(p_readings, "FLAGGING A TEMPERATURE SHUTDOWN CONDITION");
        temp_shutdown = true;
    } else if (temp <= (temp_high_thresh * (1.0 - temp_hysteresis))){ 
        temp_shutdown = false;
    }
    last_reading_time = 0;
    return true;
}

#endif // __WEATHER_MANAGER_H__

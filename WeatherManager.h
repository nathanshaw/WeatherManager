#ifndef __WEATHER_MANAGER_H__
#define __WEATHER_MANAGER_H__

// #include "../ValueTracker.h"

class WeatherManager {
    public:
        WeatherManager(float h, float t);
        bool update();
    private:
        float sensor_active = false;
        /////////////////// Humidity //////////////////////
        float humid = 0.0;
        float last_humid = 0.0;
        float humid_high_thresh;

        /////////////////// Temperature ///////////////////
        float temp = 0.0;
        float last_temp = 0.0;
};

WeatherManager::WeatherManager(float h, float t) {
    humid_high_thresh = h;
    temp_high_thresh = t;
}

bool update(){
    // see if it is time to take a new reading
    // if it is then take a new reading
    // update the current and last values
    // TODO - also track the delta over time (is it heating up?)
    // return false if no update is made and true if an update is made
}


#endif // __WEATHER_MANAGER_H__

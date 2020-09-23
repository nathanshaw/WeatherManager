#include <WeatherManager.h>

const float humid_high_thresh = 90;
const float temp_high_thresh  = 90;
const float historesis = 0.2;

WeatherManager weather_manager = WeatherManager(humid_high_thresh, temp_high_thresh, historesis);
elapsedMicros em;

void setup() {
  Serial.begin(57600);
  delay(3000);
  weather_manager.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  em = 0;
  weather_manager.update();
  Serial.print("micros for update : ");Serial.println(em);
  if (weather_manager.getHumidityShutdown()) {
    Serial.println("HUMIDITY SHUTDOWN");
    delay(10000);
  }
  if (weather_manager.getTempShutdown()) {
    Serial.println("TEMPERATURE SHUTDOWN");
    delay(10000);
  }
  weather_manager.print();
  delay(1000);
}

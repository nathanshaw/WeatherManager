#include <WeatherManager.h>

WeatherManager wm = WeatherManager(100, 100, 0.2);

void setup() {
  Serial.begin(57600);
  delay(1000);
  wm.init();
  delay(1000);
}

void loop() {
  wm.update();
  delay(500);
}

#include <Wire.h>
#include <HTU21D.h>

// SDA 21
// SCL 22

HTU21D temp_hum_sensor(HTU21D_RES_RH12_TEMP14);
bool htuIsConnected = false;
void sensor_setup()
{
  int retryCount = 0;
  while (temp_hum_sensor.begin() != true && retryCount < 5)
  {
    retryCount++;
    delay(1000);
  }
  if (retryCount == 5) {
    Serial.println("\n\rFailed to configure HTU21D sensor");
  } else {
    Serial.print("\n\rConnected to HTU21D: ");
    Serial.println(temp_hum_sensor.readDeviceID());
    htuIsConnected = true;
  }
}

bool htuSensorIsConnected() {
  return htuIsConnected;
}

float getTemp() {
  return round(temp_hum_sensor.readTemperature() * 100) / 100;
}

float getHumidity() {
  return round(temp_hum_sensor.readHumidity() * 100) / 100;
}

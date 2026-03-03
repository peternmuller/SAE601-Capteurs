#include <SHT2x.h>
#include <Wire.h>
#include <Zigbee.h>

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif
#define TEMP_SENSOR_ENDPOINT_NUMBER 10
#define LED 15

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60
#define TEMP_THRESHOLD 1.0f
#define HUM_THRESHOLD 2.0f

RTC_DATA_ATTR float derniereTemperature = -999.0f;
RTC_DATA_ATTR float derniereHumidite = -999.0f;

ZigbeeTempSensor zbTempSensor = ZigbeeTempSensor(TEMP_SENSOR_ENDPOINT_NUMBER);
SHT2x sht;

void setup() {
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  delay(100);
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  Serial.begin(115200);
  Wire.begin();
  sht.begin();

  sht.read();
  float temperature = sht.getTemperature();
  float humidite = sht.getHumidity();
  float tempDiff = fabsf(temperature - derniereTemperature);
  float humDiff = fabsf(humidite - derniereHumidite);

  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print(" °C\tHumidity: ");
  Serial.print(humidite, 1);
  Serial.println(" %");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  if (tempDiff > TEMP_THRESHOLD || humDiff > HUM_THRESHOLD) {

    zbTempSensor.setManufacturerAndModel("Espressif", "Peter-ZigbeeTempSensor");
    zbTempSensor.setMinMaxValue(-20, 50);
    zbTempSensor.setTolerance(1);
    zbTempSensor.addHumiditySensor(0, 100, 1, 0.0);

    Zigbee.addEndpoint(&zbTempSensor);
    Zigbee.setRxOnWhenIdle(false);

    esp_zb_cfg_t zigbeeConfig = ZIGBEE_DEFAULT_ED_CONFIG();
    zigbeeConfig.nwk_cfg.zed_cfg.keep_alive = 10000;
    Zigbee.setTimeout(10000);

    if (!Zigbee.begin(&zigbeeConfig, false)) {
      Serial.println("Zigbee failed to start!");
      Serial.println("Rebooting...");
      ESP.restart();
    }
    Serial.println("Connecting to network");
    while (!Zigbee.connected()) {
      Serial.print(".");
      delay(100);
    }
    Serial.println();
    Serial.println("Successfully connected to Zigbee network");

    zbTempSensor.setTemperature(temperature);
    zbTempSensor.setHumidity(humidite);
    zbTempSensor.report();
    delay(250);

    derniereTemperature = temperature;
    derniereHumidite = humidite;
    digitalWrite(LED, HIGH);
  } else {
    Serial.println("Pas d'envoi ZigBee :");
    Serial.print("  Temp: ");
    Serial.print(temperature, 1);
    Serial.print(" °C (Δ ");
    Serial.print(tempDiff, 2);
    Serial.print(" °C ");
    Serial.print(tempDiff > TEMP_THRESHOLD ? ">" : "<=");
    Serial.print(" ");
    Serial.print(TEMP_THRESHOLD, 1);
    Serial.println(")");

    Serial.print("  Hum:  ");
    Serial.print(humidite, 1);
    Serial.print(" % (Δ ");
    Serial.print(humDiff, 2);
    Serial.print(" % ");
    Serial.print(humDiff > HUM_THRESHOLD ? ">" : "<=");
    Serial.print(" ");
    Serial.print(HUM_THRESHOLD, 1);
    Serial.println(")");
  }

  Serial.flush();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {}

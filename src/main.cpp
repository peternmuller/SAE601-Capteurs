#include "LoRaE5.h"
#include "secrets.h"
#include <SHT2x.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define LED 15
#define LORA_RX_PIN 16
#define LORA_TX_PIN 17

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60
#define TEMP_THRESHOLD 1.0f
#define HUM_THRESHOLD 2.0f

RTC_DATA_ATTR float derniereTemperature = -999.0f;
RTC_DATA_ATTR float derniereHumidite = -999.0f;
RTC_DATA_ATTR bool setupLora = false;

SoftwareSerial loraSerial(LORA_RX_PIN, LORA_TX_PIN);
LoRaE5 myLoRaE5(&loraSerial);
SHT2x sht;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sht.begin();

  sht.read();
  float temperature = sht.getTemperature();
  float humidite = sht.getHumidity();
  float tempDiff = fabsf(temperature - derniereTemperature);
  float humDiff = fabsf(humidite - derniereHumidite);

  if (tempDiff > TEMP_THRESHOLD || humDiff > HUM_THRESHOLD) {
    uint8_t donnees[2];
    donnees[0] = (uint8_t)((temperature + 20) * 4);
    donnees[1] = (uint8_t)(humidite * 2);

    Serial.print("\tTempérature : ");
    Serial.print(temperature);
    Serial.print(" °C - ");
    Serial.println(donnees[0], HEX);
    Serial.print("\tHumidité : ");
    Serial.print(humidite);
    Serial.print(" % - ");
    Serial.println(donnees[1], HEX);

    loraSerial.begin(9600);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    if (!setupLora) {
      myLoRaE5.init();
      myLoRaE5.reset();
      myLoRaE5.setEU868BandPlan();
      myLoRaE5.setDR(5);
      myLoRaE5.setPower(16);
      myLoRaE5.setADR(true);
      myLoRaE5.setMode(LoRaE5::Mode::ABP);
      myLoRaE5.setDevAddr(SECRET_DEV_ADDR);
      myLoRaE5.setNwkSKey(SECRET_NWK_SKEY);
      myLoRaE5.setAppSKey(SECRET_APP_SKEY);
      myLoRaE5.setPort(1);
      setupLora = true;
    } else {
      myLoRaE5.wakeup();
    }

    Serial.println("\tEnvoi LoRa");
    myLoRaE5.sendMessage(donnees, 2);
    myLoRaE5.sleep();
    derniereTemperature = temperature;
    derniereHumidite = humidite;
    digitalWrite(LED, HIGH);
  } else {
    Serial.println("Pas d'envoi LoRa :");
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

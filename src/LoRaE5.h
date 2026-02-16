#ifndef __LORAE5_H__
#define __LORAE5_H__

#include <Arduino.h>

#include "ATInterface.h"

class LoRaE5 : private ATInterface {
public:
  enum Mode { ABP, OTAA };
  enum Class { A, B, C };

  LoRaE5(Stream *stream);
  ~LoRaE5();

  bool init();
  bool reset();
  bool setEU868BandPlan();

  bool setDR(uint8_t dr);
  int8_t getDR();

  bool setPower(uint8_t power);
  int8_t getPower();

  bool setPort(uint8_t port);
  int16_t getPort();

  bool setADR(bool adr);
  bool getADR();

  bool setMode(LoRaE5::Mode mode);
  LoRaE5::Mode getMode();

  bool setClass(LoRaE5::Class cls);
  LoRaE5::Class getClass();

  bool setNwkSKey(String nwkSKey);
  bool setAppSKey(String appSKey);
  bool setAppKey(String appKey);

  bool setDevAddr(String devAddr);
  bool setDevEUI(String devEUI);
  bool setAppEUI(String appEUI);

  bool join();

  bool sendMessage(String message, bool confirm = false);
  bool sendMessage(uint8_t *message, uint8_t size, bool confirm = false);

  bool sleep();
  bool wakeup();

  float getLastSNR();
  int16_t getLastRSSI();
  uint8_t getLastPortMessage();
  String getLastMessage();

private:
  String formatValue(String value);
  String formatAnswer(String answer);
  void parseMessageAnswer(String answer);
  bool parseMessageError(String answer);

  float _lastSNR;
  int16_t _lastRSSI;
  uint8_t _lastPortMessage;
  String _lastMessage;
};

#endif

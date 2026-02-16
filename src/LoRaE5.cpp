#include "LoRaE5.h"

LoRaE5::LoRaE5(Stream *stream) : ATInterface(stream) {}
LoRaE5::~LoRaE5() {}

bool LoRaE5::init() {
  bool res;
  String answer;
  res = this->sendATCommandAndCheckAnswer("AT", "+AT: OK", 2000);
  return res;
}

bool LoRaE5::reset() {
  bool res;
  res = this->sendATCommandAndCheckAnswer("AT+RESET", "+RESET: OK", 2000);
  if (!res) {
    return false;
  }
  return true;
}

bool LoRaE5::setEU868BandPlan() {
  bool res;
  const char querry[][50] PROGMEM = {"AT+DR=EU868",       "AT+CH=3,867.1,0,5", "AT+CH=4,867.3,0,5",
                                     "AT+CH=5,867.5,0,5", "AT+CH=6,867.7,0,5", "AT+CH=7,867.9,0,5"};
  const char answers[][50] PROGMEM = {"+DR: EU868",
                                      "+CH: 3,867100000,DR0:DR5",
                                      "+CH: 4,867300000,DR0:DR5",
                                      "+CH: 5,867500000,DR0:DR5",
                                      "+CH: 6,867700000,DR0:DR5",
                                      "+CH: 7,867900000,DR0:DR5"};
  for (int i = 0; i < 6; i++) {
    res = this->sendATCommandAndCheckAnswer(querry[i], answers[i], 2000);
    if (!res) {
      return false;
    }
  }
  return true;
}

bool LoRaE5::setDR(uint8_t dr) {
  bool res;
  if ((dr >= 0) && (dr <= 7)) {
    res = this->sendATCommandAndCheckAnswer("AT+DR=" + String(dr), "+DR: DR" + String(dr), 2000);
    return res;
  }
  return false;
}

int8_t LoRaE5::getDR() {
  bool res;
  String answer;
  res = this->sendATCommandAndReadAnswer("AT+DR", &answer, 2000);
  if (res) {
    answer = answer.substring(0, answer.indexOf("\r\n"));
    answer = answer.substring(5);
    int8_t dr = answer.substring(2).toInt();
    return dr;
  }
  return -1;
}

bool LoRaE5::setPower(uint8_t power) {
  if ((power >= 1) && (power <= 16)) {
    bool res;
    res = this->sendATCommandAndCheckAnswer("AT+POWER=" + String(power), "+POWER: " + String(power), 2000);
    return res;
  }
  return false;
}

int8_t LoRaE5::getPower() {
  bool res;
  String answer;
  res = this->sendATCommandAndReadAnswer("AT+POWER", &answer, 2000);
  if (res) {
    answer = answer.substring(0, answer.indexOf("\r\n"));
    answer = answer.substring(7);
    int8_t power = answer.substring(0).toInt();
    return power;
  }
  return -1;
}

bool LoRaE5::setPort(uint8_t port) {
  bool res = false;
  if ((port >= 1) && (port <= 255)) {
    res = this->sendATCommandAndCheckAnswer("AT+PORT=" + String(port), "+PORT: " + String(port), 2000);
  }
  return res;
}

int16_t LoRaE5::getPort() {
  bool res;
  String answer;
  res = this->sendATCommandAndReadAnswer("AT+PORT", &answer, 2000);
  if (res) {
    answer = answer.substring(0, answer.indexOf("\r\n"));
    answer = answer.substring(6);
    int16_t port = answer.substring(0).toInt();
    return port;
  }
  return -1;
}

bool LoRaE5::setADR(bool adr) {
  bool res;
  if (adr) {
    res = this->sendATCommandAndCheckAnswer("AT+ADR=ON", "+ADR: ON", 2000);
    return res;
  } else {
    res = this->sendATCommandAndCheckAnswer("AT+ADR=OFF", "+ADR: OFF", 2000);
    return res;
  }
  return false;
}

bool LoRaE5::getADR() {
  bool res;
  String answer;
  res = this->sendATCommandAndReadAnswer("AT+ADR", &answer, 2000);
  if (res) {
    answer = answer.substring(0, answer.indexOf("\r\n"));
    answer = answer.substring(5);
    if (answer.indexOf("ON") != -1) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

bool LoRaE5::setMode(LoRaE5::Mode mode) {
  bool res;
  if (mode == LoRaE5::Mode::ABP) {
    res = this->sendATCommandAndCheckAnswer("AT+MODE=LWABP", "+MODE: LWABP", 2000);
    return res;
  } else if (mode == LoRaE5::Mode::OTAA) {
    res = this->sendATCommandAndCheckAnswer("AT+MODE=LWOTAA", "+MODE: LWOTAA", 2000);
    return res;
  }
  return false;
}

LoRaE5::Mode LoRaE5::getMode() {
  bool res;
  String answer;
  res = this->sendATCommandAndReadAnswer("AT+MODE", &answer, 2000);
  if (res) {
    if (answer.indexOf("LWABP") != -1) {
      return LoRaE5::Mode::ABP;
    } else if (answer.indexOf("LWOTAA") != -1) {
      return LoRaE5::Mode::OTAA;
    }
  }
  return LoRaE5::Mode::ABP;
}

bool LoRaE5::setClass(LoRaE5::Class cls) {
  bool res;
  switch (cls) {
  case LoRaE5::Class::A:
    return this->sendATCommandAndCheckAnswer("AT+CLASS=A", "+CLASS: A", 2000);
    break;
  case LoRaE5::Class::B:
    return this->sendATCommandAndCheckAnswer("AT+CLASS=B", "+CLASS: B", 2000);
    break;
  case LoRaE5::Class::C:
    return this->sendATCommandAndCheckAnswer("AT+CLASS=C", "+CLASS: C", 2000);
    break;
  default:
    return false;
    break;
  }
  return false;
}

LoRaE5::Class LoRaE5::getClass() {
  bool res;
  String answer;
  res = this->sendATCommandAndReadAnswer("AT+CLASS", &answer, 2000);
  if (res) {
    if (answer.indexOf("A") != -1) {
      return LoRaE5::Class::A;
    } else if (answer.indexOf("B") != -1) {
      return LoRaE5::Class::B;
    } else if (answer.indexOf("C") != -1) {
      return LoRaE5::Class::C;
    }
  }
  // default value
  return LoRaE5::Class::A;
}

bool LoRaE5::setNwkSKey(String nwkSKey) {
  bool res;
  String _nwkSKey = this->formatValue(nwkSKey);
  if (_nwkSKey.length() != 32) {
    return false;
  }
  res = this->sendATCommandAndCheckAnswer("AT+KEY=NWKSKEY, \"" + _nwkSKey + "\"", "+KEY: NWKSKEY " + _nwkSKey, 2000);
  return res;
}

bool LoRaE5::setAppSKey(String appSKey) {
  bool res;
  String _appSKey = this->formatValue(appSKey);
  if (_appSKey.length() != 32) {
    return false;
  }
  res = this->sendATCommandAndCheckAnswer("AT+KEY=APPSKEY, \"" + _appSKey + "\"", "+KEY: APPSKEY " + _appSKey, 2000);
  return res;
}

bool LoRaE5::setAppKey(String appKey) {
  bool res;
  String _appKey = this->formatValue(appKey);
  if (_appKey.length() != 32) {
    return false;
  }
  res = this->sendATCommandAndCheckAnswer("AT+KEY=APPKEY, \"" + _appKey + "\"", "+KEY: APPKEY " + _appKey, 2000);
  return res;
}

bool LoRaE5::setDevAddr(String devAddr) {
  bool res;
  String _devAddr = this->formatValue(devAddr);
  if (_devAddr.length() != 8) {
    return false;
  }
  String formatedDevAddr = this->formatAnswer(_devAddr);
  res = this->sendATCommandAndCheckAnswer("AT+ID=DevAddr," + _devAddr, "+ID: DevAddr, " + formatedDevAddr, 2000);
  return res;
}

bool LoRaE5::setDevEUI(String devEUI) {
  bool res;
  String _devEUI = this->formatValue(devEUI);
  ESP_LOGD("setDevEUI", "setDevEUI: %s", _devEUI.c_str());
  if (_devEUI.length() != 16) {
    return false;
  }
  String formatedDevEUI = this->formatAnswer(_devEUI);
  res = this->sendATCommandAndCheckAnswer("AT+ID=DevEui," + _devEUI, "+ID: DevEui, " + formatedDevEUI, 2000);
  return res;
}

bool LoRaE5::setAppEUI(String appEUI) {
  bool res;
  String _appEUI = this->formatValue(appEUI);
  if (_appEUI.length() != 16) {
    return false;
  }
  String formatedAppEUI = this->formatAnswer(_appEUI);
  res = this->sendATCommandAndCheckAnswer("AT+ID=AppEui," + _appEUI, "+ID: AppEui, " + formatedAppEUI, 2000);
  return res;
}

bool LoRaE5::join() {
  bool res = false;
  String answer;
  res = this->sendATCommandAndReadAnswer("AT+JOIN", &answer, 20000);
  ESP_LOGD("join", "join: %s", answer.c_str());
  if (res) {
    if (answer.indexOf("+JOIN: Start") == -1) {
      return false;
    }
  }
  answer = this->readAnswer(20000);
  ESP_LOGD("join", "join: %s", answer.c_str());
  if (res) {
    if (answer.indexOf("+JOIN: NetID") != -1) {
      return true;
    }
    if (answer.indexOf("+JOIN: Join failed") != -1) {
      return false;
    }
  }
  return res;
}

bool LoRaE5::sendMessage(String message, bool confirm) {
  bool res;
  String answer;
  String _message = this->formatValue(message);

  if (confirm) {
    res = this->sendATCommandAndReadAnswer("AT+CMSG=\"" + _message + "\"", &answer, 20000);
  } else {
    res = this->sendATCommandAndReadAnswer("AT+MSG=\"" + _message + "\"", &answer, 20000);
  }

  // Traitement de certains message d'erreurs
  if (this->parseMessageError(answer)) {
    return false;
  }

  if (res) {
    if ((!confirm) && (answer.indexOf("+MSG: Start") == -1)) {
      return false;
    }
    if ((confirm) && (answer.indexOf("+CMSG: Start") == -1)) {
      return false;
    }
  } else {
    return false;
  }
  answer = this->readAnswer(20000);
  this->parseMessageAnswer(answer);

  if (res) {
    if ((!confirm) && (answer.indexOf("+MSG: Done") != -1)) {
      return true;
    }
    if ((confirm) && (answer.indexOf("+CMSG: Done") != -1) && (answer.indexOf("+CMSG: ACK Received") != -1)) {
      return true;
    }
  }
  return false;
}

bool LoRaE5::sendMessage(uint8_t *message, uint8_t size, bool confirm) {
  bool res;
  String answer;
  String _message;

  for (int i = 0; i < size; i++) {
    if (message[i] < 16)
      _message += '0';
    _message += String(message[i], HEX);
  }

  if (confirm) {
    res = this->sendATCommandAndReadAnswer("AT+CMSGHEX=\"" + _message + "\"", &answer, 20000);
  } else {
    res = this->sendATCommandAndReadAnswer("AT+MSGHEX=\"" + _message + "\"", &answer, 20000);
  }

  // Traitement de certains message d'erreurs
  if (this->parseMessageError(answer)) {
    return false;
  }

  if (res) {
    if ((!confirm) && (answer.indexOf("+MSGHEX: Start") == -1)) {
      return false;
    }
    if ((confirm) && (answer.indexOf("+CMSGHEX: Start") == -1)) {
      return false;
    }
  } else {
    return false;
  }
  answer = this->readAnswer(20000);
  this->parseMessageAnswer(answer);

  if (res) {
    if ((!confirm) && (answer.indexOf("+MSGHEX: Done") != -1)) {
      return true;
    }
    if ((confirm) && (answer.indexOf("+CMSGHEX: Done") != -1) && (answer.indexOf("+CMSGHEX: ACK Received") != -1)) {
      return true;
    }
  }
  return false;
}

bool LoRaE5::sleep() {
  bool res = false;
  res = this->sendATCommandAndCheckAnswer("AT+LOWPOWER", "+LOWPOWER: SLEEP", 2000);
  return res;
}

bool LoRaE5::wakeup() {
  bool res;
  String answer;
  res = this->sendATCommandAndCheckAnswer("AT", "+LOWPOWER: WAKEUP", 2000);
  return res;
}

bool LoRaE5::parseMessageError(String answer) {
  if (answer.indexOf("Length error 0") != -1) {
    // Note: N could be 0 or none zero value, if it returns 0, it means there is a pending Uplink MAC Command
    // must be sent through Port 0. User need send a dummy MSG command "AT+MSG" to flush uplink MAC command.
    // Message quand l'ADR est coupé sur la passerelle ???
    ESP_LOGV("sendMessage", "Length error 0");
    this->sendATCommandAndReadAnswer("AT+MSG", &answer, 20000);
    ESP_LOGV("sendMessage", "sendMessage after length error 0: %s", answer.c_str());
    return true;
  }
  return false;
}

void LoRaE5::parseMessageAnswer(String answer) {
  // TODO extraire dans une fonction interne de parsing...
  if (answer.indexOf("RSSI") != -1) {
    String buffer = answer.substring(answer.indexOf("RSSI") + 5);
    buffer = buffer.substring(0, buffer.indexOf(","));
    this->_lastRSSI = buffer.toInt();
  }

  if (answer.indexOf("SNR") != -1) {
    String buffer = answer.substring(answer.indexOf("SNR") + 4);
    buffer = buffer.substring(0, buffer.indexOf("\r\n"));
    this->_lastSNR = buffer.toFloat();
  }

  if (answer.indexOf("PORT") != -1) {
    String buffer = answer.substring(answer.indexOf("PORT") + 5);
    buffer = buffer.substring(0, buffer.indexOf(";"));
    this->_lastPortMessage = buffer.toInt();
  }

  if (answer.indexOf("RX: ") != -1) {
    String buffer = answer.substring(answer.indexOf("RX: \"") + 4, buffer.indexOf("\"\r\n"));
    buffer.replace("\"", "");
    // buffer = buffer.substring(0, buffer.indexOf("\""));
    // ESP_LOGD("sendMessage", "RX buffer: /%s/", buffer.c_str());
    this->_lastMessage = buffer;
  }
}

float LoRaE5::getLastSNR() {
  float value = this->_lastSNR;
  this->_lastSNR = 0.0;
  return value;
}

int16_t LoRaE5::getLastRSSI() {
  int16_t value = this->_lastRSSI;
  this->_lastRSSI = 0;
  return value;
}

uint8_t LoRaE5::getLastPortMessage() {
  uint8_t value = this->_lastPortMessage;
  this->_lastPortMessage = 0;
  return value;
}

String LoRaE5::getLastMessage() {
  String value = this->_lastMessage;
  this->_lastMessage = "";
  return value;
}

String LoRaE5::formatValue(String value) {
  String formatedValue = value;
  formatedValue.replace(" ", "");
  formatedValue.replace(":", "");
  formatedValue.toUpperCase();
  return formatedValue;
}

// Convert 0102030405060708 to 01:02:03:04:05:06:07:08
String LoRaE5::formatAnswer(String answer) {
  String formatedAnswer = "";
  for (int i = 0; i < answer.length(); i++) {
    formatedAnswer += answer[i];
    if (i % 2 == 1 && i != answer.length() - 1) {
      formatedAnswer += ":";
    }
  }
  return formatedAnswer;
}

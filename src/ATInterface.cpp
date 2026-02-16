#include "ATInterface.h"

ATInterface::ATInterface(Stream *stream) { _myStream = stream; }

ATInterface::~ATInterface() { _myStream = NULL; }

bool ATInterface::sendATCommand(String command) {
  // TODO Add parameter for \r and/or \n
  size_t value;
  ESP_LOGV("ATInterface", "AT command: %s", command.c_str());
  value = _myStream->println(command);
  if ((value == 0) || (value != command.length() + 2)) {
    ESP_LOGE("ATInterface", "Error sending command");
    return false;
  }
  return true;
}

bool ATInterface::sendATCommandAndReadAnswer(String command, String *answer, uint32_t timeout) {
  bool res;
  res = this->sendATCommand(command);
  if (!res) {
    ESP_LOGE("ATInterface", "Error sending command");
    return false;
  }

  uint32_t start = millis();
  while ((millis() - start) < timeout) {
    if (_myStream->available()) {
      *answer = _myStream->readString();
      ESP_LOGV("ATInterface", "Response: %s", answer->c_str());
      return true;
    }
  }
  ESP_LOGE("ATInterface", "Timeout: no answer!");
  return false;
}

bool ATInterface::sendATCommandAndCheckAnswer(String command, String expectedAnswer, uint32_t timeout) {
  bool res;
  String answer;
  res = this->sendATCommandAndReadAnswer(command, &answer, timeout);
  if (!res) {
    return false;
  }
  if (answer.indexOf(expectedAnswer) != -1) {
    return true;
  }
  ESP_LOGE("ATInterface", "Error: answer is not the expected one");
  return false;
}

String ATInterface::readAnswer(uint32_t timeout) {
  String answer;
  uint32_t start = millis();
  while ((millis() - start) < timeout) {
    if (_myStream->available()) {
      answer = _myStream->readString();
      ESP_LOGV("ATInterface", "Response: %s", answer.c_str());
      return answer;
    }
  }
  ESP_LOGE("ATInterface", "Timeout: no awser!");
  return answer;
}

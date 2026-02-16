#ifndef __ATINTERFACE_H__
#define __ATINTERFACE_H__

#include <Arduino.h>
#include <Stream.h>

class ATInterface {
public:
  ATInterface(Stream *stream);
  ~ATInterface();
  bool sendATCommand(String command);
  bool sendATCommandAndReadAnswer(String command, String *answer, uint32_t timeout);
  bool sendATCommandAndCheckAnswer(String command, String expectedAnswer, uint32_t timeout);
  String readAnswer(uint32_t timeout);

private:
  Stream *_myStream;
};

#endif

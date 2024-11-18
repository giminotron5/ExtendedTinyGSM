#ifndef EXTENDEDTINYGSM_H
#define EXTENDEDTINYGSM_H

#include <Arduino.h>
#include <TinyGsmClient.h>

#warning "This library only supports SIM800 series!"

enum class SmsDeleteMode
{
  READ,   // Delete all read messages
  UNREAD, // Delete all unread messages
  SENT,   // Delete all sent SMS
  UNSENT, // Delete all unsent SMS
  INBOX,  // Delete all received SMS
  ALL     // Delete all SMS
};

class ExtendedTinyGSM : public TinyGsm,
                        public TinyGsmClient
{
public:

  ExtendedTinyGSM(Stream &stream) : TinyGsm(stream) {}

  bool hardwareReset(uint8_t resetPin, bool resetLogic);

  // int getStatusCode(const char *server, uint16_t port);

  bool setSmsMessageFormat(bool format);
  bool setSmsIndication(uint8_t mode = 1, uint8_t mt = 2, uint8_t bm = 0, uint8_t ds = 0, uint8_t bfr = 0);
  bool setSmsParameters(uint8_t fo = 17, uint8_t vp = 167, uint8_t pid = 0, uint8_t dcs = 0);

  int    checkForNewSMS();
  String getSmsNumber(int smsIndex);
  String getSmsText(int smsIndex);

  bool deleteSmsByMode(SmsDeleteMode mode);
  bool deleteSmsByIndex(int smsIndex);
};

#endif

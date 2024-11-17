#include <Arduino.h>
#include "ExtendedTinyGSM.h"

bool ExtendedTinyGSM::hardwareReset(uint8_t resetPin, bool resetLogic)
{
  // Reset modem
  digitalWrite(resetPin, resetLogic);
  delay(2000); // Wait for 2 second
  digitalWrite(resetPin, !resetLogic);
  delay(5000); // Wait for 5 seconds

  // Wait for modem to restart
  return TinyGsm::restart();
}

/**
 * @brief Sets SMS message format to Text or PDU mode.
 * 
 * @param format `true` for Text mode (CMGF=1), `false` for PDU mode (CMGF=0).
 * @return true if the command succeeded, false otherwise.
 */
bool ExtendedTinyGSM::setSmsMessageFormat(bool format)
{
  TinyGsm::sendAT("+CMGF=", format ? "1" : "0");
  return TinyGsm::waitResponse(1000L) == 1; // returns true if response is OK
}
/**
 * @brief Configures SMS indication settings for new messages.
 * 
 * @param mode Specifies how indications are handled (0-3).
 * @param mt Controls if and how SMS-DELIVER indications are routed (0-3).
 * @param bm Sets behavior for Cell Broadcast Messages (0 or 2).
 * @param ds Enables/disables SMS-STATUS-REPORT routing (0 or 1).
 * @param bfr Sets buffer behavior for unsolicited codes (0 or 1).
 * @return true if the command succeeded, false otherwise.
 */
bool ExtendedTinyGSM::setSmsIndication(uint8_t mode, uint8_t mt, uint8_t bm, uint8_t ds, uint8_t bfr)
{
  if(mode > 3 || mt > 3) { return false; }
  if(bm != 0 && bm != 2) { return false; }
  if(ds > 1 || bfr > 1) { return false; }

  TinyGsm::sendAT("+CNMI=", mode, ",", mt, ",", bm, ",", ds, ",", bfr);
  return TinyGsm::waitResponse(1000L) == 1; // returns true if response is OK
}
/**
 * @brief Configures SMS service parameters such as format and encoding.
 * 
 * @param fo Specifies the format of the SMS (0-255).
 * @param vp Sets the validity period of the SMS in seconds (0-255).
 * @param pid Defines the protocol identifier (0-255).
 * @param dcs Configures the data coding scheme (0-255).
 * @return true if the command succeeded, false otherwise.
 */
bool ExtendedTinyGSM::setSmsParameters(uint8_t fo, uint8_t vp, uint8_t pid, uint8_t dcs)
{
  if(fo > 255 || vp > 255 || pid > 255 || dcs > 255) { return false; }

  TinyGsm::sendAT("+CSMP=", fo, ",", vp, ",", pid, ",", dcs);
  return TinyGsm::waitResponse(1000L) == 1; // returns true if response is OK
}

/**
 * @brief Checks for any new unread SMS and returns its index if available.
 * 
 * @return Index of the unread SMS if found, or -1 if no unread SMS is available.
 */
int ExtendedTinyGSM::checkForNewSMS()
{
  TinyGsm::sendAT("+CMGL=\"REC UNREAD\"");
  if(TinyGsm::waitResponse(10000L) == 1)
  {
    String response   = TinyGsm::stream.readStringUntil('\n');
    int    indexStart = response.indexOf(": ") + 2;
    int    indexEnd   = response.indexOf(",", indexStart);

    if(indexStart != -1 && indexEnd != -1)
    {
      int smsIndex = response.substring(indexStart, indexEnd).toInt();
      return smsIndex;
    }
  }

  return -1;
}

/**
 * @brief Retrieves the sender's phone number of the specified SMS.
 * 
 * @param smsIndex Index of the SMS message to read.
 * @return String containing the sender's phone number, or an empty string if not found.
 */
String ExtendedTinyGSM::getSmsNumber(int smsIndex)
{
  TinyGsm::sendAT("+CMGR=", smsIndex);
  if(TinyGsm::waitResponse(10000L) == 1)
  {
    String response = TinyGsm::stream.readStringUntil('\n');
    int    startIdx = response.indexOf(",\"") + 2;
    int    endIdx   = response.indexOf("\"", startIdx);

    if(startIdx != -1 && endIdx != -1) { return response.substring(startIdx, endIdx); }
  }

  return "";
}
/**
 * @brief Retrieves the text content of the specified SMS.
 * 
 * @param smsIndex Index of the SMS message to read.
 * @return String containing the SMS message text, or an empty string if not found.
 */
String ExtendedTinyGSM::getSmsText(int smsIndex)
{
  TinyGsm::sendAT("+CMGR=", smsIndex);
  if(TinyGsm::waitResponse(10000L) == 1)
  {
    TinyGsm::stream.readStringUntil('\n'); // Skip the header line
    String message = TinyGsm::stream.readStringUntil('\n');
    return message;
  }

  return "";
}

/**
 * @brief Deletes SMS messages based on the specified deletion mode.
 * 
 * @param mode The mode to specify which SMS to delete:
 * 
 *   - READ to delete all read messages
 * 
 *   - UNREAD to delete all unread messages
 * 
 *   - SENT to delete all sent SMS
 * 
 *   - UNSENT to delete all unsent SMS
 * 
 *   - INBOX to delete all received SMS
 * 
 *   - ALL to delete all SMS
 * 
 * @return true if SMS messages were successfully deleted, false otherwise.
 */
bool ExtendedTinyGSM::deleteSmsByMode(SmsDeleteMode mode)
{
  String command = "+CMGDA=\"DEL ";
  // Define commands based on mode
  switch(mode)
  {
    case SmsDeleteMode::READ:
      command += "READ\"";
      break;
    case SmsDeleteMode::UNREAD:
      command += "UNREAD\"";
      break;
    case SmsDeleteMode::SENT:
      command += "SENT\"";
      break;
    case SmsDeleteMode::UNSENT:
      command += "UNSENT\"";
      break;
    case SmsDeleteMode::INBOX:
      command += "INBOX\"";
      break;
    case SmsDeleteMode::ALL:
      command += "ALL\"";
      break;
    default:
      return false; // Invalid mode
  }

  // Send the command to delete the specified SMS
  TinyGsm::sendAT(command);
  if(TinyGsm::waitResponse(10000L) == 1) { return true; }

  return false;
}
/**
 * @brief Deletes a specific SMS message based on its index.
 * 
 * @param smsIndex The index of the SMS message to delete.
 * @return true if the SMS was successfully deleted, false otherwise.
 */
bool ExtendedTinyGSM::deleteSmsByIndex(int smsIndex)
{
  TinyGsm::sendAT("+CMGD=" + String(smsIndex));
  if(TinyGsm::waitResponse(10000L) == 1) { return true; }
  return false;
}


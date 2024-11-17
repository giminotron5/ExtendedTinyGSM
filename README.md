# ExtendedTinyGSM Library

The **ExtendedTinyGSM** library extends the functionality of the TinyGSM library to provide additional SMS management capabilities for SIM800-based GSM modules.

This library allows users to interact with GSM modules and manage SMS messages more effectively, including SMS deletion, retrieval, and formatting.

## Features

- **SMS Management**:  
  - Read SMS text and sender's number.
  - Delete SMS by index or category (read, unread, sent, etc.).
  - Check for new unread SMS.
  
- **Hardware Reset**:  
  Perform a hardware reset for SIM800 modules.

- **Configuration Options**:  
  - Set SMS format to Text or PDU mode.
  - Configure SMS indications.
  - Adjust SMS service parameters (validity period, data coding, etc.).

## Requirements

- **Hardware**: This library supports only SIM800-based GSM modules.  
- **Dependencies**: The [TinyGSM](https://github.com/vshymanskyy/TinyGSM) library is required.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/giminotron5/ExtendedTinyGSM.git
   ```
   
2. Include the library in your project.

## API Reference
### Constructor
```cpp
ExtendedTinyGSM(Stream &stream);
```
Create an instance of ExtendedTinyGSM with the specified communication stream (e.g., SoftwareSerial).

### Hardware Reset
```cpp
bool hardwareReset(uint8_t resetPin, bool resetLogic);
```
Perform a hardware reset on the SIM800 module.
Parameters:
- **resetPin**: GPIO pin connected to the module's reset pin.
- **resetLogic**: Reset signal logic (e.g., LOW for active low reset).

### SMS Functions
#### Set SMS Format
```cpp
bool setSmsMessageFormat(bool format);
```
Set SMS mode to Text (true) or PDU (false).

#### Check for New SMS
```cpp
int checkForNewSMS();
```
Check for unread SMS. Returns the index of the SMS or -1 if none.

#### Get SMS Details
```cpp
String getSmsNumber(int smsIndex);
String getSmsText(int smsIndex);
```
Get the sender's number and text of the SMS at the given index.

#### Delete SMS
```cpp
bool deleteSmsByMode(SmsDeleteMode mode);
bool deleteSmsByIndex(int smsIndex);
```
Delete SMS by category or specific index.

## Example Usage
```cpp
#include <SoftwareSerial.h>
#include <ExtendedTinyGSM.h>
#include <TinyGsmClient.h>

SoftwareSerial gsmSerial(10, 11); // RX, TX
ExtendedTinyGSM gsm(gsmSerial);
TinyGsmClient client(static_cast<TinyGsm &>(gsm));

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);

  // Reset the GSM module
  if (gsm.hardwareReset(9, LOW)) {
    Serial.println("Module restarted successfully.");
  }

  // Set SMS format to Text mode
  if (gsm.setSmsMessageFormat(true)) {
    Serial.println("SMS format set to Text mode.");
  }

  // Check for new SMS
  int smsIndex = gsm.checkForNewSMS();
  if (smsIndex > 0) {
    Serial.println("New SMS received!");

    String sender = gsm.getSmsNumber(smsIndex);  
    String message = gsm.getSmsText(smsIndex);  

    Serial.println("Sender: " + sender);  
    Serial.println("Message: " + message);  
  }
}

void loop() {
  // Your logic here
}
```

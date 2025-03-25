
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

// Define the pins for the Voice Recognition Module
VR myVR(2, 3);  // RX on pin 2, TX on pin 3

uint8_t records[7]; // save record
uint8_t buf[64];

int relayPin = 8;  // Define relay module pin

#define onRecord    (0)
#define offRecord   (1) 

void printSignature(uint8_t *buf, int len) {
  for (int i = 0; i < len; i++) {
    if (buf[i] > 0x19 && buf[i] < 0x7F) {
      Serial.write(buf[i]);
    } else {
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

void printVR(uint8_t *buf) {
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");
  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if (buf[0] == 0xFF) {
    Serial.print("NONE");
  } else if (buf[0] & 0x80) {
    Serial.print("UG ");
    Serial.print(buf[0] & (~0x80), DEC);
  } else {
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  
  Serial.print("\t");
  Serial.print(buf[1], DEC);
  Serial.print("\t\t");

  if (buf[3] > 0) {
    printSignature(buf + 4, buf[3]);
  } else {
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup() {
  myVR.begin(9600);
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl Relay Sample");

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Assuming LOW activates the relay, HIGH turns it off

  if (myVR.clear() == 0) {
    Serial.println("Recognizer cleared.");
  } else {
    Serial.println("VoiceRecognitionModule not found.");
    Serial.println("Please check connection and restart Arduino.");
    while (1);
  }

  if (myVR.load((uint8_t)onRecord) >= 0) {
    Serial.println("onRecord loaded");
  }

  if (myVR.load((uint8_t)offRecord) >= 0) {
    Serial.println("offRecord loaded");
  }
}

void loop() {
  int ret = myVR.recognize(buf, 50);
  if (ret > 0) {
    switch (buf[1]) {
      case onRecord:
        Serial.println("Relay ON");
        digitalWrite(relayPin, LOW); // Activate relay
        break;
      case offRecord:
        Serial.println("Relay OFF");
        digitalWrite(relayPin, HIGH); // Deactivate relay
        break;
      default:
        Serial.println("Record function undefined");
        break;
    }
    printVR(buf);
  }
}

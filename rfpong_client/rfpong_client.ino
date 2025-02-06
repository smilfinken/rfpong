#define RADIO_ID 1

#include "rfpong_common.h"

#include <SPI.h>
#include <RF24.h>

Ui ui(STATUS_BAD, STATUS_GOOD);
Data data(RADIO_ID);
RF24 radio(CE_PIN, CSN_PIN);

Payload payload(data.getAddress());
bool radioInitialized = false;

void initSerial() {
  Serial.begin(115200);
  sleep(1);
  Serial.println("starting up");
}

void initRadio() {
  Serial.println("initalizing radio");

  radioInitialized = false;
  if (radio.begin()) {
    radioInitialized = true;
    ui.goodStatus();

    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(sizeof(payload));
  } else {
    Serial.println(F("radio hardware is not responding!!"));
    ui.badStatus();
  }
}

uint64_t sendRequest() {
  if (!radioInitialized) {
    return 0;
  }

  radio.stopListening();
  radio.openWritingPipe(data.getAddress());
  if (radio.write(&payload, sizeof(payload))) {
    return millis();
  } else {
    return 0;
  }
}

void waitForResponse(uint64_t const &startTime) {
  if (!radioInitialized) {
    return;
  }
  
  radio.startListening();

  radio.openReadingPipe(data.getRadioId(), data.getAddress());
  bool gotResponse = false;
  for (int currentTime = millis(); !gotResponse && currentTime < startTime + 2000; currentTime = millis()) {
    uint8_t pipe;
    if (radio.available(&pipe)) {
      uint64_t tof = startTime - currentTime;
      ui.goodStatus();
      uint8_t bytes = radio.getPayloadSize();
      radio.read(&payload, bytes);
      Serial.print(F("Received "));
      Serial.print(bytes);
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);
      Serial.print(F(", counter = "));
      Serial.print(payload.counter);
      Serial.printf(F("got %d bytes on %d after %d ms, counter = %d"), bytes, pipe, tof, payload.counter);
      delay(250);
      ui.noStatus();
    }
  }

  radio.stopListening();
}

void setup() {
  initSerial();
  initRadio();
}

void loop() {
  uint64_t requestTime = sendRequest();
  if (requestTime) {
    ui.goodStatus();
    waitForResponse(requestTime);
    payload.counter++;
    delay(4999);
  } else {
    ui.badStatus();
    delay(999);
  }

  while(1);
  
  delay(1);
}

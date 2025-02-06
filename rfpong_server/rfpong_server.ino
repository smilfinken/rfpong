#define RADIO_ID 0

#include "rfpong_common.h"

#include <SPI.h>
#include <RF24.h>

Ui ui(STATUS_BAD, STATUS_GOOD);
Data data(RADIO_ID);
RF24 radio(CE_PIN, CSN_PIN);

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
    radio.setPayloadSize(sizeof(Payload));

    const uint8_t pipes = data.getSize();
    for (uint8_t i = 0; i < pipes; i++) {
      Serial.printf(F("pipe %d\n"), i);
      radio.openReadingPipe(i, data.getAddress(i));
    }
  } else {
    Serial.println(F("radio hardware is not responding!!"));
    ui.badStatus();
  }
}

void sendResponse(Payload const &payload) {
  if (!radioInitialized) {
    return;
  }

  radio.stopListening();
  radio.openWritingPipe(payload.address);
  if (!radio.write(&payload, sizeof(payload))) {
    Serial.println(F("failed to send response"));
    ui.badStatus();
  }

}

void waitForRequest() {
  if (!radioInitialized) {
    return;
  }

  radio.startListening();
  uint8_t pipe;
  while (!radio.available(&pipe)) {
    ui.goodStatus();

    uint8_t bytes = radio.getPayloadSize();
    Payload payload;
    radio.read(&payload, bytes);

    Serial.printf(F("received &d bytes on pipe %d, counter = %d\n"), bytes, pipe, payload.counter);
    sendResponse(payload);
  }

  ui.noStatus();
}

void setup() {
  initSerial();
  initRadio();
}

void loop() {
  waitForRequest();
  delay(1);
}
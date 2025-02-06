#ifndef STATUS_BAD
#define STATUS_BAD 25
#endif

#ifndef STATUS_GOOD
#define STATUS_GOOD 26
#endif

#ifndef CE_PIN
#define CE_PIN 21
#endif

#ifndef CSN_PIN
#define CSN_PIN 22
#endif

#ifndef RADIO_ID
#define RADIO_ID 0
#endif

struct Payload {
  Payload() = default;

  Payload(uint64_t address) {
    this->address = address;
    this->counter = 0;
  }

  uint64_t address;
  uint64_t counter;
};

class Ui {
  private:
    uint8_t _bad = 0;
    uint8_t _good = 0;

  public:
    Ui(uint8_t bad, uint8_t good) {
      _bad = bad;
      _good = good;

      pinMode(_bad, OUTPUT);
      pinMode(_good, OUTPUT);
      noStatus();
    }

    void noStatus() {
      digitalWrite(_bad, LOW);
      digitalWrite(_good, LOW);
    }

    void goodStatus() {
      digitalWrite(_bad, LOW);
      digitalWrite(_good, HIGH);
    }

    void badStatus() {
      digitalWrite(_good, LOW);
      digitalWrite(_bad, HIGH);
    }
};

class Data {
  private:
    const uint64_t address[6] = { 0x5F62617365LL,
                                  0x5F73617431LL,
                                  0x5F73617432LL,
                                  0x5F73617433LL,
                                  0x5F73617434LL,
                                  0x5F73617435LL };
    uint8_t _radioId = 0;

  public:
    Data(uint8_t radioId) {
      _radioId = radioId;
    }

    uint8_t getSize() {
      return std::size(address);
    }

    uint64_t getAddress() {
      return address[_radioId];
    }

    uint64_t getAddress(uint8_t index) {
      return address[index];
    }

    uint8_t getRadioId() {
      return _radioId;
    }
};

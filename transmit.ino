#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>

#define JOY_X_PIN A0
#define JOY_Y_PIN A1
#define JOY_BUTTON_PIN 2

#define JOY_X_ORIGIN 512
#define JOY_Y_ORIGIN 512

struct controls_state {
  int joy_x_axis = 0;
  int joy_y_axis = 0;
  bool joy_button = 0;
  bool joy_x_direction = 0;
  bool joy_y_direction = 0;
} cstate;

void joy_init() {
  pinMode(JOY_X_PIN, INPUT);
  pinMode(JOY_Y_PIN, INPUT);
  pinMode(JOY_BUTTON_PIN, INPUT_PULLUP);
}

void joy_update() {
  int x = analogRead(JOY_X_PIN) - JOY_X_ORIGIN;
  int y = analogRead(JOY_Y_PIN) - JOY_Y_ORIGIN;

  cstate.joy_x_axis = x;
  cstate.joy_y_axis = y;

  cstate.joy_button = digitalRead(JOY_BUTTON_PIN);
}

void joy_dump() {
  Serial.println("Directon vector is (" + String(cstate.joy_x_axis) + ", " +
                 String(cstate.joy_y_axis) + "), button state is " +
                 String(cstate.joy_button));
}

#define RADIO_CE 8
#define RADIO_CSN 9
#define RADIO_CHANNEL 13
#define RADIO_TRUBA 0x1234567890LL
#define RADIO_DATA_RATE RF24_1MBPS
#define RADIO_POWER RF24_PA_HIGH

#define RADIO_CONNECTION_PIN 3
#define RADIO_DATARATE_PIN 4

RF24 radio(RADIO_CE, RADIO_CSN);

void radio_init() {
  pinMode(RADIO_CONNECTION_PIN, OUTPUT);
  pinMode(RADIO_DATARATE_PIN, OUTPUT);

  radio.begin();

  if (!radio.isChipConnected()) {
    digitalWrite(RADIO_CONNECTION_PIN, LOW);
    Serial.println("11111");
    for (;;)
      ;
  } else {
    digitalWrite(RADIO_CONNECTION_PIN, HIGH);
  }

  radio.begin();
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RADIO_DATA_RATE);
  radio.setPALevel(RADIO_POWER);
  radio.openWritingPipe(RADIO_TRUBA);
}

void radio_send_cstate() {
  if (radio.write(&cstate, sizeof(controls_state))) {
    digitalWrite(RADIO_DATARATE_PIN, HIGH);
  } else {
    digitalWrite(RADIO_DATARATE_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  joy_init();
  radio_init();
}
void loop() {
  delay(10);

  joy_update();
  joy_dump();

  radio_send_cstate();
}

#include "config.h"
#include "fader.h"


char input[16];
int inputIndex = 0;

Fader* faders[NUM_FADERS];

void setup() {
  for(int i = 0; i < NUM_FADERS; i++) {
    faders[i] = new Fader(i, IN1_PINS[i], IN2_PINS[i], PWM_PINS[i], WIPER_PINS[i]);
  }
  
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  Serial.begin(115200);
  Serial.setTimeout(10);

  analogReadResolution(10);
}

void loop() {
  readSerial();
  for (int i = 0; i < NUM_FADERS; i++) {
    Fader* fader = faders[i];
    int value;
    bool changed = fader->update(value);
    if(changed) {
      Serial.printf("%d:%d\n", i, value);
    }
  }
}

void readSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      input[inputIndex] = '\0';
      
      char* token = strtok(input, ":");
      int index = atoi(token);

      token = strtok(NULL, ":");
      int value = atoi(token);

      if(index < NUM_FADERS) {
        faders[index]->moveTo(value);
      }

      inputIndex = 0;
    } else if (c != '\r' && inputIndex < 15) {
      input[inputIndex++] = c;
    }
  }
}

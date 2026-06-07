#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "config.h"
#include "fader.h"
#include "packet.h"

char input[16];
int inputIndex = 0;

Fader* faders[NUM_FADERS];

void setup() {
  for(int i = 0; i < NUM_FADERS; i++) {
    faders[i] = new Fader(i, IN1_PINS[i], IN2_PINS[i], PWM_PINS[i], WIPER_PINS[i], MAC_ADDRESSES[i]);
  }
  
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  analogReadResolution(10);

  Serial.begin(115200);
  Serial.setTimeout(10);

  setupESPNow();
}

void setupESPNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);

  esp_now_init();

  esp_now_register_recv_cb(onReceive);

  for(int i = 0; i < NUM_FADERS; i++) {
    faders[i]->setupESPNow();
  }

  Serial.print("MacAddress: ");
  Serial.println(WiFi.macAddress());
}

void broadcastEvent(MasterEvent event, String payload) {
  for(int i = 0; i < NUM_FADERS; i++) {
    faders[i]->sendEvent(event, payload);
  }
}

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  Packet packet;
  memcpy(&packet, data, sizeof(packet));

  Serial.print("Recieved: ");
  Serial.print(packet.event);
  Serial.print(":");
  Serial.println(packet.payload);

  if(packet.event == MUTE) {
    faders[atoi(packet.payload)]->moveTo(0);
  }

  //temp test
  if(packet.event == BUTTON) {
    Serial.println(packet.payload);
    if(std::stoi(packet.payload) == 0) {
      faders[0]->moveTo(1024);
    }
    else if(std::stoi(packet.payload) == 1) {
      faders[1]->moveTo(1024);
    }
  }
}

void loop() {
  readSerial();
  for (int i = 0; i < NUM_FADERS; i++) {
    Fader* fader = faders[i];
    int value;
    bool changed = fader->update(value);
    if(changed) {
      Serial.printf("%d:%d\n", i, value);
      String payload = String(i) + ":" + String(value);
      broadcastEvent(CHANGE_VOLUME, payload);
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

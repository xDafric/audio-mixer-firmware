#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

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

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, masterMac, 6);
  peer.channel = 6;
  peer.encrypt = false;

  esp_now_add_peer(&peer);

  Serial.println(WiFi.macAddress());
}

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  char msg[64];
  memcpy(msg, data, len);
  msg[len] = '\0';

  char* colon = strchr(msg, ':');
  if (colon != nullptr) {
    *colon = '\0';              // String hier trennen

    char* left = msg;           // vor :
    char* right = colon + 1;    // nach :

    int id = atoi(left);
    int value = atoi(right);

    faders[id]->moveTo(value);
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

      char msg[64];
      snprintf(msg, sizeof(msg), "%d:%d", i, value);
      esp_now_send(masterMac, (uint8_t*)msg, strlen(msg) + 1);
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

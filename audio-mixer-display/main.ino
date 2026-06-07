#include "main.h"

#if ENABLE_BUTTONS
  int buttonStates[BUTTON_ROW_COUNT][BUTTON_COL_COUNT];
#endif

void afterSetup() {
  setupESPNow();

  #if ENABLE_BUTTONS
    for(int i = 0; i < BUTTON_ROW_COUNT; i++) {
      pinMode(BUTTON_ROWS[i], OUTPUT);
      digitalWrite(BUTTON_ROWS[i], HIGH);
    }

    for(int i = 0; i < BUTTON_COL_COUNT; i++) {
      pinMode(BUTTON_COLS[i], INPUT_PULLUP);
    }

    for (int row = 0; row < BUTTON_ROW_COUNT; row++) {
      for (int col = 0; col < BUTTON_COL_COUNT; col++) {
        buttonStates[row][col] = 1;
      }
    }
  #endif
}

void run() {
  #if ENABLE_BUTTONS
    for (int row = 0; row < BUTTON_ROW_COUNT; row++) {

      //all rows HIGH
      for (int i = 0; i < BUTTON_ROW_COUNT; i++) {
        digitalWrite(BUTTON_ROWS[i], HIGH);
      }

      digitalWrite(BUTTON_ROWS[row], LOW);

      delay(10);

      for (int col = 0; col < BUTTON_COL_COUNT; col++) {
        int pressed = digitalRead(BUTTON_COLS[col]);

        if(pressed == 0 && buttonStates[row][col] == 1) {
          Serial.printf("Pressed: %d", BUTTON_IDS[row][col]);
          Serial.println();

          sendEvent(BUTTON, String(BUTTON_IDS[row][col]));
        }

        buttonStates[row][col] = pressed;
      }
    }
  #endif
}

static void channel1Mute(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if(code == LV_EVENT_CLICKED) {
    Serial.println("Mute Channel 0");
    sendEvent(MUTE, "0");
    lv_label_set_text(ui_Channel1Vol, "0%");
  }
}

static void channel2Mute(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if(code == LV_EVENT_CLICKED) {
    Serial.println("Mute Channel 1");
    sendEvent(MUTE, "1");
    lv_label_set_text(ui_Channel2Vol, "0%");
  }
}

void setupESPNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);

  Serial.println(WiFi.macAddress());

  esp_now_init();

  esp_now_register_recv_cb(onReceive);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, masterMac, 6);
  peer.channel = 6;
  peer.encrypt = false;

  esp_now_add_peer(&peer);

  Serial.print("MacAddress: ");
  Serial.println(WiFi.macAddress());
}


void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  Packet packet;
  memcpy(&packet, data, sizeof(packet));

  Serial.print("Recieved: ");
  Serial.print(packet.event);
  Serial.print(":");
  Serial.println(packet.payload);

  if(packet.event == CHANGE_VOLUME) {
    char* colon = strchr(packet.payload, ':');
    if (colon != nullptr) {
      *colon = '\0';              // String hier trennen

      char* left = packet.payload;           // vor :
      char* right = colon + 1;    // nach :

      int id = atoi(left);
      int value = atoi(right);

      int val = constrain(map(value, 10, 1020, 0, 100), 0, 100);
      String text = String(val) + "%";

      if(id == 0) {
        lv_label_set_text(ui_Channel1Vol, text.c_str());
      }
      else if (id == 1) {
        lv_label_set_text(ui_Channel2Vol, text.c_str());
      }
    }
  }
}

void sendEvent(ClientEvent event, String payload) {
  Packet packet;

  packet.event = event;
  strncpy(packet.payload, payload.c_str(), sizeof(packet.payload) - 1);
  packet.payload[sizeof(packet.payload) - 1] = '\0';

  Serial.print(packet.event);
  Serial.print(":");
  Serial.println(packet.payload);
  esp_now_send(masterMac, (uint8_t*)&packet, sizeof(packet));
}
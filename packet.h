#pragma once

enum ClientEvent {
  BUTTON,
  MUTE
};

enum MasterEvent {
  CHANGE_VOLUME
};

struct Packet {
  uint8_t event;
  char payload[64];
};
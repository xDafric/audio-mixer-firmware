#pragma once

#include "config.h"
#include "../packet.h"

void afterSetup();
void run();

void setupESPNow();
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len);

void sendEvent(ClientEvent event, String payload);
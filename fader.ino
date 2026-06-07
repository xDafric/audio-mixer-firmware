#include "fader.h";

Fader::Fader(int index, int IN1, int IN2, int PWM, int WIPER, uint8_t macAddress[]) {
  Fader::index = index;
  Fader::IN1 = IN1;
  Fader::IN2 = IN2;
  Fader::PWM = PWM;
  Fader::WIPER = WIPER;
  memcpy(this->macAddress, macAddress, 6);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(WIPER, INPUT);

  value = readWiper();

  target = 512;
  moving = true;
  delayCount = 0;
}


int Fader::readWiper() {
  int sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += analogRead(WIPER);
  }
  return sum / 16;
}

void Fader::setupESPNow() {
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, macAddress, 6);
  peer.channel = 6;
  peer.encrypt = false;

  esp_now_add_peer(&peer);
}

void Fader::sendEvent(MasterEvent event, String payload) {
  Packet packet;

  packet.event = event;
  strncpy(packet.payload, payload.c_str(), sizeof(packet.payload) - 1);
  packet.payload[sizeof(packet.payload) - 1] = '\0';

  Serial.print("Outgoing Event for " + String(index) + ": ");
  Serial.print(packet.event);
  Serial.print(":");
  Serial.println(packet.payload);
  esp_now_send(macAddress, (uint8_t*)&packet, sizeof(packet));
}


void Fader::moveTo(int pos) {
  target = constrain(pos, 0, 1020);
  moving = true;
}

bool Fader::update(int& outValue) {
  pos = readWiper();
  updateFader();
  if(abs(pos - value) > DEADBAND) {
    value = pos;
    if(!moving) {
      outValue = value;
      return true;
    }
  }
  return false;
}

void Fader::updateFader() {
  int error = target - pos;

  if(!moving) {
    return;
  }

  if (abs(error) < DEADBAND) {
    brakeMotor();
    delayCount += 1;
    if(delayCount > 100) {
      moving = false;
      delayCount = 0;
      return;
    }
    return;
  }

  delayCount = 0;

  int pwm = map(constrain(abs(error), 0, 100), 0, 100, 65, 140);

  if (error > 0) {
    moveUp(pwm);
  } else {
    moveDown(pwm);
  }
}


void Fader::moveUp(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(PWM, speed);
}

void Fader::moveDown(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(PWM, speed);
}

void Fader::stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(PWM, 0);
}

void Fader::brakeMotor() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  analogWrite(PWM, 0);
}

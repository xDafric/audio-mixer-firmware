#define AIN1 16
#define AIN2 4
#define PWMA 18
#define STBY 23
#define FADER_PIN 34

bool moving = true;
int target = 512;
int deadband = 5;

int value;

int delayCount = 0;

char input[16];
int inputIndex = 0;

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  Serial.begin(115200);
  Serial.setTimeout(10);

  analogReadResolution(10);

  value = analogRead(FADER_PIN);
}

void loop() {
  readSerial();
  int pos = readStable(FADER_PIN);
  updateFader(pos);
  if(abs(pos - value) > deadband) {
    changeValue(pos);
  }
}

int readStable(int pin) {
  int sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += analogRead(pin);
  }
  return sum / 16;
}

void readSerial() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      input[inputIndex] = '\0';
      target = constrain(atoi(input), 0, 1020);
      moving = true;
      inputIndex = 0;
    } else if (c != '\r' && inputIndex < 15) {
      input[inputIndex++] = c;
    }
  }
}

void updateFader(int pos) {
  int error = target - pos;

  if(!moving) {
    return;
  }

  if (abs(error) < deadband) {
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

void changeValue(int newValue) {
  value = newValue;
  if(!moving) {
    Serial.println(value);
  }
}

void moveUp(int speed) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, speed);
}

void moveDown(int speed) {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speed);
}

void stopMotor() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 0);
}

void brakeMotor() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 0);
}
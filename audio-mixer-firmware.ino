#define AIN1 6
#define AIN2 7
#define PWMA 5
#define STBY 8
#define FADER_PIN A0

bool moving = true;
int target = 512;
int deadband = 5;

int value;

int delayCount = 0;

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  Serial.begin(9600);
  Serial.setTimeout(10);

  value = analogRead(FADER_PIN);
}

void loop() {
  readSerial();
  updateFader();
  int pos = analogRead(FADER_PIN);
  if(abs(pos - value) > deadband) {
    changeValue(pos);
  }
}

void readSerial() {
  if (Serial.available()) {
    target = Serial.parseInt();
    moving = true;
  }
}

void updateFader() {
  int pos = analogRead(FADER_PIN);
  int error = target - pos;

  if(!moving) {
    return;
  }

  if (abs(error) < deadband) {
    brakeMotor();
    delayCount += 1;
    if(delayCount > 500) {
      moving = false;
      delayCount = 0;
      return;
    }
    return;
  }

  delayCount = 0;

  int pwm = map(constrain(abs(error), 0, 100), 0, 100, 60, 140);

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
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speed);
}

void moveDown(int speed) {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, speed);
}

void stopMotor() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);
}

void brakeMotor() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 0);
}
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

constexpr uint8_t PHOTO_PIN = A1;
constexpr uint8_t IR_PIN = 12;
constexpr uint8_t DOOR_PIN = 2;
constexpr uint8_t GAS_PIN = A3;
constexpr uint8_t TEMP_PIN = A5;

constexpr uint8_t GSM_RX = 10;
constexpr uint8_t GSM_TX = 11;

constexpr uint8_t LUSTER_PIN = 3;
constexpr uint8_t LUSTER_COUNT = 12;

constexpr uint8_t WAY_PIN = 7;
constexpr uint8_t WAY_COUNT = 4;

constexpr uint8_t LINE_COUNT = 5;
constexpr uint8_t LINE_PIXELS = 4;
constexpr uint8_t TEXT_FIRE_WIDTH = 27;
constexpr uint8_t TEXT_EXIT_WIDTH = 16;

constexpr uint8_t PIXELS1_PIN = 6;
constexpr uint8_t PIXELS2_PIN = 5;
constexpr uint8_t PIXELS3_PIN = 4;
constexpr uint8_t PIXELS4_PIN = 8;
constexpr uint8_t PIXELS5_PIN = 9;

constexpr int TEMP_ALARM = 30;
constexpr int TEMP_RESET = 27;
constexpr int GAS_ALARM = 500;
constexpr int GAS_RESET = 430;

constexpr uint8_t DOOR_CLOSED = 0;
constexpr uint8_t DOOR_OPEN = 90;
constexpr uint8_t DOOR_WARNING = 30;

constexpr uint8_t MIN_BRIGHTNESS = 20;
constexpr uint8_t MAX_BRIGHTNESS = 255;
constexpr uint8_t NIGHT_LUSTER_BRIGHTNESS = 45;
constexpr uint8_t DAY_LUSTER_BRIGHTNESS = 30;

constexpr unsigned long SENSOR_INTERVAL = 250;
constexpr unsigned long STATUS_INTERVAL = 1000;
constexpr unsigned long EXIT_INTERVAL = 300;
constexpr unsigned long GAS_INTERVAL = 250;
constexpr unsigned long MOTION_HOLD_TIME = 2500;
constexpr unsigned long ALARM_STABLE_RESET_TIME = 10000;
constexpr unsigned long SMS_REPEAT_TIME = 300000;
constexpr unsigned long SERVO_WARNING_INTERVAL = 2000;
constexpr unsigned long FIRE_FRAME_INTERVAL = 120;
constexpr unsigned long GSM_BOOT_DELAY = 1000;

const char PHONE[] = "+380XXXXXXXXX";

enum class SystemState : uint8_t {
  Normal,
  Motion,
  Alarm
};

struct SensorData {
  int temp = 0;
  int gas = 0;
  uint8_t light = 255;
  bool motion = false;
};

Servo doorServo;
SoftwareSerial gsm(GSM_RX, GSM_TX);

Adafruit_NeoPixel luster(LUSTER_COUNT, LUSTER_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel way(WAY_COUNT, WAY_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel line0(LINE_PIXELS, PIXELS1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel line1(LINE_PIXELS, PIXELS2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel line2(LINE_PIXELS, PIXELS3_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel line3(LINE_PIXELS, PIXELS4_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel line4(LINE_PIXELS, PIXELS5_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel *lines[LINE_COUNT] = {
  &line0,
  &line1,
  &line2,
  &line3,
  &line4
};

const uint8_t TEXT_FIRE[LINE_COUNT][TEXT_FIRE_WIDTH] PROGMEM = {
  {0,1,1,1,0,0,1,1,1,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,1,1,0},
  {0,1,0,1,0,0,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,0,0,1,0,1,0},
  {0,1,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,1,1,1,0,0,1,1,1,0},
  {0,1,0,1,0,0,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,0,0,1,0,0,0},
  {0,1,0,1,0,0,1,1,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0}
};

const uint8_t TEXT_EXIT[LINE_COUNT][TEXT_EXIT_WIDTH] PROGMEM = {
  {1,1,1,0,1,0,0,0,1,0,1,0,1,1,1,0},
  {1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0},
  {1,1,1,0,0,0,1,0,0,0,1,0,0,1,0,0},
  {1,0,0,0,0,1,0,1,0,0,1,0,0,1,0,0},
  {1,1,1,0,1,0,0,0,1,0,1,0,0,1,0,0}
};

SystemState state = SystemState::Normal;
SensorData sensors;

unsigned long lastSensorRead = 0;
unsigned long lastStatusPrint = 0;
unsigned long lastExitUpdate = 0;
unsigned long lastGasUpdate = 0;
unsigned long lastMotionTime = 0;
unsigned long alarmSafeSince = 0;
unsigned long lastSmsTime = 0;
unsigned long lastServoWarning = 0;
unsigned long lastFireFrame = 0;

uint8_t fireOffset = 0;
uint8_t wayStep = 0;
bool doorIsOpen = false;
bool alarmSmsWasSent = false;

int tempFiltered = 0;
int gasFiltered = 0;
int lightFiltered = 255;

uint32_t rgb(Adafruit_NeoPixel &strip, uint8_t r, uint8_t g, uint8_t b) {
  return strip.Color(r, g, b);
}

int smoothValue(int current, int next, uint8_t factor) {
  return ((current * factor) + next) / (factor + 1);
}

int readTemperatureRaw() {
  int raw = analogRead(TEMP_PIN);
  float voltage = raw * 5.0 / 1023.0;
  return round((voltage - 0.5) * 100.0);
}

int readGasRaw() {
  return analogRead(GAS_PIN);
}

uint8_t readLightRaw() {
  int raw = analogRead(PHOTO_PIN);
  int value = map(raw, 1017, 348, 255, 1);
  return constrain(value, 1, 255);
}

bool readMotionRaw() {
  return digitalRead(IR_PIN) == HIGH;
}

void readSensors() {
  int tempRaw = readTemperatureRaw();
  int gasRaw = readGasRaw();
  int lightRaw = readLightRaw();

  tempFiltered = smoothValue(tempFiltered, tempRaw, 3);
  gasFiltered = smoothValue(gasFiltered, gasRaw, 4);
  lightFiltered = smoothValue(lightFiltered, lightRaw, 3);

  sensors.temp = tempFiltered;
  sensors.gas = gasFiltered;
  sensors.light = constrain(lightFiltered, 1, 255);
  sensors.motion = readMotionRaw();

  if (sensors.motion) {
    lastMotionTime = millis();
  }
}

bool alarmCondition() {
  return sensors.temp >= TEMP_ALARM || sensors.gas >= GAS_ALARM;
}

bool safeCondition() {
  return sensors.temp <= TEMP_RESET && sensors.gas <= GAS_RESET;
}

void openDoor() {
  if (!doorIsOpen) {
    doorServo.write(DOOR_OPEN);
    doorIsOpen = true;
  }
}

void closeDoor() {
  if (doorIsOpen) {
    doorServo.write(DOOR_CLOSED);
    doorIsOpen = false;
  }
}

void forceCloseDoor() {
  doorServo.write(DOOR_CLOSED);
  doorIsOpen = false;
}

void clearWay() {
  way.clear();
  way.show();
}

void clearText() {
  for (uint8_t i = 0; i < LINE_COUNT; i++) {
    lines[i]->clear();
    lines[i]->show();
  }
}

void clearAllPixels() {
  clearText();
  clearWay();
  luster.clear();
  luster.show();
}

uint8_t adaptiveBrightness() {
  uint8_t value = sensors.light;

  if (value < MIN_BRIGHTNESS) {
    return MIN_BRIGHTNESS;
  }

  return value;
}

void updateLuster() {
  uint8_t brightness = adaptiveBrightness();

  if (sensors.light > 220) {
    brightness = DAY_LUSTER_BRIGHTNESS;
  } else if (sensors.light < 35) {
    brightness = NIGHT_LUSTER_BRIGHTNESS;
  }

  luster.setBrightness(brightness);

  for (uint8_t i = 0; i < LUSTER_COUNT; i++) {
    luster.setPixelColor(i, rgb(luster, 233, 250, 47));
  }

  luster.show();
}

void showExit() {
  uint8_t brightness = adaptiveBrightness();

  for (uint8_t row = 0; row < LINE_COUNT; row++) {
    lines[row]->setBrightness(brightness);

    for (uint8_t col = 0; col < LINE_PIXELS; col++) {
      uint8_t enabled = pgm_read_byte(&TEXT_EXIT[row][col]);
      lines[row]->setPixelColor(col, rgb(*lines[row], enabled ? 255 : 0, 0, 0));
    }

    lines[row]->show();
  }
}

void showGasLevel() {
  uint8_t level = constrain(map(sensors.gas, 200, 900, 0, WAY_COUNT), 0, WAY_COUNT);
  way.setBrightness(adaptiveBrightness());

  for (uint8_t i = 0; i < WAY_COUNT; i++) {
    if (i < level) {
      way.setPixelColor(i, rgb(way, 255, 80, 0));
    } else {
      way.setPixelColor(i, rgb(way, 0, 0, 0));
    }
  }

  way.show();
}

void showMotionWay() {
  way.setBrightness(adaptiveBrightness());

  for (uint8_t i = 0; i < WAY_COUNT; i++) {
    way.setPixelColor(i, rgb(way, 255, 0, 0));
  }

  way.show();
}

void showFireFrame(uint8_t offset) {
  for (uint8_t row = 0; row < LINE_COUNT; row++) {
    lines[row]->setBrightness(MAX_BRIGHTNESS);

    for (uint8_t col = 0; col < LINE_PIXELS; col++) {
      uint8_t enabled = pgm_read_byte(&TEXT_FIRE[row][offset + col]);
      lines[row]->setPixelColor(col, rgb(*lines[row], enabled ? 255 : 0, 0, 0));
    }

    lines[row]->show();
  }
}

void animateFire() {
  if (millis() - lastFireFrame < FIRE_FRAME_INTERVAL) {
    return;
  }

  lastFireFrame = millis();

  showFireFrame(fireOffset);

  way.clear();
  way.setBrightness(MAX_BRIGHTNESS);
  way.setPixelColor(wayStep, rgb(way, 255, 0, 0));
  way.show();

  wayStep = (wayStep + 1) % WAY_COUNT;
  fireOffset++;

  if (fireOffset > TEXT_FIRE_WIDTH - LINE_PIXELS) {
    fireOffset = 0;
  }
}

void warningServoPulse() {
  if (millis() - lastServoWarning < SERVO_WARNING_INTERVAL) {
    return;
  }

  lastServoWarning = millis();
  doorServo.write(DOOR_WARNING);
  delay(100);
  doorServo.write(DOOR_OPEN);
}

void gsmCommand(const __FlashStringHelper *cmd, unsigned long waitTime) {
  gsm.println(cmd);
  delay(waitTime);
}

void initGsm() {
  delay(GSM_BOOT_DELAY);
  gsmCommand(F("AT"), 1000);
  gsmCommand(F("AT+CMGF=1"), 1000);
}

void sendSms(const __FlashStringHelper *message) {
  unsigned long now = millis();

  if (alarmSmsWasSent && now - lastSmsTime < SMS_REPEAT_TIME) {
    return;
  }

  gsmCommand(F("AT+CMGF=1"), 500);

  gsm.print(F("AT+CMGS=\""));
  gsm.print(PHONE);
  gsm.println(F("\""));
  delay(700);

  gsm.print(message);
  delay(300);
  gsm.write(26);
  delay(4000);

  lastSmsTime = now;
  alarmSmsWasSent = true;
}

void printStatus() {
  Serial.print(F("State: "));

  if (state == SystemState::Normal) {
    Serial.print(F("NORMAL"));
  } else if (state == SystemState::Motion) {
    Serial.print(F("MOTION"));
  } else {
    Serial.print(F("ALARM"));
  }

  Serial.print(F(" | Temp: "));
  Serial.print(sensors.temp);
  Serial.print(F(" | Gas: "));
  Serial.print(sensors.gas);
  Serial.print(F(" | Light: "));
  Serial.print(sensors.light);
  Serial.print(F(" | IR: "));
  Serial.println(sensors.motion);
}

void enterNormal() {
  state = SystemState::Normal;
  alarmSmsWasSent = false;
  alarmSafeSince = 0;
  fireOffset = 0;
  wayStep = 0;
  forceCloseDoor();
  clearWay();
}

void enterMotion() {
  state = SystemState::Motion;
  openDoor();
  showMotionWay();
}

void enterAlarm() {
  state = SystemState::Alarm;
  openDoor();
  clearText();
  luster.setBrightness(MAX_BRIGHTNESS);

  for (uint8_t i = 0; i < LUSTER_COUNT; i++) {
    luster.setPixelColor(i, rgb(luster, 255, 255, 255));
  }

  luster.show();

  if (sensors.gas >= GAS_ALARM && sensors.temp >= TEMP_ALARM) {
    sendSms(F("Увага! Виявлено дим або газ і високу температуру."));
  } else if (sensors.gas >= GAS_ALARM) {
    sendSms(F("Увага! Виявлено дим або газ у приміщенні."));
  } else {
    sendSms(F("Увага! Температура перевищила безпечний рівень."));
  }
}

void handleNormal() {
  updateLuster();

  if (millis() - lastGasUpdate >= GAS_INTERVAL) {
    lastGasUpdate = millis();
    showGasLevel();
  }

  if (millis() - lastExitUpdate >= EXIT_INTERVAL) {
    lastExitUpdate = millis();
    showExit();
  }

  if (alarmCondition()) {
    enterAlarm();
    return;
  }

  if (sensors.motion) {
    enterMotion();
    return;
  }
}

void handleMotion() {
  updateLuster();
  showMotionWay();

  if (alarmCondition()) {
    enterAlarm();
    return;
  }

  if (millis() - lastMotionTime > MOTION_HOLD_TIME) {
    clearWay();
    forceCloseDoor();
    state = SystemState::Normal;
  }
}

void handleAlarm() {
  openDoor();
  warningServoPulse();
  animateFire();

  if (sensors.gas >= GAS_ALARM && sensors.temp >= TEMP_ALARM) {
    sendSms(F("Увага! Небезпека триває: дим або газ і висока температура."));
  } else if (sensors.gas >= GAS_ALARM) {
    sendSms(F("Увага! Небезпека триває: дим або газ у приміщенні."));
  } else if (sensors.temp >= TEMP_ALARM) {
    sendSms(F("Увага! Небезпека триває: висока температура."));
  }

  if (safeCondition()) {
    if (alarmSafeSince == 0) {
      alarmSafeSince = millis();
    }

    if (millis() - alarmSafeSince >= ALARM_STABLE_RESET_TIME) {
      enterNormal();
    }
  } else {
    alarmSafeSince = 0;
  }
}

void setup() {
  Serial.begin(115200);
  gsm.begin(9600);

  pinMode(PHOTO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);

  luster.begin();
  way.begin();

  for (uint8_t i = 0; i < LINE_COUNT; i++) {
    lines[i]->begin();
    lines[i]->clear();
    lines[i]->show();
  }

  luster.clear();
  luster.show();

  way.clear();
  way.show();

  doorServo.attach(DOOR_PIN);
  forceCloseDoor();

  tempFiltered = readTemperatureRaw();
  gasFiltered = readGasRaw();
  lightFiltered = readLightRaw();

  initGsm();
}

void loop() {
  unsigned long now = millis();

  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;
    readSensors();
  }

  if (now - lastStatusPrint >= STATUS_INTERVAL) {
    lastStatusPrint = now;
    printStatus();
  }

  switch (state) {
    case SystemState::Normal:
      handleNormal();
      break;

    case SystemState::Motion:
      handleMotion();
      break;

    case SystemState::Alarm:
      handleAlarm();
      break;
  }
}

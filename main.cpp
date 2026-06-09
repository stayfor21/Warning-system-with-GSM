#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

constexpr bool USE_GSM = true;
constexpr bool USE_BUZZER = true;

constexpr uint8_t PHOTO_PIN = A1;
constexpr uint8_t IR_PIN = 12;
constexpr uint8_t DOOR_PIN = 2;
constexpr uint8_t GAS_PIN = A3;
constexpr uint8_t TEMP_PIN = A5;
constexpr uint8_t BUZZER_PIN = A0;

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
constexpr int TEMP_SENSOR_MIN = -55;
constexpr int TEMP_SENSOR_MAX = 150;

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
constexpr unsigned long FAULT_BLINK_INTERVAL = 500;
constexpr unsigned long BUZZER_INTERVAL = 350;
constexpr unsigned long GSM_BOOT_DELAY = 1000;

const char PHONE[] = "+380XXXXXXXXX";

enum class SystemState : uint8_t {
  Normal,
  Motion,
  Alarm,
  Fault
};

enum class AlarmReason : uint8_t {
  None,
  Gas,
  Temperature,
  GasAndTemperature
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
unsigned long lastFaultBlink = 0;
unsigned long lastBuzzerUpdate = 0;

uint8_t fireOffset = 0;
uint8_t exitOffset = 0;
uint8_t wayStep = 0;

bool doorIsOpen = false;
bool alarmSmsWasSent = false;
bool gsmReady = false;
bool faultBlinkState = false;
bool buzzerState = false;

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
  sensors.gas = constrain(gasFiltered, 0, 1023);
  sensors.light = constrain(lightFiltered, 1, 255);
  sensors.motion = readMotionRaw();

  if (sensors.motion) {
    lastMotionTime = millis();
  }
}

AlarmReason getAlarmReason() {
  bool gasDanger = sensors.gas >= GAS_ALARM;
  bool tempDanger = sensors.temp >= TEMP_ALARM;

  if (gasDanger && tempDanger) {
    return AlarmReason::GasAndTemperature;
  }

  if (gasDanger) {
    return AlarmReason::Gas;
  }

  if (tempDanger) {
    return AlarmReason::Temperature;
  }

  return AlarmReason::None;
}

bool alarmCondition() {
  return getAlarmReason() != AlarmReason::None;
}

bool safeCondition() {
  return sensors.temp <= TEMP_RESET && sensors.gas <= GAS_RESET;
}

bool sensorErrorCondition() {
  return sensors.temp < TEMP_SENSOR_MIN || sensors.temp > TEMP_SENSOR_MAX;
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

void clearLuster() {
  luster.clear();
  luster.show();
}

void clearAllPixels() {
  clearText();
  clearWay();
  clearLuster();
}

uint8_t adaptiveBrightness() {
  if (sensors.light < MIN_BRIGHTNESS) {
    return MIN_BRIGHTNESS;
  }

  return sensors.light;
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

void showExitFrame(uint8_t offset) {
  uint8_t brightness = adaptiveBrightness();

  for (uint8_t row = 0; row < LINE_COUNT; row++) {
    lines[row]->setBrightness(brightness);

    for (uint8_t col = 0; col < LINE_PIXELS; col++) {
      uint8_t index = (offset + col) % TEXT_EXIT_WIDTH;
      uint8_t enabled = pgm_read_byte(&TEXT_EXIT[row][index]);
      lines[row]->setPixelColor(col, rgb(*lines[row], enabled ? 255 : 0, 0, 0));
    }

    lines[row]->show();
  }
}

void animateExit() {
  if (millis() - lastExitUpdate < EXIT_INTERVAL) {
    return;
  }

  lastExitUpdate = millis();
  showExitFrame(exitOffset);
  exitOffset = (exitOffset + 1) % TEXT_EXIT_WIDTH;
}

void showGasLevel() {
  uint8_t level = constrain(map(sensors.gas, 200, 900, 0, WAY_COUNT), 0, WAY_COUNT);
  way.setBrightness(adaptiveBrightness());

  for (uint8_t i = 0; i < WAY_COUNT; i++) {
    way.setPixelColor(i, i < level ? rgb(way, 255, 80, 0) : rgb(way, 0, 0, 0));
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

void stopBuzzer() {
  if (USE_BUZZER) {
    noTone(BUZZER_PIN);
    buzzerState = false;
  }
}

void updateAlarmBuzzer() {
  if (!USE_BUZZER) {
    return;
  }

  if (millis() - lastBuzzerUpdate < BUZZER_INTERVAL) {
    return;
  }

  lastBuzzerUpdate = millis();
  buzzerState = !buzzerState;

  if (buzzerState) {
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
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

bool waitForGsm(const char *expected, unsigned long timeout) {
  String response = "";
  unsigned long start = millis();

  while (millis() - start < timeout) {
    while (gsm.available()) {
      char c = gsm.read();
      response += c;

      if (response.indexOf(expected) >= 0) {
        return true;
      }
    }
  }

  return false;
}

bool sendGsmCommand(const __FlashStringHelper *cmd, const char *expected, unsigned long timeout) {
  while (gsm.available()) {
    gsm.read();
  }

  gsm.println(cmd);
  return waitForGsm(expected, timeout);
}

bool checkGsm() {
  if (!USE_GSM) {
    Serial.println(F("[GSM] Simulation mode enabled"));
    return true;
  }

  Serial.println(F("[GSM] Checking module"));

  bool ok = true;
  ok = ok && sendGsmCommand(F("AT"), "OK", 2000);
  ok = ok && sendGsmCommand(F("ATE0"), "OK", 2000);
  ok = ok && sendGsmCommand(F("AT+CMGF=1"), "OK", 2000);
  ok = ok && sendGsmCommand(F("AT+CPIN?"), "READY", 3000);
  ok = ok && sendGsmCommand(F("AT+CSQ"), "OK", 2000);
  ok = ok && sendGsmCommand(F("AT+CREG?"), "OK", 3000);

  if (ok) {
    Serial.println(F("[GSM] Module ready"));
  } else {
    Serial.println(F("[GSM] Module not ready"));
  }

  return ok;
}

void initGsm() {
  delay(GSM_BOOT_DELAY);
  gsmReady = checkGsm();
}

void sendSms(const __FlashStringHelper *message) {
  unsigned long now = millis();

  if (alarmSmsWasSent && now - lastSmsTime < SMS_REPEAT_TIME) {
    return;
  }

  if (!USE_GSM) {
    Serial.print(F("[SMS SIMULATION] "));
    Serial.println(message);
    lastSmsTime = now;
    alarmSmsWasSent = true;
    return;
  }

  if (!gsmReady) {
    gsmReady = checkGsm();

    if (!gsmReady) {
      Serial.println(F("[GSM] SMS was not sent"));
      return;
    }
  }

  Serial.println(F("[GSM] Sending SMS"));

  if (!sendGsmCommand(F("AT+CMGF=1"), "OK", 2000)) {
    gsmReady = false;
    Serial.println(F("[GSM] SMS mode error"));
    return;
  }

  while (gsm.available()) {
    gsm.read();
  }

  gsm.print(F("AT+CMGS=\""));
  gsm.print(PHONE);
  gsm.println(F("\""));

  if (!waitForGsm(">", 3000)) {
    gsmReady = false;
    Serial.println(F("[GSM] Phone number command error"));
    return;
  }

  gsm.print(message);
  delay(300);
  gsm.write(26);

  if (waitForGsm("OK", 12000)) {
    Serial.println(F("[GSM] SMS sent"));
    lastSmsTime = now;
    alarmSmsWasSent = true;
  } else {
    gsmReady = false;
    Serial.println(F("[GSM] SMS sending failed"));
  }
}

void printAlarmReason(AlarmReason reason) {
  if (reason == AlarmReason::GasAndTemperature) {
    Serial.print(F("GAS_AND_TEMPERATURE"));
  } else if (reason == AlarmReason::Gas) {
    Serial.print(F("GAS"));
  } else if (reason == AlarmReason::Temperature) {
    Serial.print(F("TEMPERATURE"));
  } else {
    Serial.print(F("NONE"));
  }
}

void printStatus() {
  Serial.print(F("State: "));

  if (state == SystemState::Normal) {
    Serial.print(F("NORMAL"));
  } else if (state == SystemState::Motion) {
    Serial.print(F("MOTION"));
  } else if (state == SystemState::Alarm) {
    Serial.print(F("ALARM"));
  } else {
    Serial.print(F("FAULT"));
  }

  Serial.print(F(" | Reason: "));
  printAlarmReason(getAlarmReason());
  Serial.print(F(" | Temp: "));
  Serial.print(sensors.temp);
  Serial.print(F(" | Gas: "));
  Serial.print(sensors.gas);
  Serial.print(F(" | Light: "));
  Serial.print(sensors.light);
  Serial.print(F(" | IR: "));
  Serial.print(sensors.motion);
  Serial.print(F(" | GSM: "));
  Serial.println(gsmReady ? F("READY") : F("NOT_READY"));
}

void enterNormal() {
  state = SystemState::Normal;
  alarmSmsWasSent = false;
  alarmSafeSince = 0;
  fireOffset = 0;
  wayStep = 0;
  stopBuzzer();
  forceCloseDoor();
  clearWay();
  Serial.println(F("[STATE] NORMAL"));
}

void enterMotion() {
  state = SystemState::Motion;
  openDoor();
  showMotionWay();
  Serial.println(F("[STATE] MOTION"));
}

void enterFault() {
  state = SystemState::Fault;
  stopBuzzer();
  forceCloseDoor();
  clearWay();
  clearText();
  Serial.println(F("[STATE] FAULT"));
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

  AlarmReason reason = getAlarmReason();

  Serial.print(F("[STATE] ALARM | Reason: "));
  printAlarmReason(reason);
  Serial.println();

  if (reason == AlarmReason::GasAndTemperature) {
    sendSms(F("Увага! Виявлено дим або газ і високу температуру."));
  } else if (reason == AlarmReason::Gas) {
    sendSms(F("Увага! Виявлено дим або газ у приміщенні."));
  } else if (reason == AlarmReason::Temperature) {
    sendSms(F("Увага! Температура перевищила безпечний рівень."));
  }
}

void handleNormal() {
  updateLuster();

  if (millis() - lastGasUpdate >= GAS_INTERVAL) {
    lastGasUpdate = millis();
    showGasLevel();
  }

  animateExit();

  if (sensorErrorCondition()) {
    enterFault();
    return;
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

  if (sensorErrorCondition()) {
    enterFault();
    return;
  }

  if (alarmCondition()) {
    enterAlarm();
    return;
  }

  if (millis() - lastMotionTime > MOTION_HOLD_TIME) {
    clearWay();
    forceCloseDoor();
    state = SystemState::Normal;
    Serial.println(F("[STATE] NORMAL"));
  }
}

void handleAlarm() {
  openDoor();
  warningServoPulse();
  animateFire();
  updateAlarmBuzzer();

  AlarmReason reason = getAlarmReason();

  if (reason == AlarmReason::GasAndTemperature) {
    sendSms(F("Увага! Небезпека триває: дим або газ і висока температура."));
  } else if (reason == AlarmReason::Gas) {
    sendSms(F("Увага! Небезпека триває: дим або газ у приміщенні."));
  } else if (reason == AlarmReason::Temperature) {
    sendSms(F("Увага! Небезпека триває: висока температура."));
  }

  if (sensorErrorCondition()) {
    enterFault();
    return;
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

void handleFault() {
  if (millis() - lastFaultBlink >= FAULT_BLINK_INTERVAL) {
    lastFaultBlink = millis();
    faultBlinkState = !faultBlinkState;

    luster.setBrightness(MAX_BRIGHTNESS);

    for (uint8_t i = 0; i < LUSTER_COUNT; i++) {
      luster.setPixelColor(i, faultBlinkState ? rgb(luster, 255, 0, 0) : rgb(luster, 0, 0, 0));
    }

    luster.show();
  }

  if (!sensorErrorCondition()) {
    enterNormal();
  }
}

void setup() {
  Serial.begin(115200);
  gsm.begin(9600);

  pinMode(PHOTO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);

  if (USE_BUZZER) {
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);
  }

  luster.begin();
  way.begin();

  for (uint8_t i = 0; i < LINE_COUNT; i++) {
    lines[i]->begin();
    lines[i]->clear();
    lines[i]->show();
  }

  clearAllPixels();

  doorServo.attach(DOOR_PIN);
  forceCloseDoor();

  tempFiltered = readTemperatureRaw();
  gasFiltered = readGasRaw();
  lightFiltered = readLightRaw();

  readSensors();
  initGsm();

  Serial.println(F("[SYSTEM] Started"));
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

    case SystemState::Fault:
      handleFault();
      break;
  }
}

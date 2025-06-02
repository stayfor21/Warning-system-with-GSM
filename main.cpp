#define photoSensor A1 
#define IKDatchik 12
#define doorPin 2
#define gasPin A3
#define tempPin A5

#define doorPixelsPin 7
#define doorPixelsCount 4

#define tempFire 30
#define gasFire 500

#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <SoftwareSerial.h>
Servo ServoDoor;

SoftwareSerial gsm(10, 11);

#define lusterPin 3
#define lusterCount 12
Adafruit_NeoPixel luster = Adafruit_NeoPixel(lusterCount, lusterPin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel wayPixels = Adafruit_NeoPixel(doorPixelsCount, doorPixelsPin, NEO_GRB + NEO_KHZ800);

#define pixels1Pin 6
#define pixels2Pin 5
#define pixels3Pin 4
#define pixels4Pin 8
#define pixels5Pin 9
#define pixelsCount 4
#define sizeLines 5

Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(pixelsCount, pixels1Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(pixelsCount, pixels2Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3 = Adafruit_NeoPixel(pixelsCount, pixels3Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels4 = Adafruit_NeoPixel(pixelsCount, pixels4Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels5 = Adafruit_NeoPixel(pixelsCount, pixels5Pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lines[5] = {pixels1, pixels2, pixels3, pixels4, pixels5};

int TEXT_FIRE[5][27] = {
  {0,1,1,1,0,0,1,1,1,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,1,1,0},
  {0,1,0,1,0,0,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,0,0,1,0,1,0},
  {0,1,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,0,1,1,1,0,0,1,1,1,0},
  {0,1,0,1,0,0,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,0,0,1,0,0,0},
  {0,1,0,1,0,0,1,1,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0},
};

int TEXT_EXIT[5][27] = {
  {1,1,1,0,1,0,0,0,1,0,1,0,1,1,1,0},
  {1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0},
  {1,1,1,0,0,0,1,0,0,0,1,0,0,1,0,0},
  {1,0,0,0,0,1,0,1,0,0,1,0,0,1,0,0},
  {1,1,1,0,1,0,0,0,1,0,1,0,0,1,0,0},
};

void setup() {
  Serial.begin(115200);
  gsm.begin(9600);
  delay(1000);
  gsm.println("AT");
  delay(1000);
  gsm.println("AT+CMGF=1");
  delay(1000);
  
  pinMode(IKDatchik, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(gasPin, INPUT); 

  pinMode(pixels1Pin, OUTPUT); pinMode(pixels2Pin, OUTPUT);
  pinMode(pixels3Pin, OUTPUT); pinMode(pixels4Pin, OUTPUT);
  pinMode(pixels5Pin, OUTPUT); pinMode(doorPixelsPin, OUTPUT);

  wayPixels.begin();
  for (int i = 0; i < sizeLines; i++) {
    lines[i].begin();
  }

  luster.begin();
  ServoDoor.attach(doorPin);
  closeDoor();
}

void sendAlertSMS(String message) {
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+380XXXXXXXXX\"");
  delay(1000);
  gsm.print(message);
  delay(500);
  gsm.write(26);
  delay(5000);
}

int getTemperature() {
  int reading = analogRead(tempPin);
  float voltage = reading * (5.0 / 1023.0);
  float temperatureC = (voltage - 0.5) * 100.0;
  return (int)temperatureC;
}

int getPhoto() {
  return map(analogRead(photoSensor), 1017, 348, 255, 1);
}

int getIK() {
  return digitalRead(IKDatchik);
}

int getGas() {
  return analogRead(gasPin);
}

void openDoor() {
  ServoDoor.write(90);
}

void closeDoor() {
  ServoDoor.write(0);
}

void textOFF() {
  for (int i = 0; i < sizeLines; i++) {
    for (int j = 0; j < pixelsCount; j++) {
      lines[i].setPixelColor(j, lines[i].Color(0, 0, 0));
    }
    lines[i].show();
  }
}

void textExit() {
  int photo = getPhoto();
  for (int l = 0; l < sizeLines; l++) {
    lines[l].setBrightness(photo);
    for (int j = 0; j < pixelsCount; j++) {
      lines[l].setPixelColor(j, lines[l].Color(255 * TEXT_EXIT[l][j], 0, 0));
    }
    lines[l].show();  
  }
  for (int i = 0; i < doorPixelsCount; i++) {
    wayPixels.setPixelColor(i, wayPixels.Color(0, 0, 0));
  }
  wayPixels.show();
}

void turnOnWayFull() {
  for (int i = 0; i < lusterCount; i++) {
    luster.setPixelColor(i, luster.Color(233, 250, 47));
  }
  luster.show();

  while (getGas() > gasFire || getTemperature() > tempFire) {
    int turn = -1;
    for (int i = 0; i < 27 - pixelsCount; i++) {
      for (int l = 0; l < sizeLines; l++) {
        for (int j = 0; j < pixelsCount; j++) {
          lines[l].setPixelColor(j, lines[l].Color(255 * TEXT_FIRE[l][i + j], 0, 0));
        }
        turn = (turn + 1) % doorPixelsCount;
        wayPixels.setPixelColor(turn, wayPixels.Color(255, 0, 0));
        lines[l].show();
        wayPixels.show();
        delay(100);
        wayPixels.setPixelColor(turn, wayPixels.Color(0, 0, 0));
        wayPixels.show();
      }
    }
  }

  closeDoor();
}

void goodBye() {
  while (getIK()) {
    openDoor();
    int photo = getPhoto();
    wayPixels.setBrightness(photo);
    for (int i = 0; i < sizeLines; i++) {
      lines[i].setBrightness(photo);
      lines[i].show();
    }
    for (int i = 0; i < doorPixelsCount; i++) {
      wayPixels.setPixelColor(i, wayPixels.Color(255, 0, 0));
      wayPixels.show();
    }
    delay(100);
  }
  wayPixels.setBrightness(255);
  for (int i = 0; i < doorPixelsCount; i++) {
    wayPixels.setPixelColor(i, wayPixels.Color(0, 0, 0));
  }
  wayPixels.show();
  closeDoor();
}

void adjustNightBrightness() {
  int photo = getPhoto();
  if (photo > 220) {
    luster.setBrightness(30);
  } else if (photo < 30) {
    luster.setBrightness(50);
  } else {
    luster.setBrightness(photo);
  }
}

void warningServoPulse() {
  for (int i = 0; i < 3; i++) {
    ServoDoor.write(30);
    delay(100);
    ServoDoor.write(0);
    delay(100);
  }
}

void blinkFireText(int times) {
  for (int t = 0; t < times; t++) {
    textOFF();
    delay(300);
    for (int l = 0; l < sizeLines; l++) {
      for (int j = 0; j < pixelsCount; j++) {
        lines[l].setPixelColor(j, lines[l].Color(255 * TEXT_FIRE[l][j], 0, 0));
      }
      lines[l].show();
    }
    delay(300);
  }
}

void gasDangerLevel(int gasVal) {
  int level = map(gasVal, 200, 900, 0, doorPixelsCount);
  for (int i = 0; i < doorPixelsCount; i++) {
    if (i < level)
      wayPixels.setPixelColor(i, wayPixels.Color(255, 0, 0));
    else
      wayPixels.setPixelColor(i, wayPixels.Color(0, 0, 0));
  }
  wayPixels.show();
}

void loop() {
  int temp = getTemperature();
  int photo = getPhoto();
  int gas = getGas();
  int IK = getIK();

  Serial.print("Temp: "); Serial.print(temp);
  Serial.print(" | Light: "); Serial.print(photo);
  Serial.print(" | Gas: "); Serial.print(gas);
  Serial.print(" | IR: "); Serial.println(IK);

  adjustNightBrightness();
  
  for (int i = 0; i < lusterCount; i++) {
    luster.setPixelColor(i, luster.Color(233, 250, 47));
  }
  luster.show();

  gasDangerLevel(gas);

  if (temp > tempFire || gas > gasFire) {
    openDoor();
    textOFF();
    for (int i = 0; i < sizeLines; i++) {
      lines[i].setBrightness(255);
    }
    luster.setBrightness(255);

    if (gas > gasFire) {
      Serial.println("FIRE! Reason: smoke (gas)");
      sendAlertSMS("Увага! Виявлено дим у приміщенні.");
    } else if (temp > tempFire) {
      Serial.println("FIRE! Reason: high temperature");
      sendAlertSMS("Увага! Температура перевищила безпечний рівень.");
    }

    warningServoPulse();
    blinkFireText(5);
    turnOnWayFull();
  } else if (IK == 1) {
    Serial.println("Motion detected opening door.");
    delay(500);
    goodBye();
  } else {
    Serial.println("Normal mode displaying EXIT.");
    textExit();
  }

  delay(400);
}

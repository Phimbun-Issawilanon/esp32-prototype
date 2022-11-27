#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include "BluetoothSerial.h"
#include "time.h"
float valB = 255;
float valR = 255;
float valW = 255;
float valG = 255;
float valV = 255 ;
float valRb = 255;
float valUV = 255;
String modeCheck, rec, v, sub_S, pass, sub_mode, get_time;
float tim, val, end_time, set_time;
const int buttonPin = 26;    // the number of the pushbutton pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int count = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
BluetoothSerial ESP_BT ;

void setup() {
  Serial.begin(115200);
  ESP_BT.begin("Esp32");
  pinMode(buttonPin, INPUT);
  for (int i = 0 ; i <= 6; i++) {
    ledcSetup(i, 5000, 8);
  }
  ledcAttachPin(5, 0);// กำหนดขา led ที่ต้องการควบคุม blue
  ledcAttachPin(18, 1);// กำหนดขา led ที่ต้องการควบคุม white
  ledcAttachPin(16, 2);// กำหนดขา led ที่ต้องการควบคุม green
  ledcAttachPin(4, 3);// กำหนดขา led ที่ต้องการควบคุม violet
  ledcAttachPin(0, 4);// กำหนดขา led ที่ต้องการควบคุมred
  ledcAttachPin(17, 5);// กำหนดขา led ที่ต้องการควบคุม rb
  ledcAttachPin(19, 6);// กำหนดขา led ที่ต้องการควบคุม uv

  for (int i = 0 ; i <= 6; i++) {
    ledcWrite(i, 255);
  }

  WiFiManager wifiManager;
  wifiManager.setTimeout(180);
  if (!wifiManager.autoConnect("Esp32")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  WiFi.printDiag(Serial);
  Serial.println();
  Serial.println("connected...OK");

  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); //ดึงเวลาจาก Server
  Serial.println("\nLoading time");
  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
}

void loop() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  int hour = p_tm->tm_hour;
  float mins = p_tm->tm_min;
  float m = (mins / 60);
  tim  = hour + m;

  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        count++;
        Serial.println(count);
      }
    }
  }
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  if (count == 1) {
    rec = "modeAuto";
  }
  else if (count == 2) {
    rec = "moonlight";
  }
  else if (count == 3) {
    rec = "modeLight" ;
  }
  else if (count == 4) {
    count = 0;
    for (int i = 0 ; i <= 6; i++) {
      ledcWrite(i, 255);
    }
  }
  //Bluetooth
  if (ESP_BT.available()) {
    rec = ESP_BT.readStringUntil('\n');
    sub_S = rec.substring(0, 1);
    int len = rec.length();
    v = rec.substring(1, len);
    val = (100.00 - v.toFloat()) * 2.55;
    int ind = rec.indexOf(',');
    int ind_2 = rec.lastIndexOf(',');
    sub_mode = rec.substring(0, ind);
    pass = rec.substring(ind + 1, ind_2);
    get_time = rec.substring(ind_2 + 1, len);

    end_time = (get_time.toFloat() / 60);
    set_time = end_time - tim;
    delay(10);
  
    if (sub_mode == "modeAuto" and  pass == "esp32") {
      modeCheck = "modeAuto";
      pwmModeauto();
    }
    else if (sub_mode == "moonlight" and  pass == "esp32") {
      modeCheck = "moonlight";
      modeMoonlight();
    }
    else if (sub_mode == "modeLight" and  pass == "esp32") {
      modeCheck = "modeLight";
      pwmModelight();
    }
    else if (sub_mode == "modeManual" and  pass == "esp32") {
      modeCheck = "modeManual";
      ledcWrite(0, valB);
      ledcWrite(1, valW);
      ledcWrite(2, valG);
      ledcWrite(3, valV);
      ledcWrite(4, valR);
      ledcWrite(5, valRb);
      ledcWrite(6, valUV);
      
    }
    setCol();
  }
  if (set_time != 0.00) {
    if (tim == end_time) {
      modeCheck = "modeAuto";
    }
  }
  if (modeCheck == "modeLight") {
    pwmModelight();
  }
  if (modeCheck == "modeAuto") {
    pwmModeauto();
  }
}
void pwmModeauto() {
  ledcWrite(6, 0);
  ledcWrite(4, 0);
  if (1.0 <= tim && tim < 6.0) {
    ledcWrite(0, 255);
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, 255);
    ledcWrite(5, 255);
  }
  if (6.0 <= tim && tim < 10.0) {
    ledcWrite(0, 140.25);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 140.25);
    ledcWrite(5, 140.25);
  }
  if (10.0 <= tim && tim < 12.917) {
    ledcWrite(0, 0);
    ledcWrite(1, 153);
    ledcWrite(2, 216.75);
    ledcWrite(3, 0);
    ledcWrite(5, 0);
  }
  if (12.55 <= tim && tim < 18.25) {
    ledcWrite(0, 0);
    ledcWrite(1, 76.5);
    ledcWrite(2, 191.25);
    ledcWrite(3, 0);
    ledcWrite(5, 0);
  }
  if (18.25 <= tim && tim < 21.45) {
    ledcWrite(0, 0);
    ledcWrite(1, 178.5);
    ledcWrite(2, 229.5);
    ledcWrite(3, 0);
    ledcWrite(5, 0);
  }
  if (21.45 <= tim && tim < 23.15) {
    ledcWrite(0, 0);
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, 0);
    ledcWrite(5, 0);
  }
  if (23.15 <= tim && tim <= 23.59) {
    ledcWrite(0, 63.75);
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, 63.75);
    ledcWrite(5, 63.75);
  }
  if (0.00 <= tim && tim <= 0.59) {
    ledcWrite(0, 63.75);
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, 63.75);
    ledcWrite(5, 63.75);
  }
}
void pwmModelight() {
  ledcWrite(0, 105);
  ledcWrite(2, 255);
  ledcWrite(3, 255);
  ledcWrite(4, 0);
  ledcWrite(5, 105);
  ledcWrite(6, 0);
  ledcWrite(1, 255);
  delay(2000);
  ledcWrite(1, 0);
  delay(1000);
}
void modeMoonlight() {
  ledcWrite(0, 89.25);
  ledcWrite(1, 165.75);
  ledcWrite(2, 255);
  ledcWrite(3, 89.25);
  ledcWrite(4, 255);
  ledcWrite(5, 89.25);
  ledcWrite(6, 127.5);
}
void setCol() {
  if (sub_S == "r") {
    valR = val;
  }
  else if (sub_S == "b") {
    valB = val;
  }
  else if (sub_S == "B") {
    valRb = val;
  }
  else if (sub_S == "w") {
    valW = val;
  }
  else if (sub_S == "g") {
    valG = val;
  }
  else if (sub_S == "v") {
    valV = val;
  }
  else if (sub_S == "u") {
    valUV = val;
  }
}

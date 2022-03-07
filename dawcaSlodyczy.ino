#include <Arduino.h>
#include <U8g2lib.h>
#include <Stepper.h>
#include <SPI.h>
#include "HX711.h"
#include <TimerOne.h>
#include <Wire.h>
#include "RTClib.h"

#define calibration_factor -928.0
#define ILOSC_KROKOW 32
#define DOUT  3
#define CLK  2

int h = 11; 
long m = 20;
long s = 00;
long countdown_time;
unsigned long previous = 0;

U8G2_ST7565_NHD_C12864_F_4W_SW_SPI u8g2(U8G2_R0, 11, 8, 13, 12, U8X8_PIN_NONE);
Stepper mojSilnik(ILOSC_KROKOW, 7, 9, 10, 6); // piny steerujace silnikiem
HX711 scale;
RTC_DS1307 rtc;

int buzzer_pin = 5;

struct MusicStruct {
  int A = 550;
  int As = 582;
  int B = 617;
  int C = 654;
  int Cs = 693;
  int D = 734;
  int Ds = 777;
  int E = 824;
  int F = 873;
  int Fs = 925;
  int G = 980;
  int Gs = 1003;
  int A2 = 1100;
  int A2s = 1165;
  int B2 = 1234;
  int C3 = 1308;
  int C3s = 1385;
  int D3 = 1555;
}Music;

struct LengthStruct {
  float half = 0.5; //0.5
  float one = 1.0; //1.0
  float one_half = 1.5; //1.5
  float two = 2; //2
  float two_half = 2.5; //2.5
}Length;

int tempo = 400; //400

void initial(){
  DateTime now = rtc.now();
  long currentTime = (now.hour()*3600 + now.minute() * 60 + now.second());
  long expectedTime = (h*3600 + m*60 + s);
  if(expectedTime > currentTime){
    countdown_time = expectedTime - currentTime;
  }else if(expectedTime < currentTime) {
    countdown_time =  (expectedTime - currentTime) +86400;
    }
}


void setup() {
  u8g2.begin();
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();
  Serial.begin(9600);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  initial();
  pinMode(buzzer_pin, OUTPUT);
}

void playMusic() {
  setTone(buzzer_pin, Music.B, tempo * Length.one);
  setTone(buzzer_pin, Music.E, tempo * Length.one_half);
  setTone(buzzer_pin, Music.G, tempo * Length.half);
  setTone(buzzer_pin, Music.F, tempo * Length.one);
  setTone(buzzer_pin, Music.E, tempo * Length.two);
  setTone(buzzer_pin, Music.B2, tempo * Length.one);
  setTone(buzzer_pin, Music.A2, tempo * Length.two_half);
  setTone(buzzer_pin, Music.Fs, tempo * Length.two_half);
  
  setTone(buzzer_pin, Music.E, tempo * Length.one_half);
  setTone(buzzer_pin, Music.G, tempo * Length.half);
  setTone(buzzer_pin, Music.F, tempo * Length.one);
  setTone(buzzer_pin, Music.Ds, tempo * Length.two);
  setTone(buzzer_pin, Music.F, tempo * Length.one);
  setTone(buzzer_pin, Music.B, tempo * Length.two_half);
}

void setTone(int pin, int note, int duration) {
  tone(pin, note, duration);
  delay(duration);
  noTone(pin);
}

void loop() {
  u8g2.clearBuffer(); 
  long countdowntime_seconds = countdown_time - ((millis() / 1000) - previous);
  Serial.print(countdown_time - ((millis() / 1000) - previous));
  scale.set_scale(calibration_factor); 
  float units;//, ounces;

  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }

 u8g2.setFont(u8g2_font_helvR08_tf);
  if(countdowntime_seconds >= 86400) {
    countdowntime_seconds-=86400;
  }
  if (countdowntime_seconds >= 0) {
    long countdown_hour = countdowntime_seconds / 3600;
    long countdown_minute = ((countdowntime_seconds / 60)%60);
    long countdown_sec = countdowntime_seconds % 60;
    u8g2.drawStr( 20, 20, "Czas do wydania...");
    u8g2.setCursor(40,40);
    if (countdown_hour < 10) {
        u8g2.print("0");
    }
    u8g2.print(countdown_hour);
    u8g2.print(":");
    if (countdown_minute < 10) {
        u8g2.print("0");
    }
    u8g2.print(countdown_minute);
    u8g2.print(":");
      if (countdown_sec < 10) {
        u8g2.print("0");
    }
    u8g2.print(countdown_sec);
    if (countdowntime_seconds == 0) {
      delay(100);
      mojSilnik.setSpeed(300); 
      mojSilnik.step(-342);
      playMusic();
    }
    u8g2.setCursor(30,60);
    u8g2.print(units);
    u8g2.drawStr( 70,60, "gramy");
    u8g2.sendBuffer(); 
  }
}

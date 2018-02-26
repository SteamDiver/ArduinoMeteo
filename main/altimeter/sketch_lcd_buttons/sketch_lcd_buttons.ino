#include <stdint.h>
#include "Wire.h"
#include "SPI.h"
#include <LiquidCrystal.h>
#include <Adafruit_BMP085.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7 );

double zeroPressure = 0;

//Global sensor object
Adafruit_BMP085 altimeter;

int button;
const int BUTTON_NONE   = 0;
const int BUTTON_RIGHT  = 1;
const int BUTTON_UP     = 2;
const int BUTTON_DOWN   = 3;
const int BUTTON_LEFT   = 4;
const int BUTTON_SELECT = 5;


int getPressedButton() {
  int buttonValue = analogRead(0); // считываем значения с аналогового входа(A0)
  if (buttonValue < 100) {
    return BUTTON_RIGHT;
  }
  else if (buttonValue < 200) {
    return BUTTON_UP;
  }
  else if (buttonValue < 400) {
    return BUTTON_DOWN;
  }
  else if (buttonValue < 600) {
    return BUTTON_LEFT;
  }
  else if (buttonValue < 800) {
    return BUTTON_SELECT;
  }
  return BUTTON_NONE;
}

void setup()
{
  
  lcd.begin(16,2);
  lcd.print("Hello!");
  delay(1000);
  
  altimeter.begin();
}

void loop()
{
    button = getPressedButton();  //get pressed button
    switch (button) {
    case BUTTON_SELECT:
     ResetPressure();
    }

    Altimeter();
    
 delay(1000);
}

void Altimeter(){
  if (zeroPressure == 0){
    zeroPressure = altimeter.readPressure();
  }
  double pressure = altimeter.readPressure() * 0.00750062;
  double altitude = altimeter.readAltitude(zeroPressure);

  lcd.clear();
  
  lcd.setCursor(0,0);
 
  lcd.print("Pressure:");
  lcd.print(pressure);
    

   lcd.setCursor(0,1);
   lcd.print("Alt:");
   lcd.print(altitude);
   lcd.setCursor(15,1);
   lcd.print("m");
}

void ResetPressure(){
  lcd.clear();
  lcd.print("Reset");
  delay(1000);
  zeroPressure = altimeter.readPressure();
}

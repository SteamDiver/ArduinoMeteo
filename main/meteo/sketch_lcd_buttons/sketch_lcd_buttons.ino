#include <EEPROM.h>
#include <stdint.h>
#include "SparkFunBME280.h"
#include "Wire.h"
#include "SPI.h"
#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 2   // what digital pin we're connected to
LiquidCrystal lcd(8, 9, 4, 5, 6, 7 );
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

//Display mode
// 0 -> DHT; 1 -> BME280; -1-> calibrating
int displayMode = 0;

//Global sensor object
BME280 mySensor;
DHT dht(DHTPIN, DHTTYPE);

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

//Calibrate indexes
double tempIndexBME = 0.0;
double tempIndexDHT = 0.0;

void setup()
{
  
  lcd.begin(16,2);
  lcd.print("Hello :)");
  delay(1000);
  //***Driver settings********************************//
  //commInterface can be I2C_MODE or SPI_MODE
  //specify chipSelectPin using arduino pin names
  //specify I2C address.  Can be 0x77(default) or 0x76
  //For I2C, enable the following and disable the SPI section
  
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76; //Адрес датчика
  
  //For SPI enable the following and dissable the I2C section
  //mySensor.settings.commInterface = SPI_MODE;
  //mySensor.settings.chipSelectPin = 10;

  //***Operation settings*****************************//
  
  //renMode can be:
  //  0, Sleep mode
  //  1 or 2, Forced mode
  //  3, Normal mode
  mySensor.settings.runMode = 3; //В примере предлагают использовать Forced mode, но при обновлении раз в секунду достаточно Normal mode
  
  //tStandby can be:
  //  0, 0.5ms
  //  1, 62.5ms
  //  2, 125ms
  //  3, 250ms
  //  4, 500ms
  //  5, 1000ms
  //  6, 10ms
  //  7, 20ms
  mySensor.settings.tStandby = 0; //Очевидно чаще не нужно
  
  //filter can be off or number of FIR coefficients to use:
  //  0, filter off
  //  1, coefficients = 2
  //  2, coefficients = 4
  //  3, coefficients = 8
  //  4, coefficients = 16
  mySensor.settings.filter = 0;
  
  //tempOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.tempOverSample = 1;

  //pressOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
    mySensor.settings.pressOverSample = 1;
  
  //humidOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.settings.humidOverSample = 1;
    
  //Calling .begin() causes the settings to be loaded
  mySensor.begin();
  dht.begin();
  //Calibrate(26.5);
  Serial.begin(9600);
}

void loop()
{
    button = getPressedButton();  //get pressed button
    switch (button) {
    case BUTTON_SELECT:
        switch(displayMode){
          case 0: displayMode = 1;
                lcd.clear();
                break;
          case 1: displayMode = 0;
                lcd.clear();
                break;
                               
      
    }
    break;

  }

 switch(displayMode){
  case 0: 
          displayBME();
          break;
  case 1: 
          displayDHT();
          break;
 }
 Serial.println(mySensor.readTempC());
 Serial.println(tempIndexBME);
 delay(500);
}
void displayBME(){
  lcd.setCursor(0,0);
  lcd.print("H=");
  lcd.print((uint8_t)mySensor.readFloatHumidity());
  lcd.print("%");
  lcd.print(" T=");
  lcd.print(mySensor.readTempC() + tempIndexBME, 1);
  lcd.setCursor(13,0);
  
  lcd.setCursor(0,1);
  lcd.print("P:");

  float mmH=mySensor.readFloatPressure()/133;
  lcd.print(mmH, 0);
  lcd.print("mmHg");
  lcd.print(" BME280");
}

void displayDHT(){
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  lcd.setCursor(0,0);
  lcd.print("H=");
  lcd.print(h, 0);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("T=");
  lcd.print(t + tempIndexDHT, 1);
  lcd.print(" DHT");

}

void displayCalibrate(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Cal mode");
  lcd.setCursor(0,1);
}

void Calibrate(double trueTemp){
  float r;
  EEPROM.get(0, r);
  if(r == 0xFFFFFFFF){
    tempIndexBME = trueTemp - mySensor.readTempC();
    tempIndexDHT = trueTemp - dht.readTemperature();
    EEPROM.put(0, tempIndexBME);
    EEPROM.put(sizeof(float), tempIndexDHT);
   
  } else{
    EEPROM.get(0,  tempIndexBME);
    EEPROM.get(sizeof(float),  tempIndexDHT);
  }
  
}




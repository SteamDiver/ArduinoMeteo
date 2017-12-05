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

DHT dht(DHTPIN, DHTTYPE);

//Global sensor object
BME280 mySensor;
//LiquidCrystal_I2C lcd(0x27,16,2); //Адрес дисплея, в моём случае 0x3F

int button;
bool flag = true; // true - bme ; false - DHT
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
  
  lcd.init();
  lcd.backlight();
  lcd.print("hello");
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
  mySensor.settings.tStandby = 5; //Очевидно чаще не нужно
  
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
}

void loop()
{
  //Буквы можно вывести один раз, а далее менять показания, но показания при изменении количества значащих цифр могут сдвигать строку.
  
   button = getPressedButton();
  switch (button) {
    case BUTTON_SELECT:
     if (flag==false){
      flag=true;
      displayBME();
     } else{
      flag=false;
      displayDHT();
     }
      break;

  delay(2500);
  lcd.clear();

  
  delay(2500);
  lcd.clear();
}
void displayBME(){
  lcd.setCursor(0,0);
  lcd.print("H=");
  lcd.print((uint8_t)mySensor.readFloatHumidity());
  lcd.print("%");
  lcd.print(" T=");
  lcd.print(mySensor.readTempC());
  lcd.setCursor(13,0);
  
  lcd.setCursor(0,1);
  lcd.print("P:");

  int mmH=mySensor.readFloatPressure()/133;
  lcd.print(mmH);
  lcd.print("mmHg");
  lcd.print(" BME280");
}
void displayDHT(){
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  lcd.print("H=");
  lcd.print(h);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("T=");
  lcd.print(t);
  lcd.print(" DHT");

}


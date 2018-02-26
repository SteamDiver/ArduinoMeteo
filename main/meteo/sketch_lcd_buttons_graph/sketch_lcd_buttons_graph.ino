#include <EEPROM.h>
#include <stdint.h>
#include "SparkFunBME280.h"
#include "Wire.h"
#include "SPI.h"
#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 2   // what digital pin we're connected to
LiquidCrystal lcd(8, 9, 4, 5, 6, 7 );
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

          #if defined(ARDUINO) && ARDUINO >= 100
          #define printByte(args)  write(args);
          #else
          #define printByte(args)  print(args,BYTE);
          #endif
          
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

//Создаем массив значений на дисплее, от пустого к полному.          
          uint8_t graf0[8] = {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};
          uint8_t graf1[8] = {B00000, B11111, B11111, B11111, B11111, B11111, B11111};
          uint8_t graf2[8] = {B00000, B00000, B11111, B11111, B11111, B11111, B11111};
          uint8_t graf3[8] = {B00000, B00000, B00000, B11111, B11111, B11111, B11111};
          uint8_t graf4[8] = {B00000, B00000, B00000, B00000, B11111, B11111, B11111};
          uint8_t graf5[8] = {B00000, B00000, B00000, B00000, B00000, B11111, B11111};
          uint8_t graf6[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B11111};
          uint8_t graf7[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000};
//Добавили символы для графиков

int stat[2][25]; //Возьмем 2х мерный массив, где один массив время(контроль действительности значения) и второй массив наши данные
//int stat[0]; //Давление
//int stat[1]; //Время

long data = 0;

void setup()
{
  
  lcd.begin(16,2);
  lcd.print("Hello :)");
  delay(1000);
  
  mySensor.settings.commInterface = I2C_MODE;
  mySensor.settings.I2CAddress = 0x76; //Адрес датчика
  
  mySensor.settings.runMode = 3; //В примере предлагают использовать Forced mode, но при обновлении раз в секунду достаточно Normal mode
  
  mySensor.settings.tStandby = 0; //Очевидно чаще не нужно
  
  mySensor.settings.filter = 0;
  

  mySensor.settings.tempOverSample = 1;

  mySensor.settings.pressOverSample = 1;
  
  mySensor.settings.humidOverSample = 1;
    
  //Calling .begin() causes the settings to be loaded
  mySensor.begin();
  dht.begin();
  
  lcd.createChar(0, graf0);// Подключаем доп. символы
  lcd.createChar(1, graf1);
  lcd.createChar(2, graf2);
  lcd.createChar(3, graf3);
  lcd.createChar(4, graf4);
  lcd.createChar(5, graf5);
  lcd.createChar(6, graf6);
  lcd.createChar(7, graf7);
  
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

 delay(500);
}
void displayBME(){
  lcd.setCursor(0,0);
  lcd.print((uint8_t)mySensor.readFloatHumidity());
  lcd.print(" ");
  lcd.print(mySensor.readTempC() + tempIndexBME, 1);
  
  lcd.print(" ");

  float mmH=mySensor.readFloatPressure()/133;
  lcd.print(mmH, 0);

  if (((millis()/1000)-stat[1][23]) >= 1.0) //заполняем текущее значение раз 1 час (3600000)
     {
       //сдвигаем массив влево, чтобы освободить предпоследнюю ячейку
         int i=0;
         for (i = 0; i < 24; i++) stat[0][i] = stat[0][i+1];
         for (i = 0; i < 24; i++) stat[1][i] = stat[1][i+1];
      
      data = mySensor.readTempC(); //Записываем значения давления
      stat[0][23] = data;
      stat[1][23] = millis()/1000; 
      
      grafik(0, 2, 0); //строим график, вызов усложнен для построения разных графиков из массива, первое число номер массива, затем периодичность выборки данных, потом начальное значение в массиве. Мы берем первый массив(0), берем каждое второе значение(2) и начинаем с начала(0)
     }
}

  void grafik(int x, int y, int z) {
        lcd.setCursor(0, 1);
        lcd.print("Dav");
        
           int minx = stat[x][0];
           int maxy = stat[x][0];
            for(int i=z; i<=24; i++) {
              minx = min(minx, stat[x][i]);
              maxy = max(maxy, stat[x][i]);
            }
           for (int i=z; i <= 24; i= i + y){
              if (stat[1][i] == 0){
                lcd.print("-");                                   //если значений нет
              } else {
                
           lcd.print(map(stat[x][i], minx, maxy, 7, 0));
        }
         
  }
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




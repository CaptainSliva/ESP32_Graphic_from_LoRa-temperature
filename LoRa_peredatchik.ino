#include <SPI.h>
#include <LoRa.h>
#include <OneWire.h>
#include "LowPower.h"
OneWire ds(4); // назначение контакта (Data)


float temperature(){
  // Определяем температуру от датчика DS18b20
  byte data[2]; // Место для значения температуры
  
  ds.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
  ds.write(0xCC); // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
  ds.write(0x44); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
  
  delay(1000); // Микросхема измеряет температуру, а мы ждем.  
  
  ds.reset(); // Теперь готовимся получить значение измеренной температуры
  ds.write(0xCC); 
  ds.write(0xBE); // Просим передать нам значение регистров со значением температуры
 
  // Получаем и считываем ответ
  data[0] = ds.read(); // Читаем младший байт значения температуры
  data[1] = ds.read(); // А теперь старший
 
  // Формируем итоговое значение: 
  //    - сперва "склеиваем" значение, 
  //    - затем умножаем его на коэффициент, соответсвующий разрешающей способности (для 12 бит по умолчанию - это 0,0625)
  float temperature =  ((data[1] << 8) | data[0]) * 0.0625;
  
  // Выводим полученное значение температуры в монитор порта
  return (temperature);
  
}

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);  // A3 как выход
   
  while (!Serial);

  Serial.println("LoRa Sender");
  Serial.println("423423423423423");
  LoRa.setSpreadingFactor(12);
  Serial.println("1");
  LoRa.setSignalBandwidth(7.8E3);
  Serial.println("2");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("out of setup");
}

void loop() {
  digitalWrite(3, HIGH);
  Serial.print("Sending packet: ");
//  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
//  LoRa.print("hello ");
  LoRa.print(temperature());
  LoRa.print("@2");   //LoRa.print("@2"); для второго передатчика   и LoRa.print("@1"); для первого
  LoRa.endPacket();


//  delay(60000);
LoRa.sleep();
digitalWrite(3, LOW); 
for(int i=0; i<113; i++){
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
//  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}











//#include <SPI.h>
//#include <LoRa.h>
//#include <OneWire.h>
//int counter = 0;
//
//void setup() {
//  Serial.begin(9600);
//  while (!Serial);
//
//  Serial.println("LoRa Sender");
//  LoRa.setSpreadingFactor(12);
//  LoRa.setSignalBandwidth(7.8E3);
//  if (!LoRa.begin(433E6)) {
//
//    Serial.println("Starting LoRa failed!");
//    while (1);
//  }
//}
//
//void loop() {
//  
//  Serial.print("Sending packet: ");
//  Serial.println(counter);
//
//  // send packet
//  LoRa.beginPacket();
////  LoRa.print("hello ");
//  LoRa.print(counter);
//  LoRa.print("@2");
//  LoRa.endPacket();
//
//  counter++;
//
//  delay(60000);
//}

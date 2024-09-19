#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

String temperature;
int n;

void setup() {
 Wire.begin(2);                /* задаем на шине i2c 2 адрес */
 Wire.onReceive(receiveEvent); /* регистрируем полученное событие */
 Wire.onRequest(requestEvent); /* регистрируем запрошенное событие */
 Serial.begin(9600);           /* открываем серийный порт для дебаггинга */
 while (!Serial);
////
  Serial.println("LoRa Receiver");
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(7.8E3);
  
  if (!LoRa.begin(433E6)) {

    Serial.println("Starting LoRa failed!");
    while (1);
  }
}
 
void loop() {
 int packetSize = LoRa.parsePacket();
//
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    while (LoRa.available()) {
      temperature = LoRa.readString();
    }
    n++;
    Wire.print(temperature);  /* отправляем по запросу строку "Hello NodeMCU" */
    Serial.print(temperature);
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
//    Wire.print(temperature);  /* отправляем по запросу строку "Hello NodeMCU" */

  }
}
 
// Функция для извлечения любых принимаемых данных от мастера на шину
void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
    char c = Wire.read();      /* получаем байт как символ*/
    Serial.print(c);           /* выводим символ в серийный порт */
  }
 Serial.println();             /* переходим на новую строку */
}
 
// Функция для извлечения любых отправляемых данных от мастера на шину
void requestEvent() {
 if (n != 0){
  Wire.print(temperature);  /* отправляем по запросу строку "Hello NodeMCU" */
  n = 0;
// Serial.println(temperature);
 }
}


////
//#include <SPI.h>
//#include <LoRa.h>
//#include <Wire.h>
//
//int val = 0;
//
//void setup() {
// Wire.onReceive(requestEvent); /* регистрируем полученное событие */
////  Wire.onRequest(requestEvent); /* регистрируем запрошенное событие */ 
//  Serial.begin(9600);
//  while (!Serial);
////
//  Serial.println("LoRa Receiver");
//
//  if (!LoRa.begin(433E6)) {
//    Serial.println("Starting LoRa failed!");
//    while (1);
//  }
//}
//
//void loop() {
//  // try to parse packet
//  int packetSize = LoRa.parsePacket();
//
//  if (packetSize) {
//    // received a packet
//    Serial.print("Received packet '");
//
//    // read packet
//    while (LoRa.available()) {
//      String temperature = LoRa.readString();
////      String temperature = String(temp);
//      Serial.print(temperature);
//
//
//    }
//    
//    // print RSSI of packet
//    Serial.print("' with RSSI ");
//    Serial.println(LoRa.packetRssi());
//  }
//}
//
//void requestEvent() {
// Wire.print(val);  /* отправляем по запросу строку "Hello NodeMCU" */
// val ++;
//}

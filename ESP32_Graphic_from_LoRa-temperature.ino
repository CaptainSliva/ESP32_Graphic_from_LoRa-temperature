

#define BOT_TOKEN "******************"
//#define CHAT_ID "276693613, 5288656086"
#include <FastBot.h>
FastBot bot(BOT_TOKEN);



#include <ESP32Time.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include "soc/rtc_wdt.h"
#include "SPIFFS.h"

//#define I2C_Freq 100000
//#define SDA_0 18

ESP32Time rtc(3600);  // offset in seconds GMT+1
int ss;
int mm;
int hh;
int DD;
int MM;
int YYYY;


const int massiv_size = 200;
const char* wifi_ssid = "***************";
const char* wifi_pass = "**************";
String temperature;
String separator_1 = "~1~";
String separator_2 = "~2~";
String LoRaTemperature_1;
String LoRaTemperature_2;
String temperature_1[massiv_size];
String time_1[massiv_size];
String temperature_2[massiv_size];
String time_2[massiv_size];
String time_;

int j_1;
int j_2;



// Webserver and Websockets setup
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");


// Function to handle request that is not serve by our web server
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// Callback function for our websocket message
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)

{
  if (type == WS_EVT_CONNECT)
  {
    // client connected
    Serial.println("Client connected....");
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
//    client->ping();


    if (j_1 > 0) {
      
      for (int i = 0; i < j_1; i++) {
        if (time_1[i] != "") {
          try {
            String time_temperature = time_1[i] + separator_1 + temperature_1[i];
            webSocket.printfAll(time_temperature.c_str());
             
            Serial.println(temperature_1[i]);
            Serial.println();
          }
          catch (String error) {
            Serial.println("Отправка данных с массива 1 не удалась "+ time_1[i] + separator_1 + temperature_1[i]);
          }
          
       }
//       delay(10);
      }
    }
    if (j_2 > 0) {
      for (int i = 0; i < j_2; i++) {
        if (time_2[i] != "") {
          try {
            String time_temperature = time_2[i] + separator_2 + temperature_2[i];
            webSocket.printfAll(time_temperature.c_str());
          
            Serial.println(temperature_2[i]);
            Serial.println();
          }
          catch (String error) {
            Serial.println("Отправка данных с массива 2 не удалась "+ time_2[i] + separator_2 + temperature_2[i]);
          }
          
        }
//        delay(10);
      }
    }
    //      for (int i=0; i<sizeof(temperature_2[2]); i++){
    //        String time_temperature = temperature_2[3][i]+separator+temperature_2[2][i];
    //        webSocket.printfAll(time_temperature.c_str());
    //      }
    //    }
    //  for (int i=1; i<10; i++){
    //      String sss = DifferenceTime2+"~?"+String(random(10,30));
    //      webSocket.printfAll(sss.c_str());
    //      webSocket.printfAll(("time~?"+String(random(10,30))).c_str());
    //      webSocket.printfAll(("time~?"+String(random(10,30))).c_str());
    // Send a GET request to <ESP_IP>/state

    //      delay(1000);
    //}
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    // client disconnected
    Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    // error was received from the other end
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    // pong message was received (in response to a ping request maybe)
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    // do nothing as client is not sending message to server
    Serial.printf("ws[%s][%u] data received\n", server->url(), client->id());
  }
}


void setup()
{
  
  Serial.begin(115200);
//  rtc_wdt_protect_off();
//  rtc_wdt_disable();
//  
//  disableCore0WDT();
//  disableCore1WDT();
//  disableLoopWDT(); // You forgot this one !
  
//  rtc_wdt_protect_off();    // Turns off the automatic wdt service
//rtc_wdt_enable();         // Turn it on manually
//rtc_wdt_set_time(RTC_WDT_STAGE0, 20000);  // Define how long you desire to let dog wait.

  Serial.println("Starting the Real-time Chart display of Sensor Readings ..");
  rtc.setTime(2, 2, 2, 2, 2, 2022); // ss:mm:hh DD.MM.YYYY
  Wire.begin();



  

  // Begin LittleFS for ESP8266 or SPIFFS for ESP32
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to WIFI
  //  WiFi.mode(WIFI_STA);
  connectWiFi(wifi_ssid, wifi_pass);

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // attach AsyncWebSocket
  webSocket.onEvent(onEvent);
  server.addHandler(&webSocket);

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/graphic_1.html", "text/html");
  });
  server.on("/graphic_1.html", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/graphic_1.html", "text/html");
  });
  // Route for root index.html
  server.on("/graphic_2.html", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/graphic_2.html", "text/html");
  });
  
  // Route for root library.js
  server.on("/chart.min.js", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/chart.min.js", "text/javascript");
  });

  // Route for root index.css
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/index.css", "text/css");
  });

//  // Route for root entireframework.min.css
//  server.on("/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest * request)
//  {
//    request->send(SPIFFS, "/entireframework.min.css", "text/css");
//  });

  // Route for root index.js
  server.on("/index_1.js", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/index_1.js", "text/javascript");
  });
    // Route for root index.js
  server.on("/index_2.js", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/index_2.js", "text/javascript");
  });

  server.onNotFound(notFound);

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam("input1")) {
      inputMessage = request->getParam("input1")->value();
      inputParam = "input1";
    }
    Serial.println(inputMessage); // Получаем данные с get-запроса
    ss = (inputMessage.substring(inputMessage.length() - 2)).toInt();
    mm = (inputMessage.substring(inputMessage.length() - 5, inputMessage.length() - 3)).toInt();
    hh = (inputMessage.substring(inputMessage.length() - 8, inputMessage.length() - 6)).toInt() - 1;
    YYYY = (inputMessage.substring(6, 10)).toInt();
    MM = (inputMessage.substring(3, 5)).toInt();
    DD = (inputMessage.substring(0, 2)).toInt();
//    if (String(hh) != rtc.getTime("%H")) {
      rtc.setTime(ss, mm, hh, DD, MM, YYYY); // ss:mm:hh DD.MM.YYYY
//    }
//    time_ = rtc.getTime("%A, %d.%B.%Y %H:%M:%S");
//    Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));


    request->send(200, "text/html");
  });
  // Start the server
  server.begin();
    // подключаем функцию-обработчик
  bot.attach(newMsg);
  // показать юзер меню (\t - горизонтальное разделение кнопок, \n - вертикальное
  bot.showMenu("Датчик1 \t Датчик2 \n IP-address \t Все датчики");
}





void readI2CPort() {
  bot.tick();   // тикаем в луп
  Wire.requestFrom(2, 7); /* запрашиваем и считываем 7 байт с адреса 2 slave устройства */

  while (Wire.available()) {
    temperature = Wire.readString();

    if (split(temperature, 1) == "1") {
      SendTemperatureOnWebServ(temperature, LoRaTemperature_1, temperature_1, time_1, j_1, separator_1, massiv_size);
      if (j_1 <= massiv_size) {
        j_1+=1;  
      }
      Serial.println(j_1);
    }

    if (split(temperature, 1) == "2") {
      SendTemperatureOnWebServ(temperature, LoRaTemperature_2, temperature_2, time_2, j_2, separator_2, massiv_size);
      if (j_2 <= massiv_size) {
        j_2+=1;  
      }
      Serial.println(j_2);
      
    }

    
    Serial.println(temperature);


    //    DifferenceTime1 = time_ - LastTime1;
    //    DifferenceTime2 = time_ - LastTime2;
    Serial.println();
    delay(1000);

    // Check if value read is different then send a websocket message to the client
//    Serial.println(LoRaTemperature_1);
//    Serial.println(LoRaTemperature_2);



    // adding a little delay
    delay(10);

  }
  
}

void loop() {
  
  readI2CPort();
  delay(10);
  
//      if (Serial.available() > 0)
//      {
//        time_ = rtc.getTime("%A, %d.%B.%Y %H:%M:%S");
//        String s = Serial.readString();
//        temperature_2[j]=s;
//        time_2[j] = time_;
//  
//        for (int i = 0; i<massiv_size; i++){
//          if (time_2[i] != ""){
//            Serial.println(temperature_2[i]);
//            Serial.println(time_2[i]);
//          }
//        }
//        Serial.print("\n\n\n");
//      if (j < massiv_size-1){
//        j+=1;
//      }
//      else {
//        for (int i = 0; i<massiv_size-1; i++) {
//          temperature_2[i] = temperature_2[i+1];
//          time_2[i] = time_2[i+1];
//  
//        }
//        temperature_2[massiv_size-1] = s;
//        time_2[massiv_size-1] = time_;
//  
//      }
//      String time_temperature = time_+separator+s;
//      webSocket.printfAll(time_temperature.c_str());
//  
//      }
}


//Передавать temperature, LoRaTemperature_n, temp_n[массив], time_n, k - счётчик, separator - для понимания кто отправил, massiv_size - максимальный размер массива
void SendTemperatureOnWebServ(String temperature, String LoRaTemperature, String temp[], String tme[], int k, String separator, int massiv_size){
  //Проверка соединения с WiFi
  if (WiFi.status() == WL_CONNECTED) {
    delay(10);
  }
  
  else {
    Serial.println("Произошла потеря WiFi соединения. Переподключаюсь... ");
    connectWiFi(wifi_ssid, wifi_pass);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  //Отправка температуры и времени на страницу
  LoRaTemperature = split(temperature, 0);
  time_ = rtc.getTime("%A, %d.%B.%Y %H:%M:%S");
  temp[k] = LoRaTemperature;
  tme[k] = time_;

  Serial.print("\n\n\n");
  //старый код
//  if (k < massiv_size - 1) {
//    k += 1;
//  }
//  else {
//    for (int i = 1; i < massiv_size - 1; i++) {
//      temp[i] = temp[i];
//      tme[i] = tme[i];
//      //delay(10);
//    }
//    temp[massiv_size - 1] = LoRaTemperature;
//    tme[massiv_size - 1] = time_;
//
//  }
  //новый код
  if (k >= massiv_size) {
    for(int i=1; i<massiv_size; i++){
      temp[i-1] = temp[i];
      tme[i-1] = tme[i];
    }
    temp[massiv_size-1] = LoRaTemperature;
    tme[massiv_size-1] = time_;
  }
  else {
      temp[k] = LoRaTemperature;
      tme[k] = time_;
      //k+=1;
  }
  String time_temperature = time_ + separator + LoRaTemperature;
  webSocket.printfAll(time_temperature.c_str());
  Serial.println("Сообщение от "+separator+" дошло");
  
}



// обработчик сообщений
void newMsg(FB_msg& msg) {
  if (msg.text == "/start") bot.showMenu("Датчик1 \t Датчик2 \n IP-address \t Все датчики", msg.chatID);
  if (msg.text == "Датчик1") bot.sendMessage(message(temperature_1[j_1-1], time_1[j_1-1], "one"), msg.chatID);
  if (msg.text == "Датчик2") bot.sendMessage(message(temperature_2[j_2-1], time_2[j_2-1], "one"), msg.chatID);
  if (msg.text == "Все датчики") bot.sendMessage(message("full", "2", "full"), msg.chatID);
  if (msg.text == "IP-address") bot.sendMessage(message("IP", "IP", "IP"), msg.chatID);
    // выводим ID чата, имя юзера и текст сообщения
//  Serial.print(msg.chatID);     // ID чата 
//  Serial.print(", ");
//  Serial.print(msg.username);   // логин
//  Serial.print(", ");
//  Serial.println(msg.text);     // текст
//  Serial.println(temperature_1[j_1-1]+"| "+time_1[j_1-1]+"\ "+j_1);
//  Serial.println(temperature_2[j_2-1]+"| "+time_2[j_2-1]+"\ "+j_2);
}


String message(String dat, String tim, String how_many){
  String out;
  if (how_many == "one"){
//    out = "t = "+dat+"\n"+"Обновлено в: "+tim.substring(tim.length()-8);
    out = "t = "+dat+"\n"+"Обновлено в: "+tim;
  }
  if (how_many == "full"){
//    out = "t1 = "+temperature_1[j_1-1]+"\n"+"Обновлено в: "+time_1[j_1-1].substring(time_1[j_1-1].length()-8)+"\n\n"+"t2 = "+temperature_2[j_2-1]+"\n"+"Обновлено в: "+time_2[j_2-1].substring(time_2[j_2-1].length()-8);
    out = "t1 = "+temperature_1[j_1-1]+"\n"+"Обновлено в: "+time_1[j_1-1]+"\n\n"+"t2 = "+temperature_2[j_2-1]+"\n"+"Обновлено в: "+time_2[j_2-1];

  }
  if (how_many == "IP"){
    out = WiFi.localIP().toString();
  }
  return(out);
  
}




void connectWiFi(const char* WIFI_SSID, const char* WIFI_PASS ) {
  delay(2000);
  Serial.println("connecting...");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 15000) {
      bot.sendMessage("Вайфай сигнал был потерян", "5288656086");
      ESP.restart();
    }
  }
  Serial.println("Connected!");
}





String split(String s, int n) {
  String out;
  if (n == 0) {
    for (int i = 0; i < s.length(); i++) {
      char g = s[i];
      if (g == '@') {
        break;
      }
      out += g;
    }
  }
  if (n == 1) {
    for (int i = 0; i < s.length(); i++) {
      char g = s[i];
      if (g == '@') {
        out += s[i + 1];
        break;
      }
    }
  }
  return (out);
}

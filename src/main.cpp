/*********
  Руи Сантос (Rui Santos)
  Более подробно о проекте на: http://randomnerdtutorials.com
  Пример в IDE Arduino: File > Examples > Arduino OTA > BasicOTA.ino
                       (Файл > Примеры > Arduino OTA > BasicOTA.ino)
*********/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <BlynkSimpleEsp8266.h>

#include <wifisettings.h>

#define LED 2 //GPIO 2 = D4
#define HOT_INPUT 5 //GPIO 5 = D1
#define COLD_INPUT 4 //GPIO 4 = D2
//#define SETT_INPUT 4 //GPIO 4 = D2

#define ESP_CONNECT_TIMEOUT 15000UL // Время подключения к точке доступа, ms
#define SERVER_TIMEOUT 12000UL // Время ответа сервера, ms
#define LITRES_PER_IMPULS_DEFAULT 10  // 10 литров на импульс

int i;
const char* host = "esp8266-webupdate";
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

WidgetLCD lcd(V1);

float read_state(int PinNumber)
{
  float state;
  pinMode(PinNumber, OUTPUT);
  digitalWrite(PinNumber, HIGH);
  state = digitalRead(PinNumber);
  digitalWrite(PinNumber, LOW);
  return state;
}

void setup() {
  i = 0;
  Serial.begin(115200);
  Serial.println("Booting");  //  "Загрузка"
  // WIFI_OFF = 0,
  // WIFI_STA = 1 - клиент,
  // WIFI_AP = 2 - точка доступа,
  // WIFI_AP_STA = 3 - клиент и точка доступа
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    //  "Соединиться не удалось! Перезагрузка..."
    delay(5000);
    ESP.restart();
  }

  // строчка для номера порта по умолчанию
  // можно вписать «8266»:
  // ArduinoOTA.setPort(8266);

  // строчка для названия хоста по умолчанию;
  // можно вписать «esp8266-[ID чипа]»:
  // ArduinoOTA.setHostname("myesp8266");

  // строчка для аутентификации
  // (по умолчанию никакой аутентификации не будет):
  // ArduinoOTA.setPassword((const char *)"123");

  /*
  ArduinoOTA.onStart([]() {
    Serial.println("Start");  //  "Начало OTA-апдейта"
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");  //  "Завершение OTA-апдейта"
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    //  "Ошибка при аутентификации"
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    //  "Ошибка при начале OTA-апдейта"
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    //  "Ошибка при подключении"
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    //  "Ошибка при получении данных"
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    //  "Ошибка при завершении OTA-апдейта"
  });
  ArduinoOTA.begin();
  Serial.println("Ready");  //  "Готово"
  Serial.print("IP address: ");  //  "IP-адрес: "
  Serial.println(WiFi.localIP());
  */

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);

  Serial.print("IP address: ");  //  "IP-адрес: "
  Serial.println(WiFi.localIP());

  pinMode(LED, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
//  pinMode(COLD_INPUT, INPUT);
//  pinMode(HOT_INPUT, INPUT);
//  pinMode(SETT_INPUT, INPUT);
  Serial.println("Setup");
  Serial.println(read_state(COLD_INPUT));
  Serial.println(read_state(HOT_INPUT));
}

void loop() {
  i = i + 1;
  //ArduinoOTA.handle();
  httpServer.handleClient();
  MDNS.update();
/*
  digitalWrite(LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
  Serial.println("LOW");
  delay(1000);                      // Wait for a second
  digitalWrite(LED, HIGH);  // Turn the LED off by making the voltage HIGH
  Serial.println("HIGH");
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
  */
  Serial.println("Loop");
  Serial.println(read_state(COLD_INPUT));
  Serial.println(read_state(HOT_INPUT));
  Serial.println("------------------------");
  //Serial.println(read_state(SETT_INPUT));

  Blynk.config(auth);
  if (Blynk.connect(SERVER_TIMEOUT))
  {
    Serial.println("Blynk.connect");

    //lcd.clear(); //Use it to clear the LCD Widget
    lcd.print(0, 0, i); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(0, 1, i);
    Blynk.run();


    //Blynk.disconnect();
    Serial.println(i);

  } else {
      Serial.println("Blynk.connect error");
  }

  //ESP.deepSleep(5e06);
  delay(5000);
}

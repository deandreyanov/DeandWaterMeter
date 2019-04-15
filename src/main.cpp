// Описание Blynk
// https://the-robot.ru/study/nodemcu-blynk-uroki-1-ustrojsva-vyvoda/
/* Для работы с LCD Blynk надо настроить в приложении Blynk
LCD widget, switch to ADVANCED mode, select pin V1
Создать WidgetLCD lcd(V1)
Для работы в режиме сохранения энергии надо соединить D0 с RST и вызывать
ESP.deepSleep(5e06)

TODO
Для измерения заряда аккумулятора надо на + посадить одну из ног с считывать значение
*/
/*********
Руи Сантос (Rui Santos)
Более подробно о проекте на: http://randomnerdtutorials.com
Пример в IDE Arduino: File > Examples > Arduino OTA > BasicOTA.ino
(Файл > Примеры > Arduino OTA > BasicOTA.ino)
*********/
#include <settings.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <BlynkSimpleEsp8266.h>

#include <wifisettings.h>

#define LED 2 //GPIO 2 = D4
#define COLD_INPUT 4 //GPIO 4 = D2
#define HOT_INPUT 5 //GPIO 5 = D1
//#define SETT_INPUT 4 //GPIO 4 = D2

int OnePeriod; // меняется каждый период цикла
int LED_IS_ON; // 1 - светодиод включен

// предыдущее состояние импульсного выхода
int prev_cold;
int prev_hot;

// текущее состояние импульсного выхода
int curr_cold;
int curr_hot;

// сохранённые показания счётчиков
float save_cold;
float save_hot;

const char* host = "esp8266-webupdate";
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

WidgetLCD lcd1(V1);
WidgetLCD lcd2(V2);

strSettings mySett;
//-------------------------------------------
//
//-------------------------------------------
template<typename T>
void print_2_serial(const T& mystr) {
  Serial.print(millis());
  Serial.print(" ");
  Serial.println(mystr);
}

//-------------------------------------------
//
//-------------------------------------------
template<typename T>
void print_on_LCD(int numLCD, int X, int Y, bool bClear, const T& mystr) {
  Blynk.run();
  if (bClear == true) {
    if (numLCD  == 1) {
      lcd1.clear();
    } else if (numLCD == 2) {
      lcd2.clear();
    }
  }
  if (numLCD  == 1) {
    lcd1.print(X, Y, mystr);
  } else if (numLCD == 2) {
    lcd2.print(X, Y, mystr);
  }
}

//-------------------------------------------
// Отправка показаний на Blynk
//-------------------------------------------
void send_2_blynk(int numLCD) {
  Blynk.run();
  if (numLCD  == 1) {
    lcd1.clear();
    lcd1.print(0, 0, "C:"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd1.print(0, 1, "H:");

    lcd1.print(2, 0, mySett.save_cold); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd1.print(2, 1, mySett.save_hot);
  } else if (numLCD == 2) {
    lcd2.clear();
    lcd2.print(0, 0, "C:"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd2.print(0, 1, "H:");

    lcd2.print(2, 0, mySett.save_cold); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd2.print(2, 1, mySett.save_hot);
  }
}

//-------------------------------------------
//
//-------------------------------------------
void ledON() {
  LED_IS_ON = 1;
  digitalWrite(LED, LOW);
  print_2_serial("ledON");
}

//-------------------------------------------
//
//-------------------------------------------
void ledOFF() {
  digitalWrite(LED, HIGH);
  print_2_serial("ledOFF");
}

//-------------------------------------------
//
//-------------------------------------------
int read_state(int PinNumber) {
  float state;
  pinMode(PinNumber, OUTPUT);
  digitalWrite(PinNumber, HIGH);
  state = digitalRead(PinNumber);
  digitalWrite(PinNumber, LOW);
  return state;
}

//-------------------------------------------
//
//-------------------------------------------
void setup() {
  OnePeriod = 0;
  if (loadSettings(mySett)) {

  } else {

  }

  save_cold = 0;
  save_hot = 0;

  prev_cold = 0;
  prev_hot = 0;

  Serial.begin(115200);
  print_2_serial("Booting");  //  "Загрузка"
  // WIFI_OFF = 0,
  // WIFI_STA = 1 - клиент,
  // WIFI_AP = 2 - точка доступа,
  // WIFI_AP_STA = 3 - клиент и точка доступа
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    print_2_serial("Connection Failed! Rebooting...");
    //  "Соединиться не удалось! Перезагрузка..."
    delay(5000);
    ESP.restart();
  }

  Blynk.config(auth);

  MDNS.begin(host);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
  //Open http://esp8266-webupdate.local/update in your browser
  //IP address: 192.168.31.214

  print_2_serial("IP address: ");  //  "IP-адрес: "
  print_2_serial(WiFi.localIP());

  pinMode(LED, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Blynk.run();
  send_2_blynk(NUM_LCD);
}

//-------------------------------------------
//
//-------------------------------------------
void loop() {
  print_2_serial("Begin.loop");
  httpServer.handleClient();
  MDNS.update();

  curr_cold = read_state(COLD_INPUT);
  curr_hot = read_state(HOT_INPUT);

  if (prev_cold != curr_cold) {
    if (curr_cold == 0) {
      ledON();
      mySett.save_cold += LITRES_PER_IMPULS_DEFAULT;
      saveSettings(mySett);
      //send_2_blynk(NUM_LCD);
    } else {
      ledOFF();
    }
    prev_cold = curr_cold;
  }

  if (prev_hot != curr_hot) {
    if (curr_hot == 0) {
      ledON();
      mySett.save_hot += LITRES_PER_IMPULS_DEFAULT;
      saveSettings(mySett);
      //send_2_blynk(NUM_LCD);
    } else {
      ledOFF();
    }
    prev_hot = curr_hot;
  }

  if (OnePeriod == 0) {
    OnePeriod = 1;
    send_2_blynk(NUM_LCD);
    print_on_LCD(NUM_LCD, 15, 1, false, "*");

    if (LED_IS_ON == 1) {
      LED_IS_ON = 0;
    } else {
      ledOFF();
    }

  } else {
    OnePeriod = 0;
    print_on_LCD(NUM_LCD, 15, 1, false, "0");

    if (LED_IS_ON == 1) {
      LED_IS_ON = 0;
    } else {
      ledOFF();
    }
    
  }
  //ESP.deepSleep(5e06);
  delay(1000);
  print_2_serial("End.loop");
}

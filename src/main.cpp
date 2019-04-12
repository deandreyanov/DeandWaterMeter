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
#define HOT_INPUT 5 //GPIO 5 = D1
#define COLD_INPUT 4 //GPIO 4 = D2
//#define SETT_INPUT 4 //GPIO 4 = D2

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

WidgetLCD lcd(V1);

strSettings mySett;

void send_2_blynk();

int read_state(int PinNumber) {
  float state;
  pinMode(PinNumber, OUTPUT);
  digitalWrite(PinNumber, HIGH);
  state = digitalRead(PinNumber);
  digitalWrite(PinNumber, LOW);
  return state;
}

void setup() {
  if (loadSettings(mySett)) {

  } else {

  }

  save_cold = 0;
  save_hot = 0;

  prev_cold = 0;
  prev_hot = 0;

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

  Blynk.config(auth);


  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
  //Open http://esp8266-webupdate.local/update in your browser
  //IP address: 192.168.31.214

  Serial.print("IP address: ");  //  "IP-адрес: "
  Serial.println(WiFi.localIP());

  pinMode(LED, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  send_2_blynk();
}
// Отправка показаний на Blynk
void send_2_blynk() {
  Blynk.run();
  if (Blynk.connect(SERVER_TIMEOUT)) {
    lcd.clear();
    lcd.print(0, 0, "C:"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(0, 1, "H:");

    lcd.print(2, 0, mySett.save_cold); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(2, 1, mySett.save_hot);
  } else {
      Serial.println("Blynk.connect error");
  }
}

void loop() {
  httpServer.handleClient();
  MDNS.update();

  curr_cold = read_state(COLD_INPUT);
  curr_hot = read_state(HOT_INPUT);

  if (prev_cold != curr_cold) {
    if (curr_cold == 0) {
      digitalWrite(LED, HIGH);
      mySett.save_cold += LITRES_PER_IMPULS_DEFAULT;
      saveSettings(mySett);
      send_2_blynk();
    }else{
      digitalWrite(LED, LOW);
    }
    prev_cold = curr_cold;
  }

  if (prev_hot != curr_hot) {
    if (curr_hot == 0) {
      mySett.save_hot += LITRES_PER_IMPULS_DEFAULT;
      saveSettings(mySett);
      send_2_blynk();
    }
    prev_hot = curr_hot;
  }


  //ESP.deepSleep(5e06);
  delay(1000);
}

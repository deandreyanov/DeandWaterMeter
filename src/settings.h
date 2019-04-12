#ifndef _SETTINGS_h
#define _SETTINGS_h

/*
    Уровень логирования
*/
#define LOGLEVEL 6

#define ESP_CONNECT_TIMEOUT 15000UL // Время подключения к точке доступа, ms
#define SERVER_TIMEOUT 12000UL // Время ответа сервера, ms
#define LITRES_PER_IMPULS_DEFAULT 0.01  // 10 литров на импульс
#define MY_CRC 2517 // если контрольная суммма сошлась - значит настройки сохранялись

/*
Настройки хранящиеся EEPROM
*/
struct strSettings {
    /*
    Показания счетчиках в кубометрах,
    введенные пользователем при настройке
    */
    float save_cold;
    float save_hot;
    int myCRC;
};

/*
Сохраняем конфигурацию в EEPROM
*/
void saveSettings(strSettings &sett);

/*
Читаем конфигурацию из EEPROM
*/
bool loadSettings(strSettings &sett);

#endif

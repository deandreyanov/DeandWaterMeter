#include <Logging.h>

#define FIRMWARE_VERSION "0.7"
/*
    Уровень логирования
*/
#define LOGLEVEL 6

#define ESP_CONNECT_TIMEOUT 15000UL // Время подключения к точке доступа, ms
#define SERVER_TIMEOUT 12000UL // Время ответа сервера, ms
#define LITRES_PER_IMPULS_DEFAULT 10  // 10 литров на импульс

#define BLYNK_KEY_LEN 34

#define FAKE_CRC 0422

#define CURRENT_VERSION 1

/*
Настройки хранящиеся EEPROM
*/
struct Settings
{
    uint8_t  version;      //Версия конфигурации

    uint8_t  reserved;

    //SEND_BLYNK

    //уникальный ключ устройства blynk
    char     blynk_key[BLYNK_KEY_LEN];

    /*
    Показания счетчиках в кубометрах,
    введенные пользователем при настройке
    */
    float    channel0_start;
    float    channel1_start;

    /*
    Кол-во литров на 1 импульс
    */
    uint16_t liters_per_impuls;

    /*
    Кол-во импульсов Attiny85 соответствующие показаниям счетчиков,
    введенных пользователем при настройке
    */
    uint32_t impulses0_start;
    uint32_t impulses1_start;

    /*
    Не понятно, как получить от Blynk прирост показаний,
    поэтому сохраним их в памяти каждое включение
    */
    uint32_t impulses0_previous;
    uint32_t impulses1_previous;

    /*
    Зарезервируем кучу места, чтобы не писать конвертер конфигураций.
    Будет актуально для On-the-Air обновлений
    */
    uint8_t  reserved2[256];

    /*
    Контрольная сумма, чтобы гарантировать корректность чтения настроек
    */
    uint16_t crc;
};

/*
Сохраняем конфигурацию в EEPROM
*/
void saveConfig(const Settings &sett);

/*
Читаем конфигурацию из EEPROM
*/
bool loadConfig(Settings &sett);

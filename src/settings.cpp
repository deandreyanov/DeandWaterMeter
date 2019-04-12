#include <settings.h>
#include <EEPROM.h>
/* Сохраняем конфигурацию в EEPROM */
void saveConfig(const Settings &sett)
{
    EEPROM.begin(sizeof(sett));
    EEPROM.put(0, sett);

    if (!EEPROM.commit())
    {
        LOG_ERROR("CFG", "Config stored FAILED");
    }
    else
    {
        LOG_NOTICE("CFG", "Config stored OK");
    }
}


/* Загружаем конфигурацию в EEPROM. true - успех. */
bool loadConfig(struct Settings &sett)
{
    EEPROM.begin(sizeof(sett));  //  4 до 4096 байт. с адреса 0x7b000.
    EEPROM.get(0, sett);

    if (sett.crc == FAKE_CRC)  // todo: сделать нормальный crc16
    {
        LOG_NOTICE("CFG", "CRC ok");

        // Для безопасной работы с буферами,  в библиотеках может не быть проверок

        sett.blynk_key[BLYNK_KEY_LEN-1] = '\0';
//        sett.blynk_host[BLYNK_HOST_LEN-1] = '\0';
//        sett.blynk_email[EMAIL_LEN-1] = '\0';
//        sett.blynk_email_title[BLYNK_EMAIL_TITLE_LEN-1] = '\0';
//        sett.blynk_email_template[BLYNK_EMAIL_TEMPLATE_LEN-1] = '\0';

        LOG_NOTICE("CFG", "WATERIUS.RU");
//        LOG_NOTICE("CFG", "email=" << sett.waterius_email);
//        LOG_NOTICE("CFG", "host=" << sett.waterius_host << " key=" << sett.waterius_key);

        LOG_NOTICE("CFG", "BLYNK.CC");
//        LOG_NOTICE("CFG", "host=" << sett.blynk_host << " key=" << sett.blynk_key);
//        LOG_NOTICE("CFG", "email=" << sett.blynk_email);

        // Всегда одно и тоже будет
        LOG_NOTICE("CFG", "COUNTERS");
        LOG_NOTICE("CFG", "channel0_start=" << sett.channel0_start << ", impulses0_start=" << sett.impulses0_start << ", factor=" << sett.liters_per_impuls );
        LOG_NOTICE("CFG", "channel1_start=" << sett.channel1_start << ", impulses1_start=" << sett.impulses1_start);

        return true;

    } else {
        // Конфигурация не была сохранена в EEPROM, инициализируем с нуля
        LOG_NOTICE("CFG", "crc failed=" << sett.crc );

        // Заполняем нулями всю конфигурацию
        memset(&sett, 0, sizeof(sett));

        sett.version = CURRENT_VERSION;  //для совместимости в будущем
        LOG_NOTICE("CFG", "version=" << sett.version);

//        strncpy0(sett.waterius_host, WATERIUS_DEFAULT_DOMAIN, WATERIUS_HOST_LEN);

//        strncpy0(sett.blynk_host, BLYNK_DEFAULT_DOMAIN, BLYNK_HOST_LEN);

//        String email_title = "Новые показания {DEVICE_NAME}";
//        strncpy0(sett.blynk_email_title, email_title.c_str(), BLYNK_EMAIL_TITLE_LEN);

//        String email_template = "Горячая: {V0}м3, Холодная: {V1}м3<br>За день:<br>Горячая: +{V3}л, Холодная: +{V4}л<br>Напряжение:{V2}В";
//        strncpy0(sett.blynk_email_template, email_template.c_str(), BLYNK_EMAIL_TEMPLATE_LEN);

        sett.liters_per_impuls = LITRES_PER_IMPULS_DEFAULT;

//Можно задать константы при компиляции, чтобы Вотериус сразу заработал

#ifdef BLYNK_KEY
        #pragma message(VAR_NAME_VALUE(BLYNK_KEY))
        String key = VALUE(BLYNK_KEY);
        strncpy0(sett.blynk_key, key.c_str(), BLYNK_KEY_LEN);
        LOG_NOTICE("CFG", "default Blynk key=" << key);
#endif

#if defined(SSID_NAME)
#if defined(SSID_PASS)
        #pragma message(VAR_NAME_VALUE(SSID_NAME))
        #pragma message(VAR_NAME_VALUE(SSID_PASS))
        WiFi.begin(VALUE(SSID_NAME), VALUE(SSID_PASS), 0, NULL, false);  //connect=false, т.к. мы следом вызываем Wifi.begin
        LOG_NOTICE("CFG", "default ssid=" << VALUE(SSID_NAME) << ", pwd=" << VALUE(SSID_PASS));

        sett.crc = FAKE_CRC; //чтобы больше не попадать сюда
        return true;
#endif
#endif
        return false;
    }
}

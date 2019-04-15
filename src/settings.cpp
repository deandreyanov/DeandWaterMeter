#include <EEPROM.h>
#include <settings.h>
/* Сохраняем конфигурацию в EEPROM */
void saveSettings(strSettings &sett) {
    EEPROM.begin(sizeof(sett));
    sett.myCRC = MY_CRC;
    EEPROM.put(0, sett);

    if (!EEPROM.commit()) {
        //LOG_ERROR("CFG", "Config stored FAILED");
    }
    else {
        //LOG_NOTICE("CFG", "Config stored OK");
    }
}

#define SET_FIRST_SETTINGS 0 // Если 1 - первое включение и надо задать начальные значения
#define FIRST_COLD 72.951
#define FIRST_HOT 0.001

/* Загружаем конфигурацию в EEPROM. true - успех. */
bool loadSettings(struct strSettings &sett) {
    EEPROM.begin(sizeof(sett));  //  4 до 4096 байт. с адреса 0x7b000.
    EEPROM.get(0, sett);

    if ((sett.myCRC == MY_CRC) && (SET_FIRST_SETTINGS == 0)) {
      //LOG_NOTICE("CFG", "CRC ok");
      return true;
    } else {
        // Конфигурация не была сохранена в EEPROM, инициализируем с нуля
        //LOG_NOTICE("CFG", "crc failed=" << sett.myCRC );

        // Заполняем нулями всю конфигурацию
        memset(&sett, 0, sizeof(sett));
        if (SET_FIRST_SETTINGS == 1) {
          sett.save_cold = FIRST_COLD;
          sett.save_hot = FIRST_HOT;
          saveSettings(sett);
        }
        return false;
    }
}

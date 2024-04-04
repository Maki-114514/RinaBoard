#include "EEPROMSAVE.h"

//璃奈板亮度和颜色
const uint8_t defaultBoardBrightness = 100;
const uint32_t defaultColor = 0x00FF1493;

//灯条亮度和开关
const uint8_t defaultLightBrightness = 100;
const uint8_t defaultLightIsOn = 1;

//WIFI通讯相关
const String defaultSSID = "RinaBoard";
const String defaultPassword = "12345678";

const String defaultDeviceName = "璃奈板";

void EEPROMInit()
{
    EEPROM.begin(4096); //申请操作到地址4095（比如你只需要读写地址为100上的一个字节，该处也需输入参数101）
}

//———————————————————————————————————————————————————————— Read —————————————————————————————————————————————————————————————————
uint32_t readColor()
{
    if (EEPROM.read(COLOR_ADDR) != 0)
    {
        return EEPROM.readUInt(COLOR_ADDR + 1);
    } else
    {
        return defaultColor;
    }
}

uint8_t readBoardBrightness()
{
    if (EEPROM.read(BOARDBRIGHTNESS_ADDR) != 0)
    {
        return EEPROM.read(BOARDBRIGHTNESS_ADDR + 1);
    } else
    {
        return defaultLightBrightness;
    }
}

uint8_t readLightstate()
{
    if (EEPROM.read(LIGHTSTATE_ADDR) != 0)
    {
        return EEPROM.read(LIGHTSTATE_ADDR + 1);
    } else
    {
        return defaultLightIsOn;
    }
}

uint8_t readLightBrightness()
{
    if (EEPROM.read(LIGHTBRIGHTNESS_ADDR) != 0)
    {
        return EEPROM.read(LIGHTBRIGHTNESS_ADDR + 1);
    } else
    {
        return defaultLightBrightness;
    }
}

String readDeviceName()
{
    if (EEPROM.read(DEVICENAME_ADDR) != 0)
    {
        return EEPROM.readString(DEVICENAME_ADDR + 1);
    } else
    {
        return defaultDeviceName;
    }
}

String readWifiSSID()
{
    if (EEPROM.read(WIFISSID_ADDR) != 0)
    {
        return EEPROM.readString(WIFISSID_ADDR + 1);
    } else
    {
        return defaultSSID;
    }
}

String readWifiPassword()
{
    if (EEPROM.read(WIFIPASSWORD_ADDR) != 0)
    {
        return EEPROM.readString(WIFIPASSWORD_ADDR + 1);
    } else
    {
        return defaultPassword;
    }
}

//———————————————————————————————————————————————————————— Write —————————————————————————————————————————————————————————————————
void writeColor(uint32_t color)
{
    if (EEPROM.read(COLOR_ADDR) == 0)//如果第一位是0，说明color值未设置
    {
        EEPROM.write(COLOR_ADDR, 1);
    }
    EEPROM.writeUInt(COLOR_ADDR + 1, color);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeBoardBrightness(uint8_t brightness)
{
    if (EEPROM.read(BOARDBRIGHTNESS_ADDR) == 0)
    {
        EEPROM.write(BOARDBRIGHTNESS_ADDR, 1);
    }
    EEPROM.write(BOARDBRIGHTNESS_ADDR + 1, brightness);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeLightState(uint8_t lightIsOn)
{
    if (EEPROM.read(LIGHTSTATE_ADDR) == 0)
    {
        EEPROM.write(LIGHTSTATE_ADDR, 1);
    }
    EEPROM.write(LIGHTSTATE_ADDR + 1, lightIsOn);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeLightBrightness(uint8_t brightness)
{
    if (EEPROM.read(LIGHTBRIGHTNESS_ADDR) == 0)
    {
        EEPROM.write(LIGHTBRIGHTNESS_ADDR, 1);
    }
    EEPROM.write(LIGHTBRIGHTNESS_ADDR + 1, brightness);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeDeviceName(String *deviceName)
{
    if(EEPROM.read(DEVICENAME_ADDR) == 0)
    {
        EEPROM.write(DEVICENAME_ADDR, 1);
    }
    EEPROM.writeString(DEVICENAME_ADDR + 1, *deviceName);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeWifiSSID(String *ssid)
{
    if(EEPROM.read(WIFISSID_ADDR) == 0)
    {
        EEPROM.write(WIFISSID_ADDR, 1);
    }
    EEPROM.writeString(WIFISSID_ADDR + 1, *ssid);
    EEPROM.commit();
    vTaskDelay(1);
}

void writeWifiPassword(String *password)
{
    if(EEPROM.read(WIFIPASSWORD_ADDR) == 0)
    {
        EEPROM.write(WIFIPASSWORD_ADDR, 1);
    }
    EEPROM.writeString(WIFIPASSWORD_ADDR + 1, *password);
    EEPROM.commit();
    vTaskDelay(1);
}
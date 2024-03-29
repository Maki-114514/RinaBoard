#include <Arduino.h>

//LED驱动库
#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>

//SD卡
#include <SD.h>
#include <SPI.h>

//UDP通讯和通讯格式解析库
#include <WiFi.h>
#include "cmd&TypeDef.h"

//璃奈板驱动宏定义
#define BOARD_PIN 4
#define MATRIX_WIDTH 18
#define MATRIX_HEIGHT 16
#define NUM_MATRIX (MATRIX_WIDTH * MATRIX_HEIGHT)

//灯带驱动宏定义
#define LED_PIN 16
#define LEDC_CHANNEL 0 //定时器通道
#define LEDC_TIMER_BIT 8 //定时器位宽
#define LEDC_TIMER_FREQ 30000 //定时器频率
#define LEDC_MAXDUTY 255

//超时定义
#define TIMEOUT 1800

//文件读取相关
SPIClass spi = SPIClass(HSPI);
File myFile;

//显示驱动相关
uint8_t bitmap[48];
CRGB leds[NUM_MATRIX];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT,
                                                  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                                                  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);
//璃奈板亮度和颜色
uint8_t boardBrightness = 100;
uint32_t color = 0x00FF1493;

//灯条亮度和开关
uint8_t lightBrightness = 100;
uint8_t lightIsOn = 1;

//电量
float electricity = 0.0f;

//WIFI通讯相关
String ssid = "RinaBoard";
String password = "12345678";

WiFiUDP udp;
WiFiUDP udp2;
IPAddress LocalIP(192, 168, 4, 22);
IPAddress Gateway(192, 168, 4, 22);
IPAddress SubNet(255, 255, 255, 0);
const uint16_t LocalPort = 11451;
const uint16_t LocalPort2 = 14514;

IPAddress RemoteIP;
uint16_t RemotePort;

IPAddress RemoteIP2;
uint16_t RemotePort2;

uint8_t dataCmd;
uint8_t dataLen;
uint8_t dataBuf[100];
uint8_t bufSize = sizeof(dataBuf) / sizeof(dataBuf[0]);

//任务调用相关
TaskHandle_t UdpInteract_taskHandle;
TaskHandle_t OkToConnect_taskHandle;

//设备相关
String deviceName = "璃奈板";
const String deviceType = "V1";

//状态模式相关
SystemState mode = ExpressionMode;

//连接状态相关
bool isConnected = false;
//——————————————————————————————————————————————————————————————————————— 灯阵驱动相关 ———————————————————————————————————————————————————————————————————————————————————————————————
// 灯珠地址构造矩阵
const uint16_t led_address[MATRIX_HEIGHT][MATRIX_WIDTH]
        PROGMEM = {
                {271, 271, 38,  39, 70, 71, 102, 103, 134, 135, 166, 167, 198, 199, 230, 231, 271, 271},
                {271, 10,  37,  40, 69, 72, 101, 104, 133, 136, 165, 168, 197, 200, 229, 232, 259, 271},
                {9,   11,  36,  41, 68, 73, 100, 105, 132, 137, 164, 169, 196, 201, 228, 233, 258, 260},
                {8,   12,  35,  42, 67, 74, 99,  106, 131, 138, 163, 170, 195, 202, 227, 234, 257, 261},
                {7,   13,  34,  43, 66, 75, 98,  107, 130, 139, 162, 171, 194, 203, 226, 235, 256, 262},
                {6,   14,  33,  44, 65, 76, 97,  108, 129, 140, 161, 172, 193, 204, 225, 236, 255, 263},
                {5,   15,  32,  45, 64, 77, 96,  109, 128, 141, 160, 173, 192, 205, 224, 237, 254, 264},
                {4,   16,  31,  46, 63, 78, 95,  110, 127, 142, 159, 174, 191, 206, 223, 238, 253, 265},
                {3,   17,  30,  47, 62, 79, 94,  111, 126, 143, 158, 175, 190, 207, 222, 239, 252, 266},
                {2,   18,  29,  48, 61, 80, 93,  112, 125, 144, 157, 176, 189, 208, 221, 240, 251, 267},
                {1,   19,  28,  49, 60, 81, 92,  113, 124, 145, 156, 177, 188, 209, 220, 241, 250, 268},
                {0,   20,  27,  50, 59, 82, 91,  114, 123, 146, 155, 178, 187, 210, 219, 242, 249, 269},
                {271, 21,  26,  51, 58, 83, 90,  115, 122, 147, 154, 179, 186, 211, 218, 243, 248, 271},
                {271, 22,  25,  52, 57, 84, 89,  116, 121, 148, 153, 180, 185, 212, 217, 244, 247, 271},
                {271, 23,  24,  53, 56, 85, 88,  117, 120, 149, 152, 181, 184, 213, 216, 245, 246, 271},
                {271, 271, 271, 54, 55, 86, 87,  118, 119, 150, 151, 182, 183, 214, 215, 271, 271, 271}};

/**
 * @brief   用于自定义地址映射给LED矩阵，其中输入参数x和y都是以0开始，正好对应二位数组的index从0开始
 * @return  返回led地址
 * */
uint16_t myRemapFn(uint16_t x, uint16_t y)
{
    if (x > MATRIX_WIDTH | y > MATRIX_HEIGHT)
    {
        return 271;
    } else
    {
        return led_address[y][x];
    }
}

/**
 * @brief 更新表情显示
 * */
void showImagin()
{
    matrix->clear();
    matrix->drawBitmap(0, 0, bitmap, 18, 16, FastLED_NeoMatrix::Color24to16(color));
    matrix->show();
}
//——————————————————————————————————————————————————————————————————————— 灯阵驱动相关结束 ———————————————————————————————————————————————————————————————————————————————————————————————

//——————————————————————————————————————————————————————————————————————— 数据发送&回应 ———————————————————————————————————————————————————————————————————————————————————————————————
/**
 * @brief 发送数据包
 * @param data:数据包
 * @param len:数据包长度
 * */
void SendPackage(const uint8_t *data, uint8_t len)
{
    udp.beginPacket(RemoteIP, RemotePort); //准备发送数据
    udp.write(data, len); // 发送uint8_t数组
    udp.endPacket();            //发送数据
}

void SendString(const String *str, uint8_t len)
{
    udp.beginPacket(RemoteIP, RemotePort); //准备发送数据
    udp.println(*str);
    udp.endPacket();            //发送数据
}

//——————————————————————————————————————————————————————————————————————— 数据发送&回应结束 ———————————————————————————————————————————————————————————————————————————————————————————————


//——————————————————————————————————————————————————————————————————————— UDP通讯收发 ———————————————————————————————————————————————————————————————————————————————————————————————
/**
 * @brief 接收命令并处理变量值
 * @param cmd:指令
 * @param data:数据包
 * @param len:数据包长度
 * */
void SetValue(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    switch (cmd & 0b01111111)
    {
        case REBOOT:
            ESP.restart();
        case COLOR:
            color = (data[1] << 16) | (data[2] << 8) | data[3];
//            Serial.printf("%hX, %hX, %hX, %hX\r\n", data[0], data[1], data[2], data[3]);
            Serial.printf("Color is changed to %d\n", color);
            showImagin();
            break;
        case BOARDBRIGHTNESS:
            boardBrightness = data[0];
            Serial.printf("Board brightness is changed to %d\r\n", boardBrightness);
            matrix->setBrightness(boardBrightness);
            showImagin();
            break;
        case BOARDBRIGHTNESSOVER:
            //当亮度停止改变，则将当前亮度存储在eeprom里
            Serial.printf("Board brightness is saved to eeprom\r\n");



            break;
        case BITMAP:
            for (int i = 0; i < 48; i++)
            {
                bitmap[i] = data[i];
            }
            Serial.println("Bitmap is changed");
            showImagin();
            break;
        case SAVEBITMAP: //保存bitmap时会告知当前表情名称
            char *bitmapName;
            for (int i = 0; i < len; ++i)
            {
                bitmapName[i] = (char) (data[i]);
            }
            break;
        case LIGHTSTATE:
            lightIsOn = data[0];
            if(lightIsOn){
                ledcWrite(LEDC_CHANNEL, lightBrightness);
            } else{
                ledcWrite(LEDC_CHANNEL, 0);
            }
            break;
        case LIGHTBRIGHTNESS:
            lightBrightness = data[0];
            if(lightIsOn) ledcWrite(LEDC_CHANNEL, lightBrightness);
            break;
        case LIGHTBRIGHTNESSOVER:
            //当亮度停止改变时，将亮度存储在eeprom里
            Serial.printf("Light brightness is saved to eeprom\r\n");



            break;
        case DEVICENAME:
            deviceName = "";
            for (int i = 0; i < len; ++i)
            {
                deviceName[i] = (char) data[i];
            }
            Serial.println("Device name is saved as " + deviceName);
            break;
        case WIFISSID:
            ssid = "";
            for (int i = 0; i < len; ++i)
            {
                ssid[i] = (char) data[i];
            }
            Serial.println("Wifi ssid is saved as " + ssid);
            break;
        case WIFIPASSWORD:
            password = "";
            for (int i = 0; i < len; ++i)
            {
                password[i] = (char) data[i];
            }
            Serial.println("Wifi password is saved as " + password);
            break;
        case SYSTEMSTATE:
            switch (data[0])
            {
                case EXPRESSIONMODE:
                    mode = ExpressionMode;
                    Serial.println("System state is changed to ExpressionMode");
                    break;
                case VIDEOMODE:
                    mode = VideoMode;
                    Serial.println("System state is changed to VedioMode");
                    break;
                case RECOGNITIONMODE:
                    mode = RecognitionMode;
                    Serial.println("System state is changed to RecognitionMode");
                    break;
            }
            break;
    }
}

/**
 * @brief 获取数据请求，发送数据返回给安卓app
 * @param cmd: 命令
 * @param data: 数据包
 * @param len: 数据长度
 * */
void GetValue(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    switch (cmd & 0b01111111)
    {
        case COLOR:
            uint8_t colorData[4];
            colorData[0] = (color >> 24) | 0xFF;
            colorData[1] = (color >> 16) & 0xFF;//color是24位，高8位没有实际意义，不做处理，只发后24位
            colorData[2] = (color >> 8) & 0xFF;
            colorData[3] = color & 0xFF; // 低8位
            SendPackage(colorData, 4);
            break;
        case BOARDBRIGHTNESS:
            SendPackage(&boardBrightness, 1);
            break;
        case BITMAP:
            SendPackage(bitmap, 48);
            break;
        case EXPRESSIONLIST:
            break;
        case LIGHTSTATE:
            SendPackage(&lightIsOn, 1);
            break;
        case LIGHTBRIGHTNESS:
            SendPackage(&lightBrightness, 1);
            break;
        case ELECTRICITY:
            union FloatToArray converter;
            converter.floatValue = electricity;
            SendPackage(converter.uintValue, 4);
            break;
        case DEVICENAME:
            SendString(&deviceName, sizeof(deviceName));
            break;
        case DEVICETYPE:
            SendString(&deviceType, sizeof(deviceType));
            break;
        case WIFISSID:
            SendString(&ssid, sizeof(ssid));
            break;
        case SYSTEMSTATE:
            uint8_t package[1];
            switch (mode)
            {
                case ExpressionMode:
                    package[0] = EXPRESSIONMODE;
                    break;
                case RecognitionMode:
                    package[0] = RECOGNITIONMODE;
                    break;
            }
            SendPackage(package, 1);
    }
}

//——————————————————————————————————————————————————————————————————————— UDP通讯收发结束 ———————————————————————————————————————————————————————————————————————————————————————————————

//——————————————————————————————————————————————————————————————————————————— 任务进程 ————————————————————————————————————————————————————————————————————————————————
//控制进程
void Task_OKToConnect(void *pt)
{
    uint32_t lastTime = millis();//判断是否超时用于判断是否连接
    while (1)
    {
        if (udp2.parsePacket())
        {
            //获取远端ip和端口号用于回传数据
            RemoteIP2 = udp2.remoteIP();
            RemotePort2 = udp2.remotePort();

            if (udp2.read() == ASK)
            {
                //如果接收到ASK，则说明通讯顺利，更新lastTime
                lastTime = millis();
                if(!isConnected){
                    isConnected = true;
                    Serial.println("Connect");
                }

//                Serial.println("Receive the ask");
                udp2.beginPacket(RemoteIP2, RemotePort2); //准备发送数据
                udp2.print("Ok to Link");
                udp2.endPacket();            //发送数据
//                Serial.println("Respond the ask");
            }
        }

        if(millis() - lastTime > TIMEOUT){
            if(isConnected){//如果之前处于连接状态，此时未连接，则更新连接状态为未连接
                isConnected = false;
                Serial.println("Disconnect");
                //断开连接后如果原先是视频模式，需要切换回表情模式
                if(mode == VideoMode){
                    mode = ExpressionMode;
                    Serial.println("Mode is changed to ExpressionMode from VideoMode");
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}

//udp通讯进程
void Task_UdpInteract(void *pt)
{
    while (1)
    {
        if (udp.parsePacket())
        {
            //获取远端ip和端口号用于回传数据
            RemoteIP = udp.remoteIP();
            RemotePort = udp.remotePort();

            dataCmd = udp.read();
            dataLen = udp.read();

            //清空缓存区
            for (int i = 0; i < bufSize; i++){
                dataBuf[i] = 0;
            }
            udp.read(dataBuf, dataLen);
            /*for (int i = 0; i < dataLen; ++i)
            {
                Serial.printf("Receive data 0x%02X\r\n", dataBuf[i]);
            }*/
            //判断读还是写
            switch (dataCmd & 0b10000000)
            {
                case SETCMD:
                    SetValue(dataCmd, dataBuf, dataLen);
                    break;
                case GETCMD:
                    GetValue(dataCmd, dataBuf, dataLen);
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    vTaskDelete(NULL);
}
//———————————————————————————————————————————————————————————————————— 任务进程结束 ——————————————————————————————————————————————————————————————————

//——————————————————————————————————————————————————————————————————————— 初始化&去初始化 ———————————————————————————————————————————————————————————————————————————————————————————————
/**
 * @brief: 变量初始化，从eeprom中读取而后存储在变量中
 * */
void valueInit()
{

}

/**
 * @brief: WIFI UDP初始化
 * */
void wifiUDPInit()
{

    //wifi&udp通讯初始化
    WiFi.disconnect(true);

    WiFi.mode(WIFI_AP); // 设置为AP模式

    WiFi.softAPConfig(LocalIP, Gateway, SubNet);
    WiFi.softAP(ssid, password);    // 创建WiFi接入点
    IPAddress ip = WiFi.softAPIP(); // 获取AP的IP地址

    Serial.println();
    Serial.print("AP IP address: ");
    Serial.println(ip);

    //启动udp通讯协议
    udp.begin(LocalPort);
    udp2.begin(LocalPort2);
    //wifi&udp通讯初始化结束

    xTaskCreatePinnedToCore(Task_UdpInteract, "udp interact", 1024 * 20, NULL, 2, &UdpInteract_taskHandle, 0);
    xTaskCreatePinnedToCore(Task_OKToConnect, "Ok to connect", 1024 * 14, NULL, 1, &OkToConnect_taskHandle, 0);
}

/**
 * @brief: WIFI UDP关闭
 * */
void wifiUDPDeInit()
{
    //删除udp相关任务
    vTaskDelete(UdpInteract_taskHandle);
    vTaskDelete(OkToConnect_taskHandle);
    WiFi.disconnect(true);//断开wifi
    //停止udp
    udp.stop();
    udp2.stop();
}

/**
 * @brief: SD驱动初始化
 * */
void SDInit()
{
    // SD卡初始化
    spi.begin(12, 13, 11, 10);
    if (!SD.begin(10, spi))
    {
        Serial.println("存储卡挂载失败");
        return;
    }
    // SD卡初始化结束
}

/**
 * @brief: LED矩阵初始化
 * */
void LEDmatrixInit()
{
    // 灯板初始化
    FastLED.addLeds<NEOPIXEL, BOARD_PIN>(leds, NUM_MATRIX).setCorrection(TypicalLEDStrip);
    matrix->begin();
    matrix->setTextWrap(false);
    matrix->setBrightness(boardBrightness);
    matrix->setRemapFunction(myRemapFn);

    showImagin();
    // 灯板初始化结束
}

void LEDCInit(){
    // 建立LEDC通道
    ledcSetup(LEDC_CHANNEL, LEDC_TIMER_FREQ, LEDC_TIMER_BIT);
    ledcAttachPin(LED_PIN, LEDC_CHANNEL);//绑定LEDC引脚
    if(lightIsOn){
        ledcWrite(LEDC_CHANNEL, lightBrightness);
    }else{
        ledcWrite(LEDC_CHANNEL, 0);
    }
}
//——————————————————————————————————————————————————————————————————————— 初始化&去初始化结束 ———————————————————————————————————————————————————————————————————————————————————————————————


void setup()
{
    Serial.begin(115200);

    valueInit();
    LEDmatrixInit();
    wifiUDPInit();
    SDInit();
    LEDCInit();

}


void loop()
{

}
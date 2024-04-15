#ifndef CMD_TYPEDEF_H_
#define CMD_TYPEDEF_H_

//每次写入命令结束都需要回复一个RESPONSE告知安卓app已经收到数据

//写和读的命令
#define SETCMD 0X80
#define GETCMD 0x00

//获取回复
#define ASK 0x00
//重启
#define REBOOT 0x40

//数据定义
//璃奈板相关
#define COLOR 0x02
#define BOARDBRIGHTNESS 0x04
#define BOARDBRIGHTNESSOVER 0x05
#define BITMAP 0x06
#define EXPRESSIONLIST 0x08
#define SAVEBITMAP 0x0A //保存数据时会传一个字符串作为bitmap的名字
#define DELETEBITMAP 0x0B //删除数据时会传一个字符串作为bitmap的名字
#define CHANGEBITMAP 0x1B //app端发送一个字符串告诉璃奈板要切换到哪个表情
#define LASTEXPRESSION 0x1A
#define NEXTEXPRESSION 0x1C

//灯条相关
#define LIGHTSTATE 0x0C
#define LIGHTBRIGHTNESS 0x0E
#define LIGHTBRIGHTNESSOVER 0x0F

//电量
#define ELECTRICITY 0x10

//系统
#define DEVICENAME 0x12
#define DEVICETYPE 0x14
#define WIFISSID 0x16
#define WIFIPASSWORD 0x18

//状态模式
#define SYSTEMSTATE 0x20
#define EXPRESSIONMODE 0x00
#define VIDEOMODE 0x01
#define RECOGNITIONMODE 0x02

//启动动画
#define CLEARSTART 0x21
#define APPENDBITMAP 0x22
#define APPENDBITMAPONBOARD 0x23
#define APPENDMICROSECOND 0x24
#define APPENDSECOND 0x25

union FloatToArray {
    float floatValue;
    uint8_t uintValue[4];
};

typedef enum {
    ExpressionMode,
    VideoMode,
    RecognitionMode
}SystemState;

#endif
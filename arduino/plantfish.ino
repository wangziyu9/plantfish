/*
引脚定义
数字输入输出

D2 超声波 Trig， 发射超声波信号
D3 超声波 Echo， 接收超声波信号
D12 DHT 11 温湿度传感器

模拟输入输出

A1 土壤湿度模拟值输入
A2 水位模拟值，已废弃
A3 光强 SCL
A4 光强 SDA

A0 连接空气质量传感器模拟量输出的IO口
D2 空气质量传感器中为内部Led供电的引脚 
*/

#include "TaskScheduler.h" //包含此头文件，才能使用调度器

// 温湿度传感器
#include "DHT.h"           //加载DHT11的库
#define DHTTYPE DHT11      // 定义传感器类似 DHT11
#define DHTPIN 12          //宏定义DHT数据接口，编译时DHTPIN会替换成2

DHT dht(DHTPIN, DHTTYPE);  //声明 dht 函数
const int pinD = 12;

// lux 测量光照强度
#include <Wire.h>
#define ADDRESS_BH1750FVI 0x23    //ADDR="L" for this module
#define ONE_TIME_H_RESOLUTION_MODE 0x20

byte highByte = 0;
byte lowByte = 0;
unsigned int sensorOut = 0;
unsigned int illuminance = 0;
// lux

// 超声波端口
#define Trig 2 // 引脚Tring 连接 IO D2
#define Echo 3 // 引脚Echo 连接 IO D3 
 
float cm;     //距离变量
float temp;   // 超声波间隔时间

int EH_Sensor_Pin = A1;  // select the input pin for the potentiometer
int WL_Sensor_Pin = A2;  // select the input pin for the potentiometer
int EH_Sensor_Value = 0; // variable to store the value coming from the sensor
int WL_Sensor_Value = 0; // variable to store the value coming from the sensor
const float R = 1000.0;


// Arduino连接夏普GP2Y1010AU0F空气质量传感器检测PM2.5
// 些程序也适用于用 Arduino IDE 开发 ESP8266

// 定义引脚
#define PIN_DATA_OUT A0  //连接空气质量传感器模拟量输出的IO口
#define PIN_LED_VCC 2    //空气质量传感器中为内部Led供电的引脚

// 定义时间
const int DELAY_BEFORE_SAMPLING = 280; //采样前等待时间
const int DELAY_AFTER_SAMPLING = 40;   //采样后等待时间
const int DELAY_LED_OFF = 9680;        //间隔时间

// 根据灰尘密度计算AQI00
// 环境空气质量指数（AQI）技术规定（试行）](http://kjs.mep.gov.cn/hjbhbz/bzwb/dqhjbh/jcgfffbz/201203/t20120302_224166.htm
double getAQI(double ugm3) {
    double aqiL = 0;
    double aqiH = 0;
    double bpL = 0;
    double bpH = 0;
    double aqi = 0;
    //根据pm2.5和aqi对应关系分别计算aqi
    if (ugm3 >= 0 && ugm3 <= 35) {
      aqiL = 0;
      aqiH = 50;
      bpL = 0;
      bpH = 35;
    } else if (ugm3 > 35 && ugm3 <= 75) {
      aqiL = 50;
      aqiH = 100;
      bpL = 35;
      bpH = 75;
    } else if (ugm3 > 75 && ugm3 <= 115) {
      aqiL = 100;
      aqiH = 150;
      bpL = 75;
      bpH = 115;
    } else if (ugm3 > 115 && ugm3 <= 150) {
      aqiL = 150;
      aqiH = 200;
      bpL = 115;
      bpH = 150;
    } else if (ugm3 > 150 && ugm3 <= 250) {
      aqiL = 200;
      aqiH = 300;
      bpL = 150;
      bpH = 250;
    } else if (ugm3 > 250 && ugm3 <= 350) {
      aqiL = 300;
      aqiH = 400;
      bpL = 250;
      bpH = 350;
    } else if (ugm3 > 350) {
      aqiL = 400;
      aqiH = 500;
      bpL = 350;
      bpH = 500;
    }
    //公式aqi = (aqiH - aqiL) / (bpH - bpL) * (desity - bpL) + aqiL;
    aqi = (aqiH - aqiL) / (bpH - bpL) * (ugm3 - bpL) + aqiL;
    return aqi;
}


// 根据aqi获取级别描述

String getGradeInfo(double aqi) {
    String gradeInfo;
    if (aqi >= 0 && aqi <= 50) {
      gradeInfo = String("Perfect");
    } else if (aqi > 50 && aqi <= 100) {
      gradeInfo = String("Good");
    } else if (aqi > 100 && aqi <= 150) {
      gradeInfo = String("Mild polluted");
    } else if (aqi > 150 && aqi <= 200) {
      gradeInfo = String("Medium polluted");
    } else if (aqi > 200 && aqi <= 300) {
      gradeInfo = String("Heavily polluted");
    } else if (aqi > 300 && aqi <= 500) {
      gradeInfo = String("Severely polluted");
    } else {
      gradeInfo = String("Broken roof!!!");
    }
    return gradeInfo;
}

// 读取输出电压
double getOutputV() {
    digitalWrite(PIN_LED_VCC, LOW);
    delayMicroseconds(DELAY_BEFORE_SAMPLING);
    double analogOutput = analogRead(PIN_DATA_OUT);
    delayMicroseconds(DELAY_AFTER_SAMPLING);
    digitalWrite(PIN_LED_VCC, HIGH);
    delayMicroseconds(DELAY_LED_OFF);
    //Arduino模拟量读取值的范围为0~1023,以下换算为0~5v
    double outputV = analogOutput / 1024 * 5;
    return outputV;
}

// 根据输出电压计算灰尘密度
double getDustDensity(double outputV) {
    //输出电压和灰尘密度换算公式: ug/m3 = (V - 0.9) / 5 * 1000
    double ugm3 = (outputV - 0.9) / 5 * 1000;
    //去除检测不到的范围
    if (ugm3 < 0) {
      ugm3 = 0;
    }
    return ugm3;
}

void setup()
{
    Sch.init(); //初始化调度器

    Sch.addTask(ht_senser, 100, 50000, 1); //空气温湿度传感器，第 100 ms 开始，每 50000 ms (50 秒) 读取一次
    Sch.addTask(eh_senser, 200, 50000, 1); //电容式土壤湿度
    Sch.addTask(le_senser, 300, 50000, 1); //光照强度
    
    Sch.addTask(le_now, 0, 1000, 1);  // 实时光强
    // Sch.addTask(wl_now, 0, 1000, 1);  // 实时水位，已废弃
    Sch.addTask(eh_now, 0, 1000, 1);  // 实时土壤湿度
    Sch.addTask(pm25, 0, 1000, 1);    // 实时土壤湿度
    Sch.addTask(ultrasonic_ranging, 0, 1000, 1); // 实时超声波水位
    Sch.addTask(call_pump_realy, 0, 10000, 1); // 实时超声波水位

    Sch.start(); //启动调度器

    Serial.begin(9600);
    Wire.begin();
    dht.begin(); //启动传感器
    
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
    pinMode(PIN_DATA_OUT, INPUT); //定义为输入(ADC读取模拟量)
    pinMode(PIN_LED_VCC, OUTPUT); //定义为输出
}

void loop()
{
    Sch.dispatchTasks(); // 执行被调度的任务，用调度器时放上这一句即可
}

// 把要调度的任务函数放下面
void ht_senser()
{
    delay(200);                      //采样延时，每次抓取数据时间间隔 1~2秒钟
    float h = dht.readHumidity();    //读取湿度
    float t = dht.readTemperature(); //读取摄氏度

    if (isnan(h) || isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    } //检查抓取是否成功

    Serial.print("air_humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("air_temperature: ");
    Serial.print(t);
    Serial.println(" *C ");
}

// 土壤湿度
int eh_senser()
{
    delay(200);
    EH_Sensor_Value = analogRead(EH_Sensor_Pin);
    Serial.print("earth_humidity_min ");
    Serial.print(EH_Sensor_Value);
    Serial.println(" r");
    return EH_Sensor_Value;
}

// 土壤湿度
void eh_now()
{
    delay(200);
    EH_Sensor_Value = analogRead(EH_Sensor_Pin);  
    Serial.print("earth_humidity_now ");
    Serial.print(EH_Sensor_Value);
    Serial.println(" r");
}

// // 水位
// void wl_now()
// {
//     delay(200);
//     WL_Sensor_Value = analogRead(WL_Sensor_Pin);
//     Serial.print("water_level_now ");
//     Serial.print(WL_Sensor_Value);
//     Serial.println(" r");
// }

// 光强函数
void le_senser()
{
    Wire.beginTransmission(ADDRESS_BH1750FVI);  //"notify" the matching device
    Wire.write(ONE_TIME_H_RESOLUTION_MODE);     //set operation mode
    Wire.endTransmission();
        
    delay(180);

    Wire.requestFrom(ADDRESS_BH1750FVI, 2); //ask Arduino to read back 2 bytes from the sensor
    highByte = Wire.read();   // get the high byte
    lowByte = Wire.read();    // get the low byte
    
    sensorOut = (highByte<<8)|lowByte;
    illuminance = sensorOut/1.2;
    Serial.print("luminous_emittance_min ");
    Serial.print(illuminance);    
    Serial.println(" lux");

    delay(1000);
}

// 实时光强
void le_now()
{
    Wire.beginTransmission(ADDRESS_BH1750FVI);  //"notify" the matching device
    Wire.write(ONE_TIME_H_RESOLUTION_MODE);     //set operation mode
    Wire.endTransmission();
        
    delay(180);

    Wire.requestFrom(ADDRESS_BH1750FVI, 2);   //ask Arduino to read back 2 bytes from the sensor
    highByte = Wire.read();                   // get the high byte
    lowByte = Wire.read();                    // get the low byte
    
    sensorOut = (highByte<<8)|lowByte;
    illuminance = sensorOut/1.2;
    Serial.print("luminous_emittance_now ");
    Serial.print(illuminance);
    Serial.println(" lux");

    delay(1000);
}

// 超声波测距
int ultrasonic_ranging()
{
    //给Trig发送一个低高低的短时间脉冲,触发测距
    digitalWrite(Trig, LOW);  //给Trig发送一个低电平
    delayMicroseconds(2);     //等待 2微妙
    digitalWrite(Trig,HIGH);  //给Trig发送一个高电平
    delayMicroseconds(10);    //等待 10微妙
    digitalWrite(Trig, LOW);  //给Trig发送一个低电平
    
    temp = float(pulseIn(Echo, HIGH)); //存储回波等待时间,
    //pulseIn函数会等待引脚变为HIGH,开始计算时间,再等待变为LOW并停止计时
    //返回脉冲的长度
    
    //声速是:340m/1s 换算成 34000cm / 1000000μs => 34 / 1000
    //因为发送到接收,实际是相同距离走了2回,所以要除以2
    //距离(厘米)  =  (回波时间 * (34 / 1000)) / 2
    //简化后的计算公式为 (回波时间 * 17)/ 1000
    
    cm = (temp * 17 )/1000; //把回波时间换算成cm

    Serial.print("distance ");
    Serial.print(cm);       //串口输出距离换算成cm的结果
    Serial.print(" cm ");
    Serial.print("Echo ");
    Serial.println(temp);   //串口输出等待时间的原始数据
    
    delay(1000);
    return cm;
}

// 空气质量
void pm25() {
    double outputV = getOutputV();          //采样获取输出电压
    double ugm3 = getDustDensity(outputV);  //计算灰尘浓度
    double aqi = getAQI(ugm3);              //计算aqi
    String gradeInfo = getGradeInfo(aqi);   //计算级别

    //打印到串口
    Serial.println(String("outputV=") + outputV + "\tug/m3=" + ugm3 + "\tAQI=" + aqi + "\tgradeInfo=" + gradeInfo);

    //间隔1秒执行下次检测
    delay(1000);
}

void call_pump_realy()
{
    Serial.print("watering");
}

void watching_water_humidity()
{
    int low_water_level = 100; //set the water level Cordon
    int low_earth_humidity = 100; //set the earth humidity Cordon

    int cm = ultrasonic_ranging();
    int eh = EH_Sensor_Value();

    if(cm < low_water_level && eh < low_water_level)
    {
        Serial.print("low wl low eh warning");
    }
    else if(cm > low_water_level && eh < low_water_level)
    {
        Serial.print("call_pump_realy");
        call_pump_realy();
    }
    else if(cm > low_water_level && eh > low_water_level)
    {
        Serial.print("nothing to do");
    }
    // delay(10000);
}
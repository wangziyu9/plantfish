#include "TaskScheduler.h" //包含此头文件，才能使用调度器
#include "DHT.h"           //加载DHT11的库
#define DHTTYPE DHT11      // 定义传感器类似 DHT11
#define DHTPIN 12          //宏定义DHT数据接口，编译时DHTPIN会替换成2

// lux
#include <Wire.h>
#define ADDRESS_BH1750FVI 0x23    //ADDR="L" for this module
#define ONE_TIME_H_RESOLUTION_MODE 0x20
byte highByte = 0;
byte lowByte = 0;
unsigned int sensorOut = 0;
unsigned int illuminance = 0;
// lux

DHT dht(DHTPIN, DHTTYPE); //声明 dht 函数
const int pinD = 12;

int EH_Sensor_Pin = A1;  // select the input pin for the potentiometer
int WL_Sensor_Pin = A2;  // select the input pin for the potentiometer
int EH_Sensor_Value = 0; // variable to store the value coming from the sensor
int WL_Sensor_Value = 0; // variable to store the value coming from the sensor
const float R = 1000.0;

void setup()
{
    Sch.init(); //初始化调度器

    Sch.addTask(ht_senser, 100, 50000, 1); //从第 0 毫秒开始闪烁 LED，每隔 1s, LED 状态改变一次
    Sch.addTask(eh_senser, 200, 50000, 1);
    Sch.addTask(le_senser, 300, 50000, 1);
    
    Sch.addTask(le_now, 0, 1000, 1);
    Sch.addTask(wl_now, 0, 1000, 1);
    Sch.addTask(eh_now, 0, 1000, 1);
    Sch.start(); //启动调度器

    Serial.begin(9600);
    Wire.begin();
    dht.begin(); //启动传感器
}

void loop()
{
    Sch.dispatchTasks(); // 执行被调度的任务，用调度器时放上这一句即可
}

// 把要调度的任务函数放下面

void ht_senser()
{
    delay(200);                     //采样延时，每次抓取数据时间间隔 1~2秒钟
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


void eh_senser()
{
    delay(200);
    EH_Sensor_Value = analogRead(EH_Sensor_Pin);
    Serial.print("earth_humidity_min ");
    Serial.print(EH_Sensor_Value);
    Serial.println(" r");
}

void eh_now()
{
    delay(200);
    EH_Sensor_Value = analogRead(EH_Sensor_Pin);  
    Serial.print("earth_humidity_now ");
    Serial.print(EH_Sensor_Value);
    Serial.println(" r");
}

void wl_now()
{
    delay(200);
    WL_Sensor_Value = analogRead(WL_Sensor_Pin);
    Serial.print("water_level_now ");
    Serial.print(WL_Sensor_Value);
    Serial.println(" r");
}

void le_senser()
{
    Wire.beginTransmission(ADDRESS_BH1750FVI); //"notify" the matching device
    Wire.write(ONE_TIME_H_RESOLUTION_MODE);     //set operation mode
    Wire.endTransmission();
        
    delay(180);

    Wire.requestFrom(ADDRESS_BH1750FVI, 2); //ask Arduino to read back 2 bytes from the sensor
    highByte = Wire.read();  // get the high byte
    lowByte = Wire.read(); // get the low byte
    
    sensorOut = (highByte<<8)|lowByte;
    illuminance = sensorOut/1.2;
    Serial.print("luminous_emittance_min ");
    Serial.print(illuminance);    
    Serial.println(" lux");

    delay(1000);
}

void le_now()
{
    Wire.beginTransmission(ADDRESS_BH1750FVI); //"notify" the matching device
    Wire.write(ONE_TIME_H_RESOLUTION_MODE);     //set operation mode
    Wire.endTransmission();
        
    delay(180);

    Wire.requestFrom(ADDRESS_BH1750FVI, 2); //ask Arduino to read back 2 bytes from the sensor
    highByte = Wire.read();  // get the high byte
    lowByte = Wire.read(); // get the low byte
    
    sensorOut = (highByte<<8)|lowByte;
    illuminance = sensorOut/1.2;
    Serial.print("luminous_emittance_now ");
    Serial.print(illuminance);
    Serial.println(" lux");

    delay(1000);
}
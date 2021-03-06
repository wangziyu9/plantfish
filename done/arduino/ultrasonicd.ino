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

*/

#include "TaskScheduler.h" //包含此头文件，才能使用调度器

// 温湿度传感器
#include "DHT.h"           //加载DHT11的库
#define DHTTYPE DHT11      // 定义传感器类似 DHT11
#define DHTPIN 12          //宏定义DHT数据接口，编译时DHTPIN会替换成2

DHT dht(DHTPIN, DHTTYPE); //声明 dht 函数
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
#define Trig 2 //引脚Tring 连接 IO D2
#define Echo 3 //引脚Echo 连接 IO D3 
 
float cm; //距离变量
float temp; // 超声波间隔时间

int EH_Sensor_Pin = A1;  // select the input pin for the potentiometer
int WL_Sensor_Pin = A2;  // select the input pin for the potentiometer
int EH_Sensor_Value = 0; // variable to store the value coming from the sensor
int WL_Sensor_Value = 0; // variable to store the value coming from the sensor
const float R = 1000.0;

void setup()
{
    Sch.init(); //初始化调度器

    Sch.addTask(ht_senser, 100, 50000, 1); //空气温湿度传感器，第 100 ms 开始，每 50000 ms (50 秒) 读取一次
    Sch.addTask(eh_senser, 200, 50000, 1); //电容式土壤湿度
    Sch.addTask(le_senser, 300, 50000, 1); //光照强度
    
    Sch.addTask(le_now, 0, 1000, 1); // 实时光强
    Sch.addTask(wl_now, 0, 1000, 1); // 实时水位，已废弃
    Sch.addTask(eh_now, 0, 1000, 1); // 实时土壤湿度
    Sch.addTask(ultrasonic_ranging, 0, 1000, 1); // 实时超声波水位
    Sch.start(); //启动调度器

    Serial.begin(9600);
    Wire.begin();
    dht.begin(); //启动传感器
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
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

void ultrasonic_ranging()
{
    //给Trig发送一个低高低的短时间脉冲,触发测距
  digitalWrite(Trig, LOW); //给Trig发送一个低电平
  delayMicroseconds(2);    //等待 2微妙
  digitalWrite(Trig,HIGH); //给Trig发送一个高电平
  delayMicroseconds(10);    //等待 10微妙
  digitalWrite(Trig, LOW); //给Trig发送一个低电平
  
  temp = float(pulseIn(Echo, HIGH)); //存储回波等待时间,
  //pulseIn函数会等待引脚变为HIGH,开始计算时间,再等待变为LOW并停止计时
  //返回脉冲的长度
  
  //声速是:340m/1s 换算成 34000cm / 1000000μs => 34 / 1000
  //因为发送到接收,实际是相同距离走了2回,所以要除以2
  //距离(厘米)  =  (回波时间 * (34 / 1000)) / 2
  //简化后的计算公式为 (回波时间 * 17)/ 1000
  
  cm = (temp * 17 )/1000; //把回波时间换算成cm

  Serial.print("distance ");
  Serial.print(cm);//串口输出距离换算成cm的结果
  Serial.print(" cm ");
  Serial.print("Echo ");
  Serial.println(temp);//串口输出等待时间的原始数据
  
  delay(1000);
}
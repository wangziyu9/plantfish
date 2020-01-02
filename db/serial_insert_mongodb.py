# 温度湿度
# 土壤湿度 ehMAX-ehMIN --> 600-300 --> 0-100
# 100 - (eh - 300) / (ehMAX - ehMIN) * 100
# 电阻水位 0-wlMAX --> 0-100
# wl / wlMAX * 100
# 超声波水位
# 0 cm --> max distance
# 100 --> 0
# 100 - (d / dMAX) * 100

import serial
import pymongo
import time

wlMAX = 1000
ehMAX = 700
ehMIN = 300
dMAX = 10  # cm

client = pymongo.MongoClient(host='localhost', port=27017)
# 连接数据库
db = client.rpi
# 选择数据库
collection_HT = db.HT
collection_WL = db.WL
collection_EH = db.EH
collection_LE = db.LE
# 选择数据集合

collection_n = db.now
collection_range = db.range

ser = serial.Serial("/dev/ttyUSB0", 9600)
# 从串口读取数据
line = ser.readline()


def trans_EH(eh):
    global ehMAX
    global ehMIN
    if eh > ehMAX:
        ehMAX = eh
    if eh < ehMIN:
        ehMIN = eh
    v = (100 - (eh - 300) / (ehMAX - ehMIN) * 100)
    return(round(v, 2))

def trans_WL(wl):
    global wlMAX
    if wl > wlMAX:
        wlMAX = wl
    v = (wl / wlMAX * 100)
    return(round(v, 2))


def tarns_SD(d):
    global dMAX
    if d > dMAX:
        return 0.01
    if d > 20:
        # dMAX = 10
        return False
    v = (100 - (d / dMAX) * 100)
    return(round(v, 2))


def write_HT(line):
    ht = {}
    ht['time'] = time.time()
    ht['humidity'] = float(line.split(' ')[1])
    ht['temperature'] = float(line.split(' ')[3])
    collection_HT.insert_one(ht)
    pass


def write_WL(line):
    wl = {}
    wl['time'] = time.time()
    wl['water_level'] = trans_WL(float(line.split(' ')[1]))
    collection_WL.insert_one(wl)
    pass


def write_EH(line):
    eh = {}
    eh['time'] = time.time()
    if line.split(' ')[1] == "inf":
        eh['earth_humidity'] = 100
    else:
        eh['earth_humidity'] = trans_EH(float(line.split(' ')[1]))
    collection_EH.insert_one(eh)
    pass


def write_LE(line):
    le = {}
    le['time'] = time.time()
    le['luminous_emittance'] = float(line.split(' ')[1])
    collection_LE.insert_one(le)
    pass


def write_WLn(title, line):
    value = trans_WL(float(line.split(' ')[1]))
    collection_n.update({'title': title}, {'title': title,
                                           'value': value}, upsert=True)
def write_EHn(title, line):
    value = trans_EH(float(line.split(' ')[1]))
    collection_n.update({'title': title}, {'title': title,
                                           'value': value}, upsert=True)
def write_SDn(title, line):
    value = tarns_SD(float(line.split(' ')[1]))
    if value:
        collection_n.update({'title': title}, {'title': title,
                                           'value': value}, upsert=True)
    else:
        pass

def write_LEn(title, line):
    value = float(line.split(' ')[1])
    collection_n.update({'title': title}, {'title': title,
                                           'value': value}, upsert=True)


while line:
    line = str(ser.readline())
    print(line)
    if('air_humidity' in line):
        write_HT(line)
    if('water_level_min' in line):
        write_WL(line)
    if('earth_humidity_min' in line):
        write_EH(line)
    if('luminous_emittance_min' in line):
        write_LE(line)

    if('water_level_now' in line):
        write_WLn('water_level_now', line)
    if('distance' in line):
        write_SDn('water_level_ultrasonic_now', line)
    if('earth_humidity_now' in line):
        write_EHn('earth_humidity_now', line)
    if('luminous_emittance_now' in line):
        write_LEn('luminous_emittance_now', line)
    else:
        pass

    #  print(line)

# water_level_now 0 r
# earth_humidity_now 623 r
# luminous_emittance_now 204 lux
# water_level_now 0 r
# earth_humidity_now 624 r
# air_humidity: 46.00 %	air_temperature: 27.00 *C
# earth_humidity_min 626 r
# luminous_emittance_min 204 lux
# luminous_emittance_now 204 lux
# water_level_now 0 r
# earth_humidity_now 627 r
# luminous_emittance_now 204 lux

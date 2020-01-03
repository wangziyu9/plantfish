import serial #pyserial?
import pymongo
import time

ser = serial.Serial("COM4", 9600)

client = pymongo.MongoClient(host='localhost', port=27017)
db = client.aqi
collection_n = db.now
doc = collection_n.find({})

def write_AQI(line):
    value = float(line.split(' ')[5]) + 17
    print(value)
    update = {'aqi': value}
    collection_n.update({'time':'now'},{'$set':update}, upsert=True)
    pass
def write_LT(line):
    value = float(line.split(' ')[1])
    print(value)
    update= {'light': value}
    collection_n.update({'time':'now'},{'$set':update}, upsert=True)
    pass
def write_HT(line):
    value = float(line.split(' ')[1]) / 4
    print(value)
    value1 = float(line.split(' ')[3]) * 2
    print(value1)
    update = {'air_humidity': value,'air_temperature': value1}
    collection_n.update({'time':'now'},{'$set':update}, upsert=True)
    pass

line = ser.readline()
while(line):
    line = str(ser.readline())
    print(line)
    if('AQI' in line):
        write_AQI(line)
    if('air_humidity' in line):
        write_HT(line)
    if("Light" in line):
        write_LT(line)
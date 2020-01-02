# read data from mongoDB and return json dict
# 根据时间戳，查找时间最接近的数据。
import pymongo
import time
import config_date

client = pymongo.MongoClient(host='10.84.126.18', port=27017)
db = client.rpi
collection_HT = db.HT
collection_WL = db.WL
collection_EH = db.EH
collection_n = db.get_now

def get_now():
    now = {}
    ds = db.now.find()
    for d in ds:
        now[d['title']] = d['value']
        print(now)
    return now
    # {'water_level_now': 0.0, 'earth_humidity_now': 27.75, 'luminous_emittance_now': 243.0, 'water_level_ultrasonic_now': 63.98763523956723}
    
def find_data(time_stamp):
    d = {}
    # 这里性能很差

    dht = db.HT.find({"time":{'$gt' : time_stamp}}).sort([("time",1)]).limit(1)
    dwl = db.WL.find({"time":{'$gt' : time_stamp}}).sort([("time",1)]).limit(1)
    deh = db.EH.find({"time":{'$gt' : time_stamp}}).sort([("time",1)]).limit(1)

    d["dht"] = dht
    d["dwl"] = dwl
    d["deh"] = deh
    return d

def get_last_data():
    data = {}
    humidity = []
    temperature = []
    water_level = []
    earth_humidity = []
    time_str = []

    dhts = collection_HT.find().limit(12)
    # dwls = collection_WL.find().limit(12)
    dehs = collection_EH.find().limit(12)

    for d in dhts:
        humidity.append(d['humidity'])
        temperature.append(d['temperature'])
    # for d in dwls:
    #     # print(d)
    #     water_level.append(d['water_level'])
    #     # 这里键错了，日后修正过来。已修正
    for d in dehs:
        earth_humidity.append(d['earth_humidity'])
    for i in range(0, 12):
        time_str.append(12 - i)

    data['humidity'] = humidity
    data['temperature'] = temperature
    data['water_level'] = water_level
    data['earth_humidity'] = earth_humidity
    data['time_str'] = time_str
    
    return(data)
    
# def get_data_by_time():
#     return
#     pass

def get_data_by_time_list(interval = 60):
    data = {}
    humidity = []
    temperature = []
    water_level = []
    earth_humidity = []
    time_str = []

    time_date = config_date.get_time_date(interval)
    for t in time_date["time_stamp"]:
        d = find_data(t)
        for ht in d['dht']:
            humidity.append(ht['humidity'])
            temperature.append(ht['temperature'])
        for wl in d['dwl']:
            water_level.append(wl['water_level'])
        for eh in d['deh']:
            earth_humidity.append(eh['earth_humidity'])

    # humidity.append(d['humidity'])
    # temperature.append(d['temperature'])
    # water_level.append(d['water_level'])
    # earth_humidity.append(d['earth_humidity'])
    data['humidity'] = humidity
    data['temperature'] = temperature
    data['water_level'] = water_level
    data['earth_humidity'] = earth_humidity
    data['time_str'] = time_str

    data['time_str'] = time_date['time_str']
    return data
    pass 

if __name__ == "__main__":
    # print(get_last_data())
    
    # print(get_data_by_time_list(3600))
    print(get_data_by_time_list())

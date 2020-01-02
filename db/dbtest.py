# read data from mongoDB and return json dict
import pymongo
import time

client = pymongo.MongoClient(host='192.168.137.194', port=27017)
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
    # {'water_level_now': 0.0, 'earth_humidity_now': 624.0, 'luminous_emittance_now': 123.0}

if __name__ == "__main__":
    while True:
        get_now()
        time.sleep(2)
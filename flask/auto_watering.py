# 监视土壤湿度，自动浇水
# encoding:utf-8
import ralay_switch
import pymongo
import time
from gpiozero import LED

client = pymongo.MongoClient(host='10.84.126.18', port=27017)
db = client.rpi
collection_EH = db.EH
collection_FLAG = db.FLAG
collection_n = db.get_now

# led = LED(26)

def st(state = "false"):
    db.FLAG.update({},({'$set':{"state":state}}))
    
def rt():
    for d in db.FLAG.find().limit(1):
        flag = (d["state"])
    return flag

def get_now():
    now = {}
    ds = db.now.find()
    for d in ds:
        now[d['title']] = d['value']
        # print(now)
    return now

def oversee(): 
    while True:
        print("oversee ")
        flag = rt()
        print("flag in oversee " + flag)
        # flag = "true"
        if flag == "true":
            # ds = db.now.find()
            # deh = db.EH.find().sort([("time",-1)]).limit(1)
            # for d in deh:
            d = get_now()
            if d["earth_humidity_now"] < 30:# and d["earth_humidity"] < 90000:
                # ralay_switch.watering()
                print(d)
                led = LED(24)#土壤湿度较低，开始浇水
                led.on()
                time.sleep(5)
                led = 0
                pass
        else:
            time.sleep(5)
        time.sleep(60) #一分钟之内不再浇水

if __name__ == "__main__":
    oversee()
# encoding:utf-8
import time
from flask import Flask,render_template,request,jsonify
# from gpiozero import LED
import threading
from multiprocessing import Process

import ralay_switch
import config_json_by_time
# import config_json
import auto_watering

app = Flask(__name__)
ralay_gpio = 24
ralay = 0

MINUTE = 60
HOUR = 60 * MINUTE
HOUR2 = HOUR * 2
DAY = 24 * HOUR
WEEK = 7 * DAY 
MONTH = 30 * DAY

def oversee():
    # p = Process(target=auto_watering.oversee, args=())
    # p.start()
    t = threading.Thread(target=auto_watering.oversee, name='oversee')
    t.start()
    return

@app.route('/')
def hello_world():
    return render_template("main.html")
@app.route('/main.html')
def hello_world1():
    return render_template("main.html")


@app.route('/watering')
def watering():
    # oversee()
    return render_template("watering.html")
@app.route('/watering.html')
def watering1():
    # oversee()
    return render_template("watering.html")

@app.route('/watering_10')
def watering_10():
    ralay_switch.watering(10)
    return render_template("watering.html")
@app.route('/watering_30')
def watering_30():
    ralay_switch.watering(30)
    return render_template("watering.html")
@app.route('/watering_60')
def watering_60():
    ralay_switch.watering(60)
    return render_template("watering.html")

@app.route('/watering_off')
def watering_off():
    ralay_switch.stop()
    return render_template("watering.html")
@app.route('/watering_on')
def watering_on():
    ralay_switch.watering(50)
    return render_template("watering.html")
@app.route('/watering_once')
def watering_once():
    ralay_switch.watering()
    return render_template("watering.html")

@app.route('/data/now', methods=['GET', 'POST'])
def rtn_data_now():
    # d = config_json.get_data()
    d = config_json_by_time.get_now()
    return jsonify(d)

@app.route('/data', methods=['GET', 'POST'])
def rtn_data():
    # d = config_json.get_data()
    d = config_json_by_time.get_data_by_time_list(MINUTE)
    return jsonify(d)
@app.route('/data/last', methods=['GET', 'POST'])
def rtn_data_default():
    d = config_json_by_time.get_last_data()
    return jsonify(d)

@app.route('/data/minute', methods=['GET', 'POST'])
def rtn_data_by_minute():
    d = config_json_by_time.get_data_by_time_list(MINUTE)
    return jsonify(d)
@app.route('/data/hour', methods=['GET', 'POST'])
def rtn_data_by_hour():
    d = config_json_by_time.get_data_by_time_list(HOUR)
    print(d)
    return jsonify(d)
@app.route('/data/day', methods=['GET', 'POST'])
def rtn_data_by_day():
    d = config_json_by_time.get_data_by_time_list(DAY)
    print(d)
    return jsonify(d)

@app.route('/watering_state', methods=['GET', 'POST'])
def auto_watering_state():
    if request.method == 'POST':
        v = request.values.get("watering_state", "false")
        print("post switch state " + v)
        auto_watering.st(v)
        return v + "statu"
    if request.method == 'GET':
        return auto_watering.rt()
    else:
        return "balabalanothing"


if __name__ == '__main__':
    # oversee()
    app.run(host='0.0.0.0', threaded=True) # debug = True)

# encoding:utf-8
# 没有用
import time
from flask import Flask,render_template,request,jsonify
import ralay_switch
import config_json_by_time
import config_json
from gpiozero import LED

app = Flask(__name__)
ralay_gpio = 24
ralay = 0

MINUTE = 60
HOUR = 60 * MINUTE
HOUR2 = HOUR * 2
DAY = 24 * HOUR
WEEK = 7 * DAY 
MONTH = 30 * DAY

@app.route('/')
def hello_world():
    return render_template("HTW_charts.html")

@app.route('/watering')
def watering():
    return render_template("watering_switch.html")

@app.route('/watering_off')
def watering_off():
    ralay_switch.stop()
    return render_template("watering_switch.html")

@app.route('/watering_on')
def watering_on():
    ralay_switch.watering(50)
    return render_template("watering_switch.html")

@app.route('/watering_once')
def watering_once():
    ralay_switch.watering()
    return render_template("watering_switch.html")

@app.route('/data', methods=['GET', 'POST'])
def rtn_data():
    d = config_json.get_data()
    # d = config_json_by_time.get_data_by_time_list()
    return jsonify(d)

@app.route('/data/day', methods=['GET', 'POST'])
def rtn_data_by_day():
    # d = config_json.get_data()
    d = config_json_by_time.get_data_by_time_list(DAY)
    return jsonify(d)

if __name__ == '__main__':
    app.run(host='0.0.0.0', threaded=True) # debug = True)
# encoding:utf-8
# RGB 小灯，未加入主程序
import RPi.GPIO as GPIO
import time
from flask import Flask,render_template,request
app = Flask(__name__)
GPIO.setwarnings(False)
R = 22
G = 27
B = 17
GPIO.setmode(GPIO.BCM)
GPIO.setup(R, GPIO.OUT)
GPIO.setup(G, GPIO.OUT)
GPIO.setup(B, GPIO.OUT)

pwmR = GPIO.PWM(R, 80)
pwmG = GPIO.PWM(G, 80)
pwmB = GPIO.PWM(B, 80)

pwmR.start(10)
pwmG.start(10)
pwmB.start(10)

time.sleep(2)
print("inited")

def get_r_g_b(rgb):
    r_g_b = {}
    r_g_b["r"] = int(rgb[0:2], 16) * 100 / 255 * 1
    r_g_b["g"] = int(rgb[2:4], 16) * 100 / 255 * 0.3
    r_g_b["b"] = int(rgb[4:6], 16) * 100 / 255 * 0.5
    return r_g_b

def init_rgb():
    pass

def set_rgb(r_g_b):
    print("OK")

    pwmR.start(r_g_b["r"])
    pwmG.start(r_g_b["g"])
    pwmB.start(r_g_b["b"])

@app.route('/')
def hello_world():
    return render_template("main.html")

@app.route('/color', methods=['GET', 'POST'])
def pst():
    if request.method == 'POST':
        rgb = request.values.get("color", 0)
        r_g_b = get_r_g_b(rgb)
        print(r_g_b)
        set_rgb(r_g_b)
        return "post baka" + rgb
    else:
        return "not post"


if __name__ == '__main__':
    init_rgb()
    app.run(host='0.0.0.0')
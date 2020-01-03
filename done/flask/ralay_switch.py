# 接收网页的控制，实现浇水的功能
from gpiozero import LED
# from gpiozero import LED
import time

ralay_gpio = 24
stp = 1
# 标志位，主要用于结束浇水，并只允许同时接受一项浇水作业


def watering(watering_time=5):
    global stp
    if stp == 0:
        return
        # 正在浇水作业
    stp = 0
    ralay = LED(ralay_gpio)
    # 树莓派继电器输出接口
    # 以秒为单位，可以接受中断
    for i in range(0, watering_time):
        if(stp == 0):
            time.sleep(1)
        else:
            break
    ralay = 0
    stp = 1


def stop():
    global stp
    stp = 1


if __name__ == "__main__":
    watering()

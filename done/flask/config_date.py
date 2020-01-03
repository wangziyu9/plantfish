# 根据时间计算时间戳
import pymongo
import time

MINUTE = 60
HOUR = 60 * MINUTE
HOUR2 = HOUR * 2
DAY = 24 * HOUR
WEEK = 7 * DAY 
MONTH = 30 * DAY

def get_time_date(interval = HOUR2):
    time_data = {}
    time_str = []
    time_stamp_list = [(time.time() - interval * x) for x in range(12)]
    for t in time_stamp_list:
        # s =  s = "%02d"%time.localtime(t).tm_hour + ":" + "%02d"%time.localtime(t).tm_min
        # time_str.append(s)
        # time_str.append(str(time.localtime(t).tm_hour) + ':' + str(time.localtime(t).tm_min))
        s = formart_str(interval, t)
        time_str.append(s)
    time_stamp_list.reverse()
    time_str.reverse()
    time_data['time_str'] = time_str
    time_data['time_stamp'] = time_stamp_list
    return(time_data)

def formart_str(interval, t):
    if interval == MINUTE:
        s = "%02d"%time.localtime(t).tm_hour + ":" + "%02d"%time.localtime(t).tm_min
    elif interval == HOUR or interval == HOUR2:
        s = "%02d"%time.localtime(t).tm_mday + "日" + "%02d"%time.localtime(t).tm_hour + "时"
    elif interval == DAY:
        s = "%02d"%(time.localtime(t).tm_mon) + "月" + "%02d"%time.localtime(t).tm_mday + "日"
    else:
        return ''
    return s

if __name__ == "__main__":
    print(get_time_date(DAY))
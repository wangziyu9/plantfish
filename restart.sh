# 每分钟检测程序运行状态，若未运行，重启程序
echo "starting MongoDB"
if [ $(docker container ls -a | grep mongo | grep 'Exited (100)' | awk '{print length($0)}') ]
then
    sudo rm /home/pi/data/db/mongo/mongod.lock;
    docker start rpi_mongodb;
    echo "delete lock and started MongoDB"
elif [ $(docker container ls -a | grep mongo | grep 'Up' | awk '{print length($0)}') ]
then
    echo "MongoDB looks good"
else
    docker start rpi_mongodb;
    echo "MongoDB started normally"
fi
# sudo rm /home/pi/data/db/mongo/mongod.lock;
# docker start rpi_mongodb;

echo "starting flarry">> /home/pi/testboot.txt
# /home/pi/flarry/data/serial_insert_mongodb.py
if [ $(ps -ef | grep serial_insert_mongodb | awk '{print length($0)}') ]
then
    nohup python3 /home/pi/flarry/data/serial_insert_mongodb.py &
    echo "serial-mongodb started"
fi

# /home/pi/flarry/flask/flarry.py
if [ $(ps -ef | grep flarry.py | awk '{print length($0)}') ]
then
    nohup python3 /home/pi/flarry/flask/flarry.py &
    echo "falrry started"
fi

if [ $(ps -ef | grep auto_watering.py | awk '{print length($0)}') ]
then
    nohup python3 /home/pi/flarry/flask/auto_watering.py &
    echo "auto_watering started"
fi
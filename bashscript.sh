#!/bin/sh
IP=pi@10.140.30.109
DIRPI=/home/pi/Desktop/E101
DIRLOCAL="/mnt/c/Users/Mike/Documents/AVC/bashscript/move"
echo Deleting old c files
sshpass -p raspberry ssh $IP 'cd $DIRLOCAL; rm *.cpp'
echo Transferring new files
rsync -e 'sshpass -p raspberry ssh -ax' -av $DIRLOCAL $IP:$DIRPI
echo compiling and executing
sshpass -p raspberry ssh $IP 'cd /home/pi/Desktop/E101/move;make;echo raspberry | sudo -S  ./main'

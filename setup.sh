DIR="/home/pi/BB_Code_1_5/Code/"
if [ -d "$DIR" ]; then
  echo "'/home/pi/BB_Code_1_5/' folder exist ..."
  chown -R pi:pi /home/pi/BB_Code_1_5/
  mv -f BB-Code/Code/* /home/pi/BB_Code_1_5/Code/
  mv -f BB-Code/ReadMe.txt /home/pi/BB_Code_1_5/
  cd /home/pi/BB_Code_1_5/
  make clean
  make
  rm -rf logs/*
else
  DIR="/home/pi/Desktop/Client-Code/BB_Code_1_5/Code/"
  if [ -d "$DIR" ]; then
    echo "'/home/pi/Desktop/Client-Code/BB_Code_1_5/Code/' folder doesn't exist ..."
    chown -R pi:pi /home/pi/Desktop/Client-Code/
    mv -f BB-Code/Code/* /home/pi/Desktop/Client-Code/BB_Code_1_5/Code/
    mv -f BB-Code/ReadMe.txt /home/pi/Desktop/Client-Code/BB_Code_1_5/
    cd /home/pi/Desktop/Client-Code/BB_Code_1_5/
    make clean
    make
    rm -rf logs/*
  else
    echo "'/home/pi/BB_Code_1_5/' folder doesn't exist ..."
    echo "'/home/pi/Desktop/Client-Code/BB_Code_1_5/Code/' folder doesn't exist ..."
   fi
fi

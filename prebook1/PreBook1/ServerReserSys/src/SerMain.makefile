g++ -std=c++0x EventObj.cpp MyLibevent.cpp server.cpp sqlconn.cpp -L/usr/lib/x86_64-linux-gnu -ljsoncpp -levent -lmysqlclient -o SerMain

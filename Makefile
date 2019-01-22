# Makefile
GTKLIB=`pkg-config --cflags --libs gtk+-3.0 gmodule-2.0` -rdynamic
all: clean referee player gui-player

referee: referee.cpp 
	g++ -Wall -g -o referee referee.cpp -std=c++11
player: player.cpp 
	g++ -Wall -g -o player player.cpp -std=c++11
gui-player: gui-player.cpp 
	g++ -Wall -g -o  gui-player gui-player.cpp -std=c++11 $(GTKLIB)
clean:
	rm -f *.o referee player gui-player *.*~
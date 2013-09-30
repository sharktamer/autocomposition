SHELL = /bin/sh
CPP = g++
OBJECTS = main.o midifile.o

autocomposition: $(OBJECTS)
	$(CPP) $(OBJECTS) -o autocomposition

midifile.o: midifile.cpp midifile.h
	$(CPP) -c midifile.cpp

main.o: main.cpp midifile.h
	$(CPP) -c main.cpp

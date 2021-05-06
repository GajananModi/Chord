CXX = g++

default: ChordClient

ChordClient: ChordClient.o ChordNode.o Utilities.o
	$(CXX) -g -o ChordClient ChordClient.o ChordNode.o Utilities.o -lcrypto -lssl -lpthread

Utilities.o: Utilities.cpp Utilities.hpp
	$(CXX) -g -c Utilities.cpp

ChordNode.o: ChordNode.cpp ChordNode.hpp Utilities.cpp Utilities.hpp ChordClient.cpp
	$(CXX) -g -c ChordNode.cpp Utilities.cpp

ChordClient.o: ChordClient.cpp Utilities.cpp Utilities.hpp ChordNode.cpp ChordNode.hpp
	$(CXX) -g -c Utilities.cpp ChordClient.cpp ChordNode.cpp
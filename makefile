OBJECTS=Test.o
CXX=clang++-9 
CXXFLAGS=-std=c++2a 

all: client main1 gurde singleton selectserver selectclient test
	
gurde: gurde.o 
	$(CXX) -lpthread $(CXXFLAGS) $^ -o gurde

singleton: singleton.o 
	$(CXX) -lpthread $(CXXFLAGS) $^ -o singleton

selectserver: selectserver.o reactor.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o selectserver


selectclient: selectclient.o reactor.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o selectclient

reactor.o: reactor.cpp  reactor.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

test: Test.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o test

ob_test: ob_test.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o ob_test

client: client.o 
	$(CXX) $(CXXFLAGS) $^ -o client

main1: main1.o 
	$(CXX) -lpthread $(CXXFLAGS) $^ -o main1

Ex6: $(OBJECTS)
	gcc --shared -fPIC -pthread $(OBJECTS) 

clean:
	rm -f *.o client main1 gurde singleton selectserver selectclient test ob_test

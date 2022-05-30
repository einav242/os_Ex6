OBJECTS= active_object.o queue.o pipeline.o gurde.o singleton.o reactor.o
CXX=clang++-9 
CXXFLAGS=-std=c++2a 

all: Ex6.so client main1 selectserver selectclient test ob_test test_gurde test_singleton test_reactor
	
#q1-q3
main1: main1.o queue.o active_object.o pipeline.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o main1 

client: client.o 
	$(CXX) $(CXXFLAGS) $^ -o client

main1.o: main1.cpp queue.hpp active_object.hpp pipeline.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

#q1
queue.o: queue.cpp queue.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@
#q2
active_object.o: active_object.cpp active_object.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@
#q3
pipeline.o: pipeline.cpp pipeline.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

ob_test: ob_test.o 
	$(CXX) -lpthread $(CXXFLAGS) $^ -o ob_test

#q4
test_gurde: test_gurde.o gurde.o
		$(CXX) -lpthread $(CXXFLAGS) $^ -o test_gurde

test_gurde.o: test_gurde.cpp gurde.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

gurde.o: gurde.cpp gurde.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

#q5
test_singleton: test_singleton.o singleton.o
		$(CXX) -lpthread $(CXXFLAGS) $^ -o test_singleton

test_singleton.o: test_singleton.cpp singleton.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

singleton.o: singleton.cpp singleton.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

#q6
selectserver: selectserver.o reactor.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o selectserver

selectclient: selectclient.o reactor.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o selectclient

test_reactor: test_reactor.o reactor.o
		$(CXX) -lpthread $(CXXFLAGS) $^ -o test_reactor

test_reactor.o: test_reactor.cpp reactor.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

#test q4-q6
test: Test.o gurde.o singleton.o reactor.o
	$(CXX) -lpthread $(CXXFLAGS) $^ -o test

Test.o: Test.cpp gurde.cpp gurde.hpp singleton.cpp singleton.hpp reactor.cpp reactor.hpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

#library
Ex6.so:$(OBJECTS)
	$(CXX) -shared -fpic -o Ex6.so $(OBJECTS)


clean:
	rm -f *.o client main1 gurde singleton selectserver selectclient test ob_test  test_gurde test_singleton test_reactor  Ex6.so
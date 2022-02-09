CXX = g++
LDFLAGS = -std=c++11 -l sqlite3 -pthread -lcrypto 

bin/client: obj/client.o obj/logger.o obj/splitAndJoin.o
	$(CXX) -o bin/client obj/client.o obj/logger.o obj/splitAndJoin.o $(LDFLAGS)

bin/server: obj/server.o obj/operation.o obj/products.o obj/logger.o obj/splitAndJoin.o
	$(CXX) -o bin/server obj/server.o obj/operation.o obj/products.o obj/logger.o obj/splitAndJoin.o $(LDFLAGS)

bin/setupServer: obj/setupServer.o obj/products.o obj/logger.o
	$(CXX) -o bin/setupServer obj/setupServer.o obj/products.o obj/logger.o $(LDFLAGS)

obj/logger.o: src/logger.cpp include/logger.hpp
	$(CXX) -c src/logger.cpp -o obj/logger.o

obj/operation.o: src/operation.cpp include/operation.hpp
	$(CXX) -c src/operation.cpp -o obj/operation.o

obj/products.o: src/products.cpp include/products.hpp
	$(CXX) -c src/products.cpp -o obj/products.o

obj/splitAndJoin.o: src/splitAndJoin.cpp include/splitAndJoin.hpp
	$(CXX) -c src/splitAndJoin.cpp -o obj/splitAndJoin.o

obj/%.o: src/%.cpp
	$(CXX) -c -o $@ $<

clean:
	rm -r bin/* obj/*

all: bin/client bin/server bin/setupServer
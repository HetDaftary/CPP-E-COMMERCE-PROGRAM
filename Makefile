CXX = g++
LDFLAGS = -std=c++11 -l sqlite3 -pthread -lcrypto

bin/client: obj/client.o obj/logger.o
	$(CXX) -o bin/client obj/client.o obj/logger.o $(LDFLAGS)

bin/server: obj/server.o obj/operation.o obj/products.o obj/logger.o
	$(CXX) -o bin/server obj/server.o obj/operation.o obj/products.o obj/logger.o $(LDFLAGS)

bin/setupServer: obj/setupServer.o obj/products.o obj/logger.o
	$(CXX) -o bin/setupServer obj/setupServer.o obj/products.o obj/logger.o $(LDFLAGS)

obj/logger.o: src/logger.cpp src/logger.hpp
	$(CXX) -c src/logger.cpp -o obj/logger.o

obj/operation.o: src/operation.cpp src/operation.hpp
	$(CXX) -c src/operation.cpp -o obj/operation.o

obj/products.o: src/products.cpp src/products.hpp
	$(CXX) -c src/products.cpp -o obj/products.o

obj/%.o: src/%.cpp
	$(CXX) -c -o $@ $<

clean:
	rm -r bin obj libs
	mkdir bin obj libs

all: bin/client bin/server bin/setupServer
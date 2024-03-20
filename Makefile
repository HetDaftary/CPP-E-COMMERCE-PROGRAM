CXX = g++
LDFLAGS = -lsqlite3 -pthread -lcrypto 
CFLAGS = -std=c++11 -Iinclude

obj/%.o: src/%.cpp include/%.hpp
	$(CXX) -c $< $(CFLAGS) -o $@

bin/client: obj/client.o obj/logger.o obj/splitAndJoin.o
	mkdir -p obj bin data/Logs 
	$(CXX) -o $@ $^ $(LDFLAGS)

bin/server: obj/server.o obj/operation.o obj/products.o obj/logger.o obj/splitAndJoin.o
	$(CXX) -o $@ $^ $(LDFLAGS)

bin/setupServer: obj/setupServer.o obj/products.o obj/logger.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -r bin/* obj/*

all: bin/client bin/server bin/setupServer
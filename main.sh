g++ src/setupServer.cpp -o build/setupServer.out -std=c++11 -l sqlite3
g++ src/client.cpp -o build/client.out -std=c++11 -l sqlite3 -lcrypto
g++ src/server.cpp -o build/server.out -std=c++11 -l sqlite3 -pthread
if [ ! -d data/Logs ] ; then
    mkdir -p data/Logs
fi

g++ src/setupServer.cpp -o build/setupServer -std=c++11 -l sqlite3
g++ src/client.cpp -o build/client -std=c++11 -l sqlite3 -lcrypto
g++ src/server.cpp -o build/server -std=c++11 -l sqlite3 -pthread
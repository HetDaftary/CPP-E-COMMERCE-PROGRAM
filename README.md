# CPP-E-COMMERCE-PROGRAM

Creating a basic C++ console app for e-commerce related work.

- This code runs on client-server architecture.
- Seller and Costumer codes are clients to the server and a server code will be running.
- For now, we are using a loopback ip. 
- Port used will be 54000.
- Data would be stored in sqlite database.
- We store data of orders in a TXT file.
- The data passed between client and server will be in TXT FORMAT.
- We use SHA512 hashing to store passwords from openssl.

## Schema of Database

<img src="img/drawSQL-export-2022-01-19_17 09.png">

## UML Diagram for classes

<img src = "img/Class Diagram.png">

## Credits

- [@Sloan Kelly](https://www.youtube.com/channel/UC4LMPKWdhfFlJrJ1BHmRhMQ) for tutorial on making a TCP client and server in Linux.
- [@Jacob Scober](https://www.youtube.com/channel/UCwd5VFu4KoJNjkWJZMFJGHQ) for tutorial on mutli-threaded server.
- Logger is borrowed from [@danielblagy](https://github.com/danielblagy/Yelloger) Yelloger repo which I got to know about by his [YouTube Tutorial series](https://www.youtube.com/watch?v=1rnmYBSppYY&list=PL5Lk2LPoiyAKcw7T-_FB_4BNrWkxfwnus)
- This logger library is updated by me to log about various types of logs in di
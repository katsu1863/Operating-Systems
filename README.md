# Operating-Systems
Public repository for CSCE 36103 Operating Systems at the University of Arkansas. All projects are programmed in C and utilize basic OS concepts, such as processes, multi-threading, and sockets.

## Project 1
Uses multiple processes and ordinary pipes to copy the contents of a given file to a specified destination file. Filenames are passed as arguments through the command line.

To run, compile the program by running the command `make`. Then, run `./filecopy [input filename] [output filename]`.

## Project 2
Uses multi-threaded programming and thread synchronization to simulate traffic on a one-way bridge. Each vehicle (car or truck) is represented as a thread and must safely navigate arrival, crossing, and departure stages while adhering to bridge constraints. Vehicles can either arrive from the north or south, but traffic can only move in one direction at a time. Trucks are given absolute priority, but only one truck can be on the bridge at a time, and if there are waiting trucks in both directions, the direction of traffic must be switched between trucks to maintain fairness. Up to 3 cars can be on the bridge, and cars moving in the same direction will have priority. Synchronization is achieved through mutex locks and condition variables to avoid busy waiting and deadlocks.

To run, compile the program by running the command `make`. Then, run `./bridge_crossing`.

## Project 3
Uses client-server architecture and TCP sockets to create a chatroom application. The central server program manages multiple chat rooms and handles client requests and join, exit, and send messages. Valid messages sent by clients will then be broadcasted to other clients within the specified chat room. Threads are utilized to ensure that incoming and outgoing messages from the client are not delayed.

To run, compile the programs by running the command `make`. Then, run `./tcpserver` to start the central chat server. Run `./tcpclient` on a separate machine or command prompt to start the client program.

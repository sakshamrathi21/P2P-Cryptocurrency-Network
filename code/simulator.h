// Contains the Simulator class which is responsible for handling the events.
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <queue>
#include <vector>
#include <string>
#include "peer.h"
#include "transaction.h"
#include "block.h"
#include "event.h"
#include "helper.h"
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;

class Peer;  // Forward declaration

// Referring from main.cpp
extern int num_nodes;
extern vector<Peer*> peers;

class Simulator {
public:
    Simulator(double Time) {
        meanTime = Time;
        currentTime = 0.0;
    }
    void run();            
    double getCurrentTime() const { return currentTime; }
    void scheduleEvent(double time, EventType type, int sourcePeer, int targetPeer, EventData txn);
    double getInterArrivalTime();
    double meanTime;       
private:
    std::priority_queue<Event> eventQueue; 
    double currentTime;                   
    void handleEvent(Event& event);  
};

#endif 

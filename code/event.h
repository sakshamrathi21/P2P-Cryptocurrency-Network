/* This file contains the event class */
#ifndef EVENT_H
#define EVENT_H

#include "transaction.h"
#include "block.h"
#include <variant>

typedef std::variant<Transaction, Block> EventData; // type safe union

// Enum to represent the type of the event. It creates a mapping between the event type and a string.
enum EventType {
    CREATE_TRANSACTION,
    TRANSACTION_SEND,
    TRANSACTION_RECEIVE,
    MINING_START,
    MINING_END,
    BLOCK_SEND,
    BLOCK_RECEIVE
};

struct Event {
    double time;           
    EventType type;        
    int sourcePeer;        
    int targetPeer;       
    EventData data;       

    Event(double time, EventType type, int sourcePeer, int targetPeer, EventData data)
        : time(time), type(type), sourcePeer(sourcePeer), targetPeer(targetPeer), data(data) {}

    bool operator<(const Event& other) const {
        // For sorting the events in the priority queue
        return time > other.time; 
    }
    string eventTypeToString() {
        /* This function creates a mapping between the type of the event and a string, which can be used for printing */
        switch (type) {
            case CREATE_TRANSACTION: return "CREATE_TRANSACTION";
            case TRANSACTION_SEND: return "TRANSACTION_SEND";
            case TRANSACTION_RECEIVE: return "TRANSACTION_RECEIVE";
            case MINING_START: return "MINING_START";
            case MINING_END: return "MINING_END";
            case BLOCK_SEND: return "BLOCK_SEND";
            case BLOCK_RECEIVE: return "BLOCK_RECEIVE";
            default: return "UNKNOWN_EVENT";
        }
    }

};

#endif 
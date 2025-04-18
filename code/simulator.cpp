#include "simulator.h"

void Simulator::run() {
    // This function is the main function that runs the simulation
    generateConnectedGraph(); // Generate a connected graph of peers
    for (int i = 0 ; i < num_nodes ; i++ ) {
        // Create genesis block for each peer
        peers[i]->createGenesisBlock();
    }
    for (int i = 0 ; i < num_nodes ; i ++ ) {
        double interArrivalTime = getInterArrivalTime();
        scheduleEvent(interArrivalTime, CREATE_TRANSACTION, i, -1, {}); // Schedule the first transaction for each peer
    }
    int random_peer = uniformRandom(0, num_nodes - 1); // Randomly select a peer to start mining
    scheduleEvent(currentTime, MINING_START, random_peer, -1, {}); // Schedule the first mining event
    while ( !eventQueue.empty() ) {
        // Run the simulation until the event queue is empty
        Event current = eventQueue.top();
        eventQueue.pop();
        currentTime = current.time;
        handleEvent(current);
    }
}

void Simulator::handleEvent( Event& event ) {
    // This function handles the event based on the event type
    double newTime;
    switch (event.type) {
        case MINING_START:
            peers[event.sourcePeer]->mining_start();
            newTime = currentTime + peers[event.sourcePeer]->getBlockInterArrivalTime();
            scheduleEvent(newTime, MINING_END, event.sourcePeer, -1, event.data); // Schedule the mining end event 
            break;
        case MINING_END:
            peers[event.sourcePeer]->mining_end();
            break;
        case CREATE_TRANSACTION:
            peers[event.sourcePeer]->generateTransaction();
            newTime = currentTime + getInterArrivalTime();
            scheduleEvent(newTime, CREATE_TRANSACTION, event.sourcePeer, -1, event.data); // Schedule the next transaction
            break;
        case TRANSACTION_SEND:
            newTime = currentTime + calculateLatency(peers[event.sourcePeer]->isSlow, peers[event.targetPeer]->isSlow, get<Transaction>(event.data).getSize());
            scheduleEvent(newTime, TRANSACTION_RECEIVE, event.sourcePeer, event.targetPeer, event.data); // Schedule the transaction receive event
            break;
        case TRANSACTION_RECEIVE:
            peers[event.targetPeer]->receiveTransaction(std::get<Transaction>(event.data), event.sourcePeer); // Receive the transaction
            break;
        case BLOCK_SEND:
            newTime = currentTime + calculateLatency(peers[event.sourcePeer]->isSlow, peers[event.targetPeer]->isSlow, get<Block>(event.data).getBlocksize());
            scheduleEvent(newTime, BLOCK_RECEIVE, event.sourcePeer, event.targetPeer, event.data);
            break;
        case BLOCK_RECEIVE:
            peers[event.targetPeer]->receiveBlock(std::get<Block>(event.data), event.sourcePeer);
            break;
    }
}

void Simulator::scheduleEvent(double time, EventType type, int sourcePeer, int targetPeer, EventData data) {
    Event newEvent = Event(time, type, sourcePeer, targetPeer, data);
    // Push the event to the event queue
    eventQueue.push(newEvent);
}

double Simulator::getInterArrivalTime() { 
    return exponentialRandom(meanTime); 
}
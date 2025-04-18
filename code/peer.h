// The header file for the peer class. The peer class is the main class that represents a node in the network.
#ifndef PEER_H
#define PEER_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "transaction.h"
#include "block.h"
#include "blockchain.h"
#include "simulator.h"
#include "helper.h"
#include <iostream>
#include <set>
using namespace std;

// Referring from main.cpp
extern const int maxTransactionsPerBlock;
extern const double initial_balance;
extern double averageBlockArrivalTime;
extern int num_nodes;
extern vector<Peer*> peers;

// Forward declaration of the Simulator and Blockchain classes
class Simulator;
class Blockchain;
class Peer {
public:
    Peer(Simulator* simulator, int id, bool isSlow, bool isLowCPU); 
    ~Peer();
    void generateTransaction(); 
    void receiveTransaction(Transaction txn, int sender_id);
    void receiveBlock(Block block, int sender_id);     
    void sendTransaction(Transaction txn, int targetPeerID); 
    double interArrivalTime;       
    void setHashingPower();
    void sendBlock(Block block, int targetPeerID);
    int id;                       
    unordered_map<int, Peer*> neighbours; 
    bool isSlow;
    bool isLowCPU;                  
    double getBlockInterArrivalTime();
    void createGenesisBlock();
    void mining_start();
    void mining_end();
    Blockchain* blockchain;        
    multiset<Transaction> txPool; 
private:
    Simulator* simulator;          
    double balance;                
    unordered_set<int> txIDs; 
    Block current_mined_block;
    int leaf_node = -1;              
    double hashingPower;           
};

#endif 
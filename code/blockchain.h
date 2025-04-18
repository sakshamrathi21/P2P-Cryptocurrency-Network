/* This file contains the blockchain class*/
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <iostream>
#include "block.h"
#include "transaction.h"
#include "helper.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <fstream>
using namespace std;

// Referring variables from main.cpp
extern const double initial_balance;
extern int num_nodes;
extern const double minerReward;

class Blockchain {
    public:
        Blockchain(int owner_id) { this->owner_id = owner_id; }
        int owner_id;
        unordered_map<int, Block> blocks;
        unordered_map<int, int> parent_block_id;
        unordered_map<int, vector<int>> children_block_ids;
        bool insertBlock(Block block, double timestamp);
        bool validateBlock(Block block);
        unordered_set<int> leafBlocks;
        int current_leaf_node = -1;
        int returnLeafNode();
        double getPeerBalance(int peerID);
        int getLongestChainHeight ();
        unordered_map<int, double> block_to_timestamp;
        multiset<int> children_without_parent;
        void saveBlockChain(string filename);
        vector<Block> currentChain(); 
};

#endif 

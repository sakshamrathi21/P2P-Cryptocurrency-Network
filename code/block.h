/* This file contains the block class */
#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include "transaction.h"
using namespace std;

const int EmptyBlockSize = 8000; // 8KB

class Block {
public:
    Block() = default;
    Block(int id);
    void addTransaction(Transaction& txn);
    int id;
    int parentID;
    vector<Transaction> transactions;
    int minerID; 
    int height;           
    int getBlocksize();
};

#endif
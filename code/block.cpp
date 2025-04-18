#include "block.h"

Block::Block(int id)
    : id(id) {
        // Declares the block id
        // Constructor for the block class
    }

void Block::addTransaction(Transaction& txn) {
    // Adds a transaction to the block
    transactions.push_back(txn);
}

int Block::getBlocksize() {
    // Returns the size of the block
    int size = 0;
    size += EmptyBlockSize; // Empty block has a size of 1KB
    for (Transaction& txn : transactions) {
        size += txn.getSize();
    }
    return size;
}

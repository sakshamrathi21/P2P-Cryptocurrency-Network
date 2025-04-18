#include "peer.h"

Peer::Peer(Simulator* simulator, int id, bool isSlow, bool isLowCPU) : simulator(simulator), id(id), isSlow(isSlow), isLowCPU(isLowCPU) {
    // Default constructor
    balance = initial_balance; 
    blockchain = new Blockchain(id); // Create a new blockchain for this peer
}

void Peer::generateTransaction() {
    // This function generates a transaction
    double our_balance = blockchain->getPeerBalance(id);
    if (our_balance <= 0) return; // If we don't have any balance, we can't generate a transaction    
    int targetPeerID = id;
    while (targetPeerID == id) {
        targetPeerID = uniformRandom(0, num_nodes - 1); // Choose a random peer to send the transaction to
    }
    Transaction txn = Transaction(id, targetPeerID, uniformRandom(1, our_balance)); // Create a new transaction with a random amount
    txPool.insert(txn); // Insert the transaction into the transaction pool
    for (auto& [id, peer] : neighbours) {
        // Send the transaction to all neighbours
        sendTransaction(txn, peer->id);
    }
}

void Peer::receiveTransaction(Transaction txn, int sender_id) {
    // This function is called when a peer receives a transaction
    if (txIDs.count(txn.getID()) || txn.amount <= 0) return; // If the transaction has already been received, ignore it
    txIDs.insert(txn.getID());
    txPool.insert(txn);  // Insert the transaction into the transaction pool
    for (auto& [id, peer] : neighbours) {
        // Send the transaction to all neighbours
        if (peer->id != sender_id) {
            sendTransaction(txn, peer->id);
        }
    }
}

void Peer::sendTransaction(Transaction txn, int targetPeerID) {
    // This function is called when a peer sends a transaction
    simulator->scheduleEvent(simulator->getCurrentTime(), TRANSACTION_SEND, id, targetPeerID, txn);
}

void Peer::receiveBlock(Block block, int sender_id) {
    // This function is called when a peer receives a block 
    if(blockchain->blocks.count(block.id)) return; // If the block has already been received, ignore it
    bool whether_valid = blockchain->insertBlock(block, simulator->getCurrentTime()); // Insert the block into the blockchain
    if (!whether_valid) return; // If the block is invalid, ignore it
    for (auto& [id, peer] : neighbours) {
        if (peer->id != sender_id) {
            // Send the block to all neighbours except the sender
            sendBlock(block, peer->id);
        }
    }
    logToFile("RECEIVED BLOCK", "Peer " + to_string(id) + " received block " + to_string(block.id) + " (with parent id " + to_string(block.parentID) + ")" + " from peer " + to_string(sender_id) + " at time " + to_string(simulator->getCurrentTime())); // Log the event to the log file
    simulator->scheduleEvent(simulator->getCurrentTime(), MINING_START, id, -1, {});
}

void Peer::sendBlock(Block block, int targetPeerID) {
    // This function is called when a peer sends a block
    simulator->scheduleEvent(simulator->getCurrentTime(), BLOCK_SEND, id, targetPeerID, block);
}

void Peer::setHashingPower() {
    // This function sets the hashing power of the peer, based on the number of peers and whether the peer is low CPU
    int sum = 0;
    for (int i = 0 ; i < peers.size() ; i ++ ) {
        peers[i]->isLowCPU ? sum++ : sum += 10;
    }
    hashingPower = isLowCPU ? 1 : 10;
    hashingPower /= sum;
}


Peer::~Peer() {
    // Default destructor
    // cout << " checking " << endl;
    delete blockchain;
}

void Peer::createGenesisBlock() {
    // This function creates the genesis block and inserts it into the blockchain
    Block genesisBlock = Block(-1);
    genesisBlock.parentID = -1;
    genesisBlock.minerID = -1;
    genesisBlock.height = 0;
    blockchain->insertBlock(genesisBlock, simulator->getCurrentTime());
}


void Peer::mining_start() {
    // This function is called when a peer starts mining
    current_mined_block = Block(generateBlockID());
    int count = 0;
    vector<double> peerBalancesRightNow(num_nodes); // Get the current balances of all peers
    for (int i = 0 ; i < num_nodes ; i ++ ) peerBalancesRightNow[i] = blockchain->getPeerBalance(i);
    for (Transaction txn : txPool) {
        if (peerBalancesRightNow[txn.sender] < txn.amount) {
            // If the sender doesn't have enough balance, ignore the transaction
            continue;
        }
        peerBalancesRightNow[txn.sender] -= txn.amount;
        current_mined_block.addTransaction(txn);
        count++; 
        if(count >= maxTransactionsPerBlock) {break;}  // If the block is full, stop adding transactions
    }
    current_mined_block.parentID = blockchain->current_leaf_node; // Set the parent ID of the block
    current_mined_block.minerID = id; // Set the miner ID of the block
    current_mined_block.height = blockchain->getLongestChainHeight(); // Set the height of the block
    leaf_node = blockchain->current_leaf_node; // Set the leaf node of the block
}

void Peer::mining_end() {
    // This function is called when a peer finishes mining and we need to insert the block into the blockchain
    if (blockchain->current_leaf_node != leaf_node) return;  // If the leaf node has changed, ignore the block, and we need to start mining again
    blockchain->insertBlock(current_mined_block, simulator->getCurrentTime());
    for (auto& [id, peer] : neighbours) {
        sendBlock(current_mined_block, peer->id);
    }   
}

double Peer::getBlockInterArrivalTime() {
    // This function returns the block inter-arrival time
    return exponentialRandom( averageBlockArrivalTime / hashingPower );
}
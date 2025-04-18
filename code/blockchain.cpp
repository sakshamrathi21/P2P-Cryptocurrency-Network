#include "blockchain.h"

bool Blockchain::validateBlock(Block block) {
    // This function checks if the block is valid or not
    if (blocks.find(block.parentID) == blocks.end()) {
        // If the parent block is not present in the blockchain
        return false;
    }
    unordered_map<int, double> all_peer_balances; // calculating the peer balances from the blocks of the blockchain
    for (int i = 0 ; i < num_nodes ; i ++ ) {
        all_peer_balances[i] = initial_balance;
    }
    Block current_block = block;
    while (true) {
        for (Transaction txn : current_block.transactions) {
            all_peer_balances[txn.sender] -= txn.amount;
            all_peer_balances[txn.receiver] += txn.amount;
        }
        all_peer_balances[current_block.minerID] += minerReward; // mining fee
        if (current_block.parentID == -1) break; // genesis block reached, we can break
        current_block = blocks[current_block.parentID];
    }
    for (int i = 0 ; i < num_nodes ; i ++ ) {
        if (all_peer_balances[i] < 0) {
            // If the balance of any peer is negative, then the block is invalid
            return false;
        }
    }
    return true;
}

double Blockchain::getPeerBalance(int peerID) {
    // This function returns the balance of the peer
    double balance = initial_balance;
    Block current_block = blocks[returnLeafNode()]; // get the leaf node
    while (true) {
        for (Transaction txn : current_block.transactions) {
            if (txn.sender == peerID) balance -= txn.amount;
            if (txn.receiver == peerID) balance += txn.amount;

        }
        if (current_block.minerID == peerID) balance += minerReward; // mining fee
        if (current_block.parentID == -1) break; // reached the genesis block
        current_block = blocks[current_block.parentID];
    }
    return balance;
}

bool Blockchain::insertBlock(Block block, double timestamp) {
    // This function inserts the block in the blockchain
    if (blocks.find(block.id) == blocks.end()) block_to_timestamp[block.id] = timestamp; // Storing the timestamps for printing purposes (if the block comes back again, then no need to update the timestamp)
    blocks[block.id] = block; // Map from id to block object
    if (blocks.find(block.parentID) == blocks.end()) {
        // Child came before the parent, so for now we just return false, and add it when the parent reaches
        children_without_parent.insert(block.id);
        return false;
    }
    if (block.id != -1 && !validateBlock(block)) return false; // If it is not the genesis block and the block is invalid, then return false
    if (block.id != -1) {
        // map between parent and children
        parent_block_id[block.id] = block.parentID;
        children_block_ids[block.parentID].push_back(block.id);
    }
    if (current_leaf_node == block.parentID) current_leaf_node = block.id; // if the parent was previous leaf node, then update the leaf node
    if (leafBlocks.find(block.parentID) != leafBlocks.end()) leafBlocks.erase(block.parentID); // parent is no longer a leaf node
    leafBlocks.insert(block.id); // child is the new leaf node
    int max_height = blocks[current_leaf_node].height; // the previous maximum height
    int prev_leaf_node = current_leaf_node;
    for (auto& b : leafBlocks) {
        // Updating the leaf node
        // This is essentially the logic for picking the chain with the maximum height
        if (blocks[b].height > max_height) {
            max_height = blocks[b].height;
            current_leaf_node = b;
        }
    }
    int new_leaf_node = current_leaf_node;
    if (prev_leaf_node != new_leaf_node) {
        // We need to update the transaction pool of the peer, if we have a new leaf node
        int common_ancestor = -1;
        // We need to find the common ancestor of the two leaf nodes
        // For this we keep going up the chain until we reach the same height
        while(blocks[prev_leaf_node].height > blocks[new_leaf_node].height) {
            for (Transaction &t : blocks[prev_leaf_node].transactions) {
                if (peers[owner_id]->txPool.find(t) == peers[owner_id]->txPool.end()) {
                    peers[owner_id]->txPool.insert(t);
                }
            }
            prev_leaf_node = parent_block_id[prev_leaf_node];
        }
        while(blocks[new_leaf_node].height > blocks[prev_leaf_node].height) {
            for (Transaction &t : blocks[new_leaf_node].transactions) {
                if (peers[owner_id]->txPool.find(t) != peers[owner_id]->txPool.end()) {
                    peers[owner_id]->txPool.erase(t);
                }
            }
            new_leaf_node = parent_block_id[new_leaf_node];
        }
        while (prev_leaf_node != new_leaf_node) {
            for (Transaction &t : blocks[prev_leaf_node].transactions) {
                if (peers[owner_id]->txPool.find(t) == peers[owner_id]->txPool.end()) {
                    peers[owner_id]->txPool.insert(t);
                }
            }
            for (Transaction &t : blocks[new_leaf_node].transactions) {
                if (peers[owner_id]->txPool.find(t) != peers[owner_id]->txPool.end()) {
                    peers[owner_id]->txPool.erase(t);
                }
            }
            prev_leaf_node = parent_block_id[prev_leaf_node];
            new_leaf_node = parent_block_id[new_leaf_node];
        }
    }
    if (current_leaf_node == block.id) {
        for (Transaction& t : block.transactions) {
            if (peers[owner_id]->txPool.find(t) != peers[owner_id]->txPool.end()) {
                peers[owner_id]->txPool.erase(t);
            }
        }
    }
    for (int child : children_without_parent) {
        // We need to check if the children of the block can now be added
        if (blocks[child].parentID == block.id) {
            children_without_parent.erase(child); // Parent found
            for (auto p : peers[owner_id]->neighbours) 
                peers[owner_id]->sendBlock(blocks[child], p.second->id); // Send the block to the neighbours
            insertBlock(blocks[child], timestamp); // Insert the block
            return true;
        }
    }
    return true;
}

int Blockchain::returnLeafNode () {
    return current_leaf_node; // Returns the current leaf node
}

int Blockchain::getLongestChainHeight () {
    int leaf_node = current_leaf_node;
    return blocks[leaf_node].height + 1; // Returns the height of the longest chain
}

void Blockchain::saveBlockChain(string filename) {
    // This function saves the blockchain to a file
    ofstream file(filename, ios::trunc);
    file << std::fixed;
    for (const auto& b : blocks) {
        if(b.second.id == -1) continue;
        file << b.second.id << "\t" << b.second.parentID << "\t" << block_to_timestamp[b.second.id] << "\n";
    }
    file.close();
}

vector<Block> Blockchain::currentChain() {
    // This function returns the current chain
    vector<Block> chain;
    int leaf_node = current_leaf_node;
    while (leaf_node != -1) {
        chain.push_back(blocks[leaf_node]);
        leaf_node = parent_block_id[leaf_node];
    }
    reverse(chain.begin(), chain.end());
    return chain;
}
#include "helper.h"

mt19937 gen(random_device{}()); // Mersenne Twister 19937 generator

int current_transaction_id = 0; // Transaction ID counter
int current_block_id = 0; // Block ID counter

int generateBlockID() {
    // Return block ID and increment counter
    return current_block_id++;
}

double exponentialRandom(double mean) {
    // Return random number from exponential distribution
    exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

double uniformRandom(double min, double max) {
    // Return random number from uniform distribution
    uniform_real_distribution<> dist(min, max);
    return dist(gen);
}

double calculateLatency(bool isSlowI, bool isSlowJ, int messageLength) {
    // For latency in seconds
    bool isFastI = !isSlowI;
    bool isFastJ = !isSlowJ;
    double cij = isFastI && isFastJ ? FAST_LINK_SPEED : SLOW_LINK_SPEED;
    double rhoij = uniformRandom(MIN_PROPAGATION_DELAY, MAX_PROPAGATION_DELAY);
    double meanQueuingDelay = MEAN_QUEUING_DELAY_FACTOR / cij;
    double dij = exponentialRandom(meanQueuingDelay);
    double latency = rhoij + ((double)messageLength / cij) * 1e3 + dij * 1e3; // converting to milliseconds
    return latency / 1e3; 
}

int generateTransactionID() {
    // Return transaction ID and increment counter
    return current_transaction_id++;
}

vector<int> getSlowNodeSubset(int numNodes, double slowNodePercentage) {
    // Return subset of slow nodes
    if (numNodes <= 0) {
        throw invalid_argument("Number of nodes must be positive.");
    }
    slowNodePercentage /= 100;
    int numSlowNodes = ceil(numNodes * slowNodePercentage);
    vector<int> indices(numNodes);
    iota(indices.begin(), indices.end(), 0);
    shuffle(indices.begin(), indices.end(), gen); // Shuffling and picking the first numSlowNodes
    return vector<int>(indices.begin(), indices.begin() + numSlowNodes);
}

void clearLogFile(const string& filePath) {
    // Clear log file
    ofstream logFile(filePath, ios::trunc);
    if (!logFile.is_open()) {
        cerr << "[ERROR] Unable to open log file!" << endl;
    }
}

void logToFile(string level, string message, const string& filePath) {
    // Log message to file
    ofstream logFile(filePath, ios::app); // Write to file
    if (logFile.is_open()) {
        logFile << "[" << level << "] " << message << endl;
        logFile.close();
    } else {
        cerr << "[ERROR] Unable to open log file!" << endl;
    }
}

void generateConnectedGraph() {
    // Generate connected graph
    vector<int> peer_ids;
    for (int i = 0 ; i < num_nodes ; i ++ ) peer_ids.push_back(peers[i]->id);
    vector<pair<int, int>> edges;
    int num_peers = peer_ids.size();
    if(num_peers < 2) return; // No edges to add
    vector<unordered_set<int>> adj_list(num_peers);
    vector<int> shuffled_ids = peer_ids;
    shuffle(shuffled_ids.begin(), shuffled_ids.end(), gen);
    for(int i = 1; i < num_peers; i++) {
        // Adding edges between shuffled peers
        int peer1 = shuffled_ids[i - 1],
            peer2 = shuffled_ids[i];
        adj_list[peer1].insert(peer2);
        adj_list[peer2].insert(peer1);
        edges.emplace_back(peer1, peer2);
    }
    for(int i = 0; i < num_peers; i++) {
        // Adding more edges to make the graph connected and ensuring that each peer has at least 3 neighbours and not more than 6
        int curr_peer = peer_ids[i];
        while(adj_list[curr_peer].size() < 3) {
            int rand_peer = peer_ids[rand() % num_peers];
            if(rand_peer != curr_peer && adj_list[curr_peer].find(rand_peer) == adj_list[curr_peer].end() && adj_list[rand_peer].size() < 6) {
                adj_list[curr_peer].insert(rand_peer);
                adj_list[rand_peer].insert(curr_peer);
                edges.emplace_back(curr_peer, rand_peer);
            }
        }
    }
    for (int i = 0 ; i < edges.size() ; i ++ ) {
        // Updating the neighbours of the peers
        int peer1 = edges[i].first;
        int peer2 = edges[i].second;
        peers[peer1]->neighbours[peer2] = peers[peer2];
        peers[peer2]->neighbours[peer1] = peers[peer1];
    }
    for (int i = 0 ; i < peers.size() ; i ++ ) peers[i]->setHashingPower(); // Setting the hashing power of the peers
}

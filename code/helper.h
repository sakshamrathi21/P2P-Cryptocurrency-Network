/* This file contains some of the helpful functions */
#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>
#include <unordered_set>
#include "peer.h"
using namespace std;

extern int num_nodes; // referring from main.cpp

extern int current_transaction_id;  
extern std::mt19937 gen;      
extern int current_block_id;  

class Peer;  // Forward declaration of the class Peer
extern vector<Peer*> peers;

// Some of the important constants to calculate the latency
constexpr double FAST_LINK_SPEED = 100e6; // in bits per second
constexpr double SLOW_LINK_SPEED = 5e6; // in bits per second
constexpr double MIN_PROPAGATION_DELAY = 10; // in milliseconds
constexpr double MAX_PROPAGATION_DELAY = 500; // in milliseconds
constexpr double MEAN_QUEUING_DELAY_FACTOR = 96e3; // in bits

int generateTransactionID();
int generateBlockID();
vector<int> getSlowNodeSubset(int numNodes, double slowNodePercentage);
double uniformRandom(double min, double max);
double exponentialRandom(double mean);
double calculateLatency(bool isFastI, bool isFastJ, int messageLength);
void logToFile(string level, string message, const string& filePath = "log.txt");
void clearLogFile(const string& filePath = "log.txt");
void generateConnectedGraph();

#endif
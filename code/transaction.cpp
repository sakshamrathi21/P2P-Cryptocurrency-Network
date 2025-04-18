#include "transaction.h"
#include "helper.h"
#include <sstream>

Transaction::Transaction(int sender, int receiver, double amount)
    : sender(sender), receiver(receiver), amount(amount) {
        // Constructor
    id = generateTransactionID();
}

int Transaction::getID() const {
    return id;
}

int Transaction::getSize() {
    // Each transaction has a size of 1KB
    return TransactionSize;
}

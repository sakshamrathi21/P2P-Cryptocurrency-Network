# Simulation of a P2P Cryptocurrency Network
This project has been done as part of the course CS765 - Introduction to Blockchain, Cryptocurrency and Smart Contracts. Here are the team members:
- Saksham Rathi (22B1003)
- Kavya Gupta (22B1053)
- Mayank Kumar (22B0933)

Here are list of files/directories present in this repository:

```
22B1003_22B1053_22B0933
|-- README.md
|-- code
    |-- Makefile
    |-- cpp files
    |-- header files
    |-- blockchain_data
    |-- blockchain_graphs
    |-- python scripts
|-- documentation
    |-- design-doc.pdf
    |-- report.pdf
|-- images
    |-- flowchart
    |-- class-structure
    |-- plots folder
```

## Instructions on compiling and running the code
```
cd code
make clean
make
```
This will produce an executable named "run".

Here is the list of command line arguments which the executable expects:
```
./run <number-of-nodes> <percentage-of-slow-nodes> <percentage-of-low-cpu-nodes> <mean-Time-for-transactions> <average-Block-Arrival-Time> <time-of-execution> <flags> 
```

Here is an example usage:
```
./run 100 50 50 10 600 10
```

It is worth noting that the time of execution is measured in seconds, whereas the other inter-arrival times are in terms of the discrete event simulator. 

### For making blockchains
Here is an example usage:
```
./run 100 50 50 10 600 10 --blockchain
python3 plot_blockchain.py
```
The plots will be generated in the blockchain_graphs folder.

### For calculating ratios
Here is an example usage:
```
./run 100 50 50 10 600 10 --ratio
```

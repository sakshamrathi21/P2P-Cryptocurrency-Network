run: *.cpp *.h
	g++ -w -std=c++23 -O2 block.cpp blockchain.cpp transaction.cpp peer.cpp simulator.cpp helper.cpp main.cpp -o run

.PHONY: clean
clean:
	rm -f run log.txt
	rm -rf blockchain_data blockchain_graphs
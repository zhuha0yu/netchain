switch: clean
	g++ -g hostsrc/*.cpp -o send -std=c++11 -lpthread
	
	sudo p4run


controller:
	sudo python NetChain-controller.py
host:
	g++ -g hostsrc/*.cpp -o send -std=c++11 -lpthread
clean:
	sudo rm -rf log p4src/*.p4i *.pyc *.db pcap *.csv
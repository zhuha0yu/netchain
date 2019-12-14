.PHONY:NetChain
switch:
	g++ -g hostsrc/*.cpp -o send -std=c++11 
	
	sudo p4run


controller:
	sudo python NetChain-controller.py
host:
	g++ -g hostsrc/*.cpp -o send -std=c++11 
clean:
	rm -rf log p4src/*.p4i *.pyc *.db pcap *.csv
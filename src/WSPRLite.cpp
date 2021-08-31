#include <iostream>

#include "wsprlite/message.h"
#include "wsprlite/encoder.h"
#include "wsprlite/interleaver.h"
#include "wsprlite/sync.h"


using namespace wsprlite;
using namespace std;

int main(int argc, char **argv){
	if(argc < 4){
		cout << "Usage:" << endl << argv[0] << " callsign locator power" << endl;
		return 1;
	}

	wsprlite::SymbolStream encoder;
	encoder << CallSign(argv[1]) << Locator(argv[2]) << PowerLevel(atoi(argv[3]));
	auto symbols = encoder.encode();
	
	cout << "Channel Symbols: " << endl << "\t";
	int line_count = 0;
	for(auto symbol : symbols){
		cout << symbol << " ";
		if(++line_count == 30){
			line_count = 0;
			cout << endl << "\t";
		}
	}
	cout << endl;
}
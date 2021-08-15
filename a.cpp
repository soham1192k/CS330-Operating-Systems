#include<iostream>
#include "testlib.h"
using namespace std;
/********************* Custom Inputs ***************************/
int n_low=1;
int n_high=1e5;
int h_low=1;
int h_high=1e9;
/********************* Custom Inputs ***************************/
#define endl '\n'
void generate();
int main(int argc, char* argv[]){
	registerGen(argc, argv, 1);
	string type = argc > 1 ? argv[1] : "unexpected";
	
	if(type == "small"){
		n_high=10;
		h_high=30;
	}

	if(type == "extreme"){
		n_low=n_high;
		h_low=1e8;
	}

	generate();
	return 0;
}
void generate(){
	int n = rnd.next(n_low, n_high);
	cout << n << endl;
	vector<int>a;
	for(int i = 0; i < n; i++){
		a.push_back(rnd.next(h_low,h_high));
	}
    for(auto &ele : a) 
	cout << ele << endl;
}

#include<iostream>
#include "testlib.h"
using namespace std;
/********************* Custom Inputs ***************************/
int n_low=1;
int n_high=1e5;
long long cars_low=1;
long long cars_high=1e14;
int k_high=1e5;
/********************* Custom Inputs ***************************/
#define endl '\n'
void generate();
int main(int argc, char* argv[]){
    registerGen(argc, argv, 1);
    string type = argc > 1 ? argv[1] : "unexpected";
	
    if(type == "small"){
    	n_high = 10;
    	cars_high=20;
    }

    if(type == "custom"){
        n_low = n_high;
        cars_low = 1e10;
        k_high = 5;
    }

    if(type == "extreme"){
	n_low = n_high;
    }
	
    generate();
    return 0;
}
void generate(){
    int n = rnd.next(n_low, n_high);
    cout << n << endl;
    set<long long>s;
    while((int)s.size() != n){
        s.insert(rnd.next(cars_low,cars_high));
    }
    for(auto &ele : s) 
    cout << ele << endl;
    int k = rnd.next(1,min(k_high,n));
    cout << k << endl;
}

#include<iostream>
#include "testlib.h"
using namespace std;
/********************* Custom Inputs ***************************/
int n_low=1;
int n_high=10;
int s_low=1;
int s_high=2500;
/********************* Custom Inputs ***************************/
#define endl '\n'
void generate();
int main(int argc, char* argv[]){
    registerGen(argc, argv, 1);
    string type = argc > 1 ? argv[1] : "unexpected";

    if(type == "small"){
      n_high = 1;
          s_high = 10;
    }

    if(type == "extreme"){
      n_low = n_high;
      s_low = s_high;
    }

    generate();
    return 0;
}
void generate(){
    int n = rnd.next(n_low, n_high);
    cout << n << endl;
    vector<char>hold = {'L','R','G'};
    for(int i = 1; i <= n; i++){
      int len = rnd.next(s_low,s_high);
      string now = "";
      for(int j = 1; j <= len; j++){
          int val = rnd.next(1,3);
          now += hold[val-1];
      }
      cout << now << endl;
    }
}

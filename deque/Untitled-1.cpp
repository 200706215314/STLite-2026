#include<bits/stdc++.h>

using namespace std;

int main(){

    deque <int> dq;
    dq.push_back(1);
    deque<int>::iterator it = dq.begin();   
    //   cout << *it;
    it--;
    cout << (it == dq.end());
}
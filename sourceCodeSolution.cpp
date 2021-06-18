// GAURAV GULATI
// gauravgulati6408@gmail.com
// C++ CODE
#include<bits/stdc++.h>

using namespace std;

#define ll long long int

unordered_map<string,vector<string>> txp; // to store key(txids) value(array of parent_txids) pairs in map
unordered_map<string,pair<ll,ll>> txwf; // to store (txids,(fee,weight)) pairs
set<string> visited; // to keep track of txids which we already have considered
vector<pair<string,pair<ll,ll>>> vect; // to store txwf map in descending order of the fee/weight ratio.

ll W = 4000000; // maximum block weight

// function to get different parent txids from the file
vector<string> getParent(string s){
    vector<string> Parents;
    int start = 0, end=s.find(";");
    while(end != -1){
        Parents.push_back(s.substr(start,end - start));
        start = end + 1;
        end = s.find(";", start);
    }
    Parents.push_back(s.substr(start,end - start));

    return Parents;
}

// function to convert string to interger values of weight and fee.
ll stringToInt(string s){
    ll x;
    stringstream geek(s);
    geek >> x;
    return x;
}

// function comp which helps in sorting txwf map in descending order of fee/weight ratio.
bool comp(pair<string,pair<ll,ll>>&a, pair<string,pair<ll,ll>>&b){
    return a.second.first/a.second.second > b.second.first/b.second.second;
}

int main()
{
    ifstream ip("mempool.csv");

    string txid, fee, Weight, parent, first;

    getline(ip,first,'\n');
    ll i=0;
    while(ip.good()){

        txid.clear();
        fee.clear();
        Weight.clear();
        parent.clear();

        getline(ip,txid,',');
        getline(ip,fee,',');
        getline(ip,Weight,',');
        getline(ip,parent,'\n');

        // adding txid , fee, weight in a single map.
        if(!fee.empty())
            {
                txwf.insert(make_pair(txid,make_pair(stringToInt(fee),stringToInt(Weight))));
            }

        // adding txid and parents array (if exist)
        vector<string> Parents;
        if(parent.empty()&&!fee.empty()){
            txp.insert(make_pair(txid,Parents));
        }
        else{
            Parents = getParent(parent);
            txp.insert(make_pair(txid,Parents));
        }

    }
    txp.erase("");
    ip.close();

    // to sort txwf map on basis of fee/weight ratio in descending order.
    for(auto i : txwf) vect.push_back(i);
    sort(vect.begin(),vect.end(),comp);

    // main solution to get desirable txids and print them in block.txt file
    // the approach implemented is that start from that txid which has maximum fee/weight ratio
    // and then check its parents whether they has any parent or not.
    // We will track the order using eleQ & ans side by side so that
    // parent transaction can come before the its children transaction.
    ofstream f;
    f.open("block.txt",ios::out);

    ll total_weight = 0;
    ll total_fee = 0;
    
    for(auto i : vect){
        if(visited.find(i.first)==visited.end()){
            ll local_weight = 0;
            ll local_fee = 0;
            set<string> eleQ;
            vector<string> ans;
            queue<string> q;
            q.push(i.first);
            eleQ.insert(i.first);
            auto fw = txwf.find(i.first);
            local_fee += fw->second.first;
            local_weight += fw->second.second;
            ans.push_back(i.first);
            while(!q.empty()){
                string k = q.front();
                q.pop();
                if(visited.find(k)==visited.end()){
                    visited.insert(k);
                    auto itr = txp.find(k);
                    for(auto op : itr->second){
                        if(eleQ.find(op)==eleQ.end()&&visited.find(op)==visited.end()) {
                            eleQ.insert(op);
                            ans.push_back(op);
                            auto FW = txwf.find(op);
                            local_fee += FW->second.first;
                            local_weight += FW->second.second;
                            q.push(op);
                        }
                    }
                }
            }
            if((local_weight + total_weight) < W && (local_fee + total_fee) > total_fee){
                total_weight += local_weight;
                total_fee += local_fee;
                for (auto it = ans.rbegin(); it != ans.rend(); ++it){
                    f << *it << endl;
                }
            }
        }
    }
    
    cout<<"Total Weight: "<<total_weight<<"\nTotal Fee: "<<total_fee;

    f.close();
    return 0;
}
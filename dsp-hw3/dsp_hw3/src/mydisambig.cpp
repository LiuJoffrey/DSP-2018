#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include "Ngram.h"
#include "File.h"
#include "Vocab.h"

using namespace std;

void ReadMappingFile(char* map_file, map<string, set<string>> &mapping){
    //cout<<"In read map function";
    string str, zh, word;

    ifstream in(map_file);
    while(getline(in, str)){
        int n = str.length();
        zh = str.substr(0,2);
        //cout<<zh<<": ";
        for(int i=2; i<n; i++){
            if (str.at(i)<0){
                //cout<<str.substr(i,2);
                word = str.substr(i,2);
                mapping[zh].insert(word);
                i++;
            }   
        }        
    }
}

void print_map(map<string, set<string>> &mapping){
    for(auto it=mapping.cbegin(); it != mapping.cend(); ++it){
        cout<< it->first<<": ";
        for(auto its=it->second.begin(); its!=it->second.end(); ++its)
            cout<<" "<<*its;
        cout<<endl<<"================"<<endl;
    }
}

void find_all_nodes(vector<string> in_line, map<string, set<string>> mapping, vector<set<string>> &all_nodes){
    set<string> tmp_set;
    map<string, set<string>>::iterator iter;

    for(int i=0; i<in_line.size(); i++){
        tmp_set.clear();
        iter = mapping.find(in_line.at(i));
        if(iter != mapping.end()){
            tmp_set = iter->second;
        }else{
            tmp_set.insert(in_line.at(i));
        }
        //for(auto its=tmp_set.begin(); its!=tmp_set.end(); ++its)
        //    cout<<" "<< *its;
        //cout<<endl;
        all_nodes.push_back(tmp_set);
    }
}

void print_vec_set(vector<set<string>> &all_nodes){
    cout<<all_nodes.size()<<endl;
    for(int i=0; i<all_nodes.size(); i++){
        for(auto its=all_nodes.at(i).begin(); its!=all_nodes.at(i).end(); ++its){
            cout<<" "<<*its;
        }
        cout<<endl<<"===================="<<endl;
    }
}

double get_bigram_prob(const char* w1, const char* w2, Ngram &lm, Vocab &vocab){
    VocabIndex w1_idx = vocab.getIndex(w1);
    VocabIndex w2_idx = vocab.getIndex(w2);

    if(w1_idx == Vocab_None){
        w1_idx = vocab.getIndex(Vocab_Unknown);
    }
    if(w2_idx == Vocab_None){
        w2_idx = vocab.getIndex(Vocab_Unknown);
    }

    VocabIndex context[] = {w1_idx, Vocab_None};
    return lm.wordProb(w2_idx, context);

}

pair<vector<string>, double> find_best_bigram(vector<pair<vector<string>, double>> path_prob, string cur_word, Ngram &lm, Vocab &vocab){
    vector<string>::iterator iter;
    int max_idx = 0;
    double prob, max_prob=-10000;

    for(int i=0; i<path_prob.size(); i++){
        iter = path_prob.at(i).first.end();
        iter -= 1; // get final word of the path
        string pre_word = *iter;
        double pre_prob = path_prob.at(i).second;
        double cur_prob = get_bigram_prob(pre_word.c_str(), cur_word.c_str(), lm, vocab);
        if((pre_prob+cur_prob)>max_prob){
            max_prob = pre_prob + cur_prob;
            max_idx = i;
        }

        //cout<<"cur_prob: "<< cur_prob<<endl;
        //exit(1);

    }
    pair<vector<string>, double> best_path = path_prob.at(max_idx);
    best_path.first.push_back(cur_word);
    best_path.second = max_prob;
    return best_path;

}

void find_best_line(vector<set<string>> all_nodes, Ngram &lm, Vocab &vocab, vector<string> &max_prob_line){
    vector<pair<vector<string>, double>> path_prob;
    set<string>::iterator iter = all_nodes.begin()->begin();
    vector<string> start;
    
    for(iter; iter != all_nodes.begin()->end(); iter++){
        start.clear();
        start.push_back(*iter);
        path_prob.push_back(make_pair(start, 0.0)); // 0.0 should be change for 1-gram probility
    }

    for(int i=1; i<all_nodes.size(); i++){
        vector<pair<vector<string>, double>> next_path_prob;
        next_path_prob.clear();
        for(iter=all_nodes.at(i).begin(); iter!=all_nodes.at(i).end(); iter++){
            pair<vector<string>, double> cur_best = find_best_bigram(path_prob, *iter, lm, vocab);
            next_path_prob.push_back(cur_best);
        }
        path_prob.clear();
        path_prob = next_path_prob;
    }
    
    double max_prob = -10000.0;
    int max_prob_idx = -1;
    for(int i=0; i<path_prob.size(); i++){
        if(path_prob.at(i).second > max_prob){
            max_prob = path_prob.at(i).second;
            max_prob_idx = i;
        } 
    }
    
    max_prob_line.assign(path_prob.at(max_prob_idx).first.begin(), path_prob.at(max_prob_idx).first.end());
}

int main(int argc, char* argv[]){
    
    char* seg_file;
    //char* mapping;
    //char* lm_model;
    //char* order;

    Vocab vocab;
    Ngram lm(vocab, 2);
    map<string, set<string>> mapping;
    int lm_order;

    //cout<<"# arg: "<< argc<<endl;
    for(int i=1; i<argc; i++){
        if(argv[i] == string("-lm")){
            //cout<<"-lm "<<argv[i+1]<<endl;
            File frLm(argv[i+1], "r");
            lm.read(frLm);
            frLm.close();
        }
        if(argv[i] == string("-text")){
            seg_file = argv[i+1];
            //cout<<"-text "<<argv[i+1]<<endl;
        }
        if(argv[i] == string("-map")){
            //cout<<"-map "<<argv[i+1]<<endl;
            ReadMappingFile(argv[i+1], mapping);
            //print_map(mapping);

        }
        if(argv[i] == string("-order")){
            //cout<<"-order "<<argv[i+1]<<endl;
            lm_order = atoi(argv[i+1]);
            //cout<<"lm_order: "<<lm_order<<endl;
        }
    }
    /////////// For read input seg line /////////////// 
    string str;
    string tmp;
    vector<string> split_line;
    vector<set<string>> all_nodes;
    vector<string> max_prob_line;
    
    ifstream in(seg_file);
    while(getline(in, str)){
        split_line.clear();
        all_nodes.clear();
        max_prob_line.clear();
        int n = str.length();
        for(int i=0; i<n; i++){
            if (str.at(i)<0){
                tmp = str.substr(i,2);
                split_line.push_back(tmp);
                //cout<<str.substr(i,2);
                i++;
            }
        }
        //for(int i=0; i<split_line.size();i++){
        //    cout<<split_line.at(i);
        //}
        //cout<<split_line.size()<<endl;

        find_all_nodes(split_line, mapping, all_nodes);
        //print_vec_set(all_nodes);
        
        find_best_line(all_nodes, lm, vocab, max_prob_line);
        
        cout<<"<s>";
        for(int i=0; i<max_prob_line.size(); i++){
            cout<<" "<< max_prob_line.at(i);
        }
        cout<<"</s>"<<endl;
        
        
    }

    return 0;
}


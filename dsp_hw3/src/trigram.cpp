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
    /*--------------------------*/
    tmp_set.clear();
    tmp_set.insert("<s>");
    all_nodes.push_back(tmp_set);
    
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
    /*--------------------------*/
    tmp_set.clear();
    tmp_set.insert("</s>");
    all_nodes.push_back(tmp_set);
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

double get_trigram_prob(const char* w1, const char* w2, const char* w3, Ngram &lm, Vocab &vocab){
    VocabIndex w1_idx = vocab.getIndex(w1);
    VocabIndex w2_idx = vocab.getIndex(w2);
    VocabIndex w3_idx = vocab.getIndex(w3);

    if(w1_idx == Vocab_None){
        w1_idx = vocab.getIndex(Vocab_Unknown);
    }
    if(w2_idx == Vocab_None){
        w2_idx = vocab.getIndex(Vocab_Unknown);
    }
    if(w3_idx == Vocab_None){
        w3_idx = vocab.getIndex(Vocab_Unknown);
    }

    VocabIndex context[] = {w2_idx, w1_idx, Vocab_None};
    return lm.wordProb(w3_idx, context);
}

pair<vector<string>, double> find_best_bigram(vector<pair<vector<string>, double>> path_prob, string cur_word, Ngram &lm, Vocab &vocab){
    vector<string>::iterator iter;
    int max_idx = 0;
    double prob, max_prob=-10000;
    const LogP LogP_PseudoZero = -100;

    for(int i=0; i<path_prob.size(); i++){
        iter = path_prob.at(i).first.end();
        iter -= 1; // get final word of the path
        string pre_word = *iter;
        double pre_prob = path_prob.at(i).second;
        double cur_prob = get_bigram_prob(pre_word.c_str(), cur_word.c_str(), lm, vocab);
        if(cur_prob == LogP_Zero){
            //cout<<"Zero Prob"<<endl;
            cur_prob = LogP_PseudoZero;
        }
        //cout<<pre_word<<" "<<cur_word<<cur_prob<<endl;
        //exit(1);
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

    /*for(int i=0; i<best_path.first.size(); i++){
        cout<<best_path.first.at(i)<<" ";
    }
    cout<<best_path.second<<endl;*/
    return best_path;

}

pair<vector<string>, double> find_best_trigram(vector<pair<vector<string>, double>> path_prob, string cur_word,  Ngram &lm, Vocab &vocab){
    vector<string>::iterator iter;
    int max_idx = 0;
    double prob, max_prob=-10000;
    double cur_prob;
    const LogP LogP_PseudoZero = -100;
    //vector<double> cur_tri_prob_vec;
    //vector<double> pre_bi_prob_vec;
    //vector<double> cur_bi_prob_vec;

    for(int i=0; i<path_prob.size(); i++){
        iter = path_prob.at(i).first.end();
        iter -= 1; // get final word of the path
        string pre_word = *iter;
        iter -= 1;
        string pre_2_word = *iter;
        double pre_prob = path_prob.at(i).second;
        double cur_tri_prob = get_trigram_prob(pre_2_word.c_str(), pre_word.c_str(), cur_word.c_str(), lm, vocab);
        //double cur_bi_prob = get_bigram_prob(pre_word.c_str(), cur_word.c_str(), lm, vocab);
        //double pre_bi_prob = get_bigram_prob(pre_2_word.c_str(), pre_word.c_str(), lm, vocab);
        
        if(cur_tri_prob == LogP_Zero){
            //cout<<"LogP_Zero"<<endl;
            cur_tri_prob = LogP_PseudoZero;
        }
        /*if(cur_bi_prob == LogP_Zero){
            //cout<<"LogP_Zero"<<endl;
            cur_bi_prob = LogP_PseudoZero;
        }*/
        //if(pre_bi_prob == LogP_Zero){
            //cout<<"LogP_Zero"<<endl;
        //    pre_bi_prob = LogP_PseudoZero;
        //}
        //cur_tri_prob_vec.push_back(cur_tri_prob);
        //pre_bi_prob_vec.push_back(pre_bi_prob);
        //cur_bi_prob_vec.push_back(cur_bi_prob);
        /*if(cur_bi_prob<-3.0){
            cur_bi_prob = 0;
        }
        if(pre_bi_prob<-3.0){
            pre_bi_prob = 0;
        }*/
        /*if(cur_tri_prob>-3){
            cur_prob = cur_tri_prob;
        }else{
            cur_prob = cur_bi_prob;
        }*/
        cur_prob = cur_tri_prob *1.0;// + cur_bi_prob *0.0;// + pre_bi_prob *0.0;
        
        //cout<<pre_2_word<<" "<<pre_word<<" "<<cur_word<<" "<<pre_bi_prob<<cur_bi_prob<<cur_prob<<endl;
        
        //exit(1);

        if((pre_prob+cur_prob)>max_prob){
            max_prob = pre_prob + cur_prob;
            max_idx = i;
        }

        //cout<<"cur_prob: "<< cur_prob<<endl;
        //exit(1);

    }
    //if(cur_word == " "){
    //    cout<<"SPACE~~~~~~"<<endl;
    //}
    pair<vector<string>, double> best_path = path_prob.at(max_idx);
    best_path.first.push_back(cur_word);
    best_path.second = max_prob;

    /*if(cur_word != " "){
        best_path.first.push_back(cur_word);
        best_path.second = max_prob;
    }*/

    /*for(int i=0; i<best_path.first.size(); i++){
        cout<<best_path.first.at(i)<<" ";
    }
    cout<<best_path.second<<endl;*/

    //exit(1);
    return best_path;

}



void find_best_line(vector<set<string>> all_nodes, Ngram &lm, Vocab &vocab, vector<string> &max_prob_line){
    vector<pair<vector<string>, double>> path_prob;
    set<string>::iterator iter = all_nodes.begin()->begin();
    vector<string> start;
    /*For <s> as begining*/
    for(iter; iter != all_nodes.begin()->end(); iter++){
        start.clear();
        start.push_back(*iter);
        path_prob.push_back(make_pair(start, 0.0)); // 0.0 should be change for 1-gram probility
    }

    //start.clear();
    //start.push_back("<s>");
    //path_prob.push_back(make_pair(start, 0.0));
    
    for(int i=1; i<2; i++){
        vector<pair<vector<string>, double>> next_path_prob;
        next_path_prob.clear();
        for(iter=all_nodes.at(i).begin(); iter!=all_nodes.at(i).end(); iter++){
            pair<vector<string>, double> cur_best = find_best_bigram(path_prob, *iter, lm, vocab);
            next_path_prob.push_back(cur_best);
        }
        path_prob.clear();
        path_prob = next_path_prob;
    }
    //cout<<all_nodes.at(1).size()<<" "<<path_prob.size()<<endl;
    //exit(1);
    //cout<<"=============="<<endl;
    for(int i=2; i<all_nodes.size(); i++){ // i=0
        vector<pair<vector<string>, double>> next_path_prob;
        next_path_prob.clear();
        //cout<<"size: "<<all_nodes.at(i).size()<<endl;
        //exit(1);
        if(all_nodes.at(i).size() == 1){
            for(iter=all_nodes.at(i).begin(); iter!=all_nodes.at(i).end(); iter++){
                vector<string>::iterator one_w_iter;
                for(int j=0; j<path_prob.size(); j++){
                    //path_prob.at(j).first.push_back(*iter);
                    one_w_iter = path_prob.at(j).first.end();
                    one_w_iter -= 1; // get final word of the path
                    string pre_word = *one_w_iter;
                    one_w_iter -= 1;
                    string pre_2_word = *one_w_iter;
                    string cur_word = *iter;
                    double pre_prob = path_prob.at(j).second;
                    double cur_tri_prob = get_trigram_prob(pre_2_word.c_str(), pre_word.c_str(), cur_word.c_str(), lm, vocab);

                    path_prob.at(j).first.push_back(*iter);
                    path_prob.at(j).second = pre_prob + cur_tri_prob;
                }
            }
        }else{
        for(iter=all_nodes.at(i).begin(); iter!=all_nodes.at(i).end(); iter++){    
            pair<vector<string>, double> cur_best = find_best_trigram(path_prob, *iter, lm, vocab);
            next_path_prob.push_back(cur_best);
            
        }
        path_prob.clear();
        path_prob = next_path_prob;
        }
    }

    //next_path_prob.clear();
    
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
    char* out_file;
    Vocab vocab;
    //Ngram lm(vocab, 2);
    Ngram lm(vocab, 3);
    map<string, set<string>> mapping;
    int lm_order;

    //cout<<"# arg: "<< argc<<endl;
    seg_file = argv[1];
    ReadMappingFile(argv[2], mapping);
    File file_LM(argv[3], "r");
    lm.read(file_LM);
    file_LM.close();
    out_file = argv[4];
    cout<<"in: "<<seg_file<<endl;
    cout<<"out: "<<out_file<<endl;
    
    /////////// For read input seg line /////////////// 
    string str;
    string tmp;
    vector<string> split_line;
    vector<set<string>> all_nodes;
    vector<string> max_prob_line;
    
    FILE *fp = fopen(out_file, "wb");
    ifstream in(seg_file);
    int count = 0;
    while(getline(in, str)){
        count += 1;
        /*if(count != 1){
            continue;
            //exit(1);
        }*/
            
        split_line.clear();
        all_nodes.clear();
        max_prob_line.clear();
        int n = str.length();
        int space_count = 0;
        for(int i=0; i<n; i++){
            if (str.at(i)<0){
                tmp = str.substr(i,2);
                split_line.push_back(tmp);
                //cout<<str.substr(i,2);
                i++;
                space_count = 0;
            }
            
            space_count++;
            /*if(space_count>3){
                split_line.push_back(" ");
            }*/
        }
        /*for(int i=0; i<split_line.size();i++){
            cout<<split_line.at(i);
        }
        exit(1);*/
        //cout<<split_line.size()<<endl;

        find_all_nodes(split_line, mapping, all_nodes);
        //print_vec_set(all_nodes);
        //exit(1);
        find_best_line(all_nodes, lm, vocab, max_prob_line);
        
        //cout<<"<s>";
        //for(int i=0; i<max_prob_line.size(); i++){
        //    cout<<" "<< max_prob_line.at(i);
        //}
        //cout<<" </s>"<<endl;
       
        //fprintf(fp, "<s>");
        for(int i=0; i<max_prob_line.size()-1; i++){
            if(max_prob_line.at(i) == " ")
                continue;
            //fprintf(fp, " ");
            fprintf(fp, max_prob_line.at(i).c_str());
            fprintf(fp, " ");
        }
        fprintf(fp, max_prob_line.at(max_prob_line.size()-1).c_str());
        fprintf(fp, "\n");
        //fprintf(fp, "</s>\n");
        
        
    }

    return 0;
}


#include <File.h>
#include <Ngram.h>
#include <Vocab.h>

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <stdio.h>

using namespace std;
#define log_zero -20

Vocab voc;
map<string,vector<string>> dict;
map<string,vector<string>>::iterator iter;
vector<string> found;

VocabIndex get_index(VocabString w){
    VocabIndex wid=voc.getIndex(w);
    if(wid==Vocab_None){
	return voc.getIndex(Vocab_Unknown);
    }
    else{
	return wid;
    }
}

void viterbi(Ngram bigram, VocabString* words, int length){
    //declare
    VocabIndex emptyContext[] = {Vocab_None};
    LogP prob[200][2048]={0.0};
    int back[200][2048];
    vector<vector<string>> table(200, vector<string>(2048));
    int size[200]={0};
    
    //initial    
    Prob p;
    VocabIndex index1;

    found=dict[words[0]];
    int count=found.size();
    size[0]=count;
    for (int i=0; i<count; i++) {
        VocabIndex index=get_index(found[i].c_str());
        LogP curr_prob=bigram.wordProb(index,emptyContext);
        if (curr_prob==LogP_Zero) {
            curr_prob=log_zero;
        }
        prob[0][i]=curr_prob;
        table[0][i]=found[i];
        back[0][i]=0;
    } 
    
    // doing dynamic programming
    for(int t=1;t<length;t++){
        Prob p1;
        VocabIndex w1;
        found=dict[words[t]];
        count=found.size();
        size[t]=count;
        for (int j=0; j<count; j++) {
            VocabIndex index2=get_index(found[j].c_str());
            LogP max_prob=-1.0/0.0,pre;
            for (int i=0; i<size[t-1]; i++) {
                VocabIndex ind_tmp=get_index(table[t-1][i].c_str());
                VocabIndex context[]={ind_tmp,Vocab_None};
                LogP curr_prob1=bigram.wordProb(index2,context);
                LogP uni_prob=bigram.wordProb(index2,emptyContext);
                if (curr_prob1==LogP_Zero && uni_prob==LogP_Zero) {
                    curr_prob1=log_zero;
                }
                curr_prob1+=prob[t-1][i];
                if (curr_prob1>max_prob) {
                    back[t][j]=i;
                    max_prob=curr_prob1;
                }
            }
            prob[t][j]=max_prob;
            table[t][j]=found[j];
        }
    }
    
    // find max probability
    LogP max=-1.0/0.0;
    int maxend=-1;
    for (int i=0; i<size[length-1]; i++) {
        LogP curr_prob2=prob[length-1][i];
        if (curr_prob2>max) {
            max=curr_prob2;
            maxend=i;
        }
    }
    
    // backtrack path
    int path[200];
    path[length-1]=maxend;
    for (int t=length-2; t>=0; t--) {
        path[t]=back[t+1][path[t+1]];
    }
    printf("%s","<s>");
    printf(" ");
    for (int t=0; t<length; t++) {
        printf("%s",table[t][path[t]].c_str());
        printf(" ");
    }
    printf("%s","</s>");
    printf("\n");
    return;
}


int main(int argc,char** argv){
    // 0:mydisammbig 1:input path 2:map 3:bigram.lm 4:output path
    File mapfile(argv[2],"r");
    char* line;
    while(line=mapfile.getline()){
        VocabString all[2048];
        int cnt=Vocab::parseWords(line,&all[0],2048);
        string tmp=string(all[0]);
        for (int k=1; k<cnt; k++) {
            dict[tmp].push_back(string(all[k]));
        }
    }
    mapfile.close();
    Ngram bigram(voc,2);
    File lmfile(argv[3],"r");
    bigram.read(lmfile);
    lmfile.close();
    File input_file(argv[1],"r");
    FILE* fp=freopen(argv[4],"w",stdout);
    while(line=input_file.getline()){
        VocabString words[200];
        int cnt=Vocab::parseWords(line,&words[0],200);
        if (cnt==0) {
            continue;
        }
        viterbi(bigram,words,cnt);
    }
    input_file.close();
    fclose(stdout);
}


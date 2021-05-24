#include <iostream>
#include <cstdio>
#include <cstring>

#include "hmm.h"

using namespace std;


double best_path(HMM hmm,int observ[],int time_leng){
    int state_num=hmm.state_num;
    int observ_num=hmm.observ_num;
    int time_len=time_leng;
    double delta[60][state_num];
    double result=0;
    for (int t=0; t<time_len; t++) {
        for (int i=0; i<state_num; i++) {
            if (t==0) {
                delta[0][i]=hmm.initial[i]*hmm.observation[observ[0]][i];
            }
            else{
                double ma=-1;
                for (int s=0; s<state_num; s++) {
                    double tmp=delta[t-1][s]*hmm.transition[s][i];
                    if(ma<tmp){
                        ma=tmp;
                    }
                }
                delta[t][i]=ma*hmm.observation[observ[t]][i];
            }
        }
    }
    for (int i=0; i<state_num; i++) {
        if(result<delta[time_len-1][i]){
            result=delta[time_len-1][i];
        }
    }
    return result;
}

void testHMM(HMM hmms[5], char* input, char* output, char txt[10][15]){
    FILE* fh_in= open_or_die(input,"r");
    FILE* fh_out= open_or_die(output,"w");
    
    char observ[60];
    int num_obs[60];
    char path_record[5][60];
    double prob[5];
    int N=0;
    while(fscanf(fh_in, "%s", observ)!=EOF){
        for (int i=0; i<int(strlen(observ)-1); i++) {
            num_obs[i]=observ[i]-'A';
        }
        
        double max=0;
        int max_index=-1;
        for (int i=0; i<5; i++) {
            prob[i]=best_path(hmms[i],num_obs,int(strlen(observ)-1));
            if (prob[i]>max) {
                max=prob[i];
                max_index=i;
            }
        }
        fprintf(fh_out, "%s\t%e\n", txt[max_index], max);
        
    }
    fclose(fh_in);
    fclose(fh_out);
}



int main(int argc, char* argv[]){
    if(argc!=4){
        cout<<"argc error"<<endl;
        return 1;
    }
    string file_name=argv[0];
    char* model_list_path=argv[1];
    char* seq_path=argv[2];
    char* result_output=argv[3];
    
    FILE* txt=open_or_die(model_list_path,"r");
    char txt_name[10][15];
    int index=0;
    while (fscanf(txt,"%s",txt_name[index])!=EOF) {
        index++;
    }

    HMM hmms[5];
    load_models(model_list_path, hmms, 6);
    testHMM(hmms, seq_path, result_output, txt_name);

    return 0;

}

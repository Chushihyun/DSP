#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdio.h>

#include "hmm.h"

using namespace std;

/*
typedef struct{
    char *model_name;
    int state_num;					//number of state
    int observ_num;					//number of observation
    double initial[MAX_STATE];			//initial prob.
    double transition[MAX_STATE][MAX_STATE];	//transition prob.
    double observation[MAX_OBSERV][MAX_STATE];	//observation prob.
} HMM;
*/

void trainHMM(HMM& hmm, int iter_num, char* data_path){
    char data[60];
    // access hmm content
    int state_num=hmm.state_num;
    int observ_num=hmm.observ_num;
    double pi[state_num];
    double A[state_num][state_num];
    double B[observ_num][state_num];
    // hmm.initial=pi[state]  hmm.transition=A[state_i][state_j]  hmm.observation=B[observ][state]
    for (int i=0; i<state_num; i++) {
        pi[i]=hmm.initial[i];
    }
    for (int i=0; i<state_num; i++) {
        for (int j=0; j<state_num; j++) {
            A[i][j]=hmm.transition[i][j];
        }
    }
    for (int i=0; i<observ_num; i++) {
        for (int j=0; j<state_num; j++) {
            B[i][j]=hmm.observation[i][j];
        }
    }
    
    // public variable
    double total;
    double total1;
    double total2;
    double p;
    for(int iter=0;iter<iter_num;iter++){
        
        // alpha, beta , epsilon, gamma
        double alpha[60][state_num];
        double beta[60][state_num];
        double epsilon[60][state_num][state_num];
        double gamma[60][state_num];
        double observ_gamma[observ_num][state_num];
        float count=0.0;
        char observ[60];
        int num_obs[60];
        int time_len=0;
        
        FILE* fh=open_or_die(data_path,"r");
        
        // refresh
        for (int i=0; i<60; i++) {
            for (int s=0; s<state_num; s++) {
                gamma[i][s]=0;
            }
        }
        for (int i=0; i<observ_num; i++) {
            for (int s=0; s<state_num; s++) {
                observ_gamma[i][s]=0;
            }
        }
        for (int i=0; i<60; i++) {
            for (int s=0; s<state_num; s++) {
                for (int s1=0; s1<state_num; s1++) {
                    epsilon[i][s][s1]=0;
                }
            }
        }
        
        while(fgets(observ,60,fh) != NULL){
            // data processing
            if (time_len==0) {
                time_len=int(strlen(observ)-1);
            }
            for (int i=0; i<time_len; i++) {
                num_obs[i]=observ[i]-'A';
            }
            count+=1.0;
            
            // calculate alpha
            for (int t=0; t<time_len; t++) {
                 for (int s=0; s<state_num; s++){
                    if (t==0) {
                        alpha[0][s]=pi[s]*B[num_obs[0]][s];
                    }
                    else{
                        total=0.0;
                        for (int i=0; i<state_num; i++) {
                            total+=alpha[t-1][i]*A[i][s];
                        }
                        alpha[t][s]=total*B[num_obs[t]][s];
                    }
                }
            }
            
            // calculate beta
            for (int t=time_len-1; t>=0; t--) {
                 for (int s=0; s<state_num; s++){
                    if (t==time_len-1) {
                        beta[time_len-1][s]=1;
                    }
                    else{
                        total=0.0;
                        for (int i=0; i<state_num; i++) {
                            total+=A[s][i]*beta[t+1][i]*B[num_obs[t+1]][i];
                        }
                        beta[t][s]=total;
                    }
                }
            }
            
            // calculate sum[state]
            for (int t=0; t<time_len; t++) {
                double prob[state_num];
                total=0.0;
                for (int s=0; s<state_num; s++) {
                    prob[s]=alpha[t][s]*beta[t][s];
                    total+=prob[s];
                }
                // calculate gamma
                for (int i=0; i<state_num; i++) {
                    gamma[t][i]+=prob[i]/total;
                    observ_gamma[num_obs[t]][i]+=prob[i]/total;
                }
                
                // calculate epsilon
                if (t==time_len-1) {
                    continue;
                }
                for (int i=0; i<state_num; i++) {
                    for (int j=0; j<state_num; j++) {
                        epsilon[t][i][j]+=(alpha[t][i]*A[i][j]*beta[t+1][j]*B[num_obs[t+1]][j])/total;
                    }
                }
            }
        }
        fclose(fh);
        
        // update HMM
        for (int i=0; i<state_num; i++) {
            for (int j=0; j<state_num; j++) {
                total1=0;
                total2=0;
                for (int t=0; t<time_len-1; t++) {
                    total1+=epsilon[t][i][j];
                    total2+=gamma[t][i];
                }
                A[i][j]=total1/total2;
            }
        }
        
        for (int s=0; s<state_num; s++) {
            pi[s]=gamma[0][s]/count;
        }
        
        for (int s=0; s<state_num; s++) {
            for (int ob=0; ob<observ_num; ob++) {
                total=0.0;
                for (int t=0; t<time_len; t++) {
                    total+=gamma[t][s];
                }
                B[ob][s]=observ_gamma[ob][s]/total;
            }
        }
        // finish one iteration
        
    }
    // finish all iteration
    
    // update to input hmm
    for (int s=0; s<state_num; s++) {
        hmm.initial[s]=pi[s];
    }
    
    
    for (int i=0; i<state_num; i++) {
        for (int j=0; j<state_num; j++) {
            hmm.transition[i][j]=A[i][j];
        }
    }
    
    
    for (int s=0; s<state_num; s++) {
        for (int ob=0; ob<observ_num; ob++) {
            hmm.observation[ob][s]=B[ob][s];
        }
    }
}


int main(int argc,char* argv[]){
    if(argc!=5){
        cout<<"argc error"<<endl;
        return 1;
    }
    string file_name=argv[0];
    int iter_num=atoi(argv[1]);
    char* model_input=argv[2];
    char* seq_model=argv[3];
    char* model_output=argv[4];

    HMM hmm;
    loadHMM(&hmm, model_input );

    trainHMM(hmm,iter_num,seq_model);
    
    FILE* fh=open_or_die(model_output,"w");
    dumpHMM(fh, &hmm);
    fclose(fh);

    return 0;

}

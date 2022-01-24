// So I'm doing some research into the douglas ratchford algorithm.
// This is the only problem I know of on orac2 that the algorithm can be applied to.
// Let's see how this runs

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <cmath>
#include <time.h>
using namespace std;
int N;

bool doRandomisation = false;
bool projectCheckingOptimisation = false;
bool calcDist = false;
bool addNoise = false;
float noiseMagnitude = 0.001;

int NUM_TICKS = 2000000;

int constraints[3][200];
inline int myRound(float x){
    if (x >= 0.5){
        return 1;
    }
    return 0;
}

float randFloat(float l, float h){
    float x =(float) rand();
    return (h - l) * (x/(float)RAND_MAX) + l;
}
inline vector<float> projS(vector<float> &v, int m, bool discrete){
    int n = v.size();
    vector<float> res(n,0);
    if (discrete){
        vector<pair<float,int>> temp;
        for (int i = 0; i < n; i++){
            if (doRandomisation){
                temp.push_back({v[i] + randFloat(0,0.01),i});
            }
            else{
                temp.push_back({v[i],i});
            }
        }
        sort(temp.begin(),temp.end());
        for (int i = n-m; i < n; i++){
            res[temp[i].second] = 1;
        }
    }
    else{
        float s = 0;
        for (int i = 0; i < n; i++){
            s += v[i];
        }
        for (int i = 0; i < n; i++){
            res[i] = v[i] + (m-s)/n;
        }
    }
    return res;

}
inline vector<float> projH(vector<float> &v, int m, bool discrete){
    int n = v.size();
    vector<float> res(n,0);
    
    if (discrete){
        vector<pair<float,int>> temp;
        for (int i = 0; i < n; i++){
            if (doRandomisation){
                temp.push_back({v[i] + randFloat(0,0.01),i});
            }
            else{
                temp.push_back({v[i],i});
            }
        }
        sort(temp.begin(),temp.end());
        for (int i = n-m; i < n; i++){
            if (temp[i].first > 0.5){
                res[temp[i].second] = 1;
            }
        }
    }
    else{
        float s = 0;
        for (int i = 0; i < n; i++){
            s += v[i];
        }
        for (int i = 0; i < n; i++){
            res[i] = v[i] + min(0.f,(m-s)/n);
        }
    }
    return res;
}

class Board{
    public:
    int n;
    vector<vector<float>> b;

    Board(int boardSize, bool initRandom){
        n = boardSize;
        b = vector<vector<float>>(n,vector<float>(n,0));
        if (initRandom){
            for (int i = 0; i < n; i++){
                for (int j = 0; j < n ; j++){
                    b[i][j] = rand() % 2;
                }
            }
        }
    }
    Board(){
        n = N;
        b = vector<vector<float>>(n,vector<float>(n,0));
    }
    Board project(bool discrete, int idx){
        Board result(n,false);
        if (idx <= 1){
            for (int i = 0; i < n; i++){
                vector<float> v;
                for (int j = 0; j < n; j++){
                    if (idx == 0){
                        v.push_back(b[i][j]);
                    }
                    else{
                        v.push_back(b[j][i]);
                    }
                }
                
                vector<float> temp = projS(v,constraints[idx][i],true);
                for (int j = 0; j < n; j++){
                    if (idx == 0){
                        result.b[i][j] = temp[j];
                    }
                    else{
                        result.b[j][i] = temp[j];
                    }
                }
                
            }
            return result;
        }
        else{
            for (int i = 0; i < n; i++){
                vector<float> v1;
                vector<float> v2;
                for (int j = 0; j <= i; j++){
                    v1.push_back(b[i-j][n-1-j]);
                    if (i != n-1){
                        v2.push_back(b[n-1 - (i-j)][j]);
                    }
                }
                vector<float> t1;
                vector<float> t2;
                t1 = projS(v1,constraints[2][i],discrete);
                if (i != n-1){
                    t2 = projS(v2,constraints[2][2 * n - 1 - i], discrete);
                }
                for (int j = 0; j <= i; j++){
                    result.b[i-j][n-1-j] = t1[j];
                    if (i != n-1){
                        result.b[n-1-(i-j)][j] = t2[j];
                    }
                }
                // v.push_back(b[])
            }
            return result;
        }


    }
    int check(){
        int cnt = 0;
        for (int i = 0; i < n; i++){
            int hcnt = 0;
            int vcnt = 0;
            int dcnt1 = 0;
            int dcnt2 = 0;
            for (int j = 0; j < n; j++){
                hcnt += myRound(b[j][i]);
                vcnt += myRound(b[i][j]);
            }
            for (int j = 0; j <= i; j++){
                dcnt1 += myRound(b[i-j][n-1-j]);
                if (i != n-1){
                    dcnt2 += myRound(b[n-1-(i-j)][j]);
                }
            }

            cnt += abs(hcnt-constraints[0][i]);
            cnt += abs(vcnt-constraints[1][i]);
            cnt += abs(dcnt1-constraints[2][i]);
            cnt += abs(dcnt2-constraints[2][2*n-1-i]);
        }
        return cnt;
    }
    Board operator+(Board const &other){
        if (n != other.n){
            throw 1;
        }
        Board res;

        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                res.b[i][j] = b[i][j] + other.b[i][j];
            }
        }
        return res;
    }
    Board operator*(float const &other){
        Board res;

        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                res.b[i][j] = b[i][j] * other;
            }
        }
        return res;
    }
    Board operator-(Board const &other){
        Board res;

        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                res.b[i][j] = b[i][j] - other.b[i][j];
            }
        }
        return res;
    }
    void print(bool floats){
        for (int y = 0; y < n; y++){
            for (int x = 0; x < n; x++){
                if (floats){
                    printf("%.2f ", b[x][y]);
                }
                else{
                    printf("%d", 1-myRound(b[x][y]));
                }
            }
            printf("\n");
        }
    }
    float normSquared(){
        float d = 0;
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                d += b[i][j] * b[i][j];
            }
        }
        return d;
    }
    void addNoise(){
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                b[i][j] += randFloat(-noiseMagnitude,noiseMagnitude);
            }
        }
    }
};

Board boards[2][5];
Board toProject[5];
Board projected[5];
int best = 1e9;
Board bestBoard;
int main(){
    scanf("%d", &N);
    NUM_TICKS = NUM_TICKS/(N*N);
    srand(time(NULL));
    for (int i = 0; i < N; i++){
        scanf("%d", &constraints[0][i]);
    }

    for (int i = 0; i < N; i++){
        scanf("%d", &constraints[1][i]);
    }
    for (int i = 0; i < 2*N-1; i++){
        scanf("%d", &constraints[2][i]);
    }

    boards[0][0] = Board(N,true);
    for (int i = 1; i < 3; i++){
        boards[0][i] = boards[0][0];
    }
    Board avg;
    for (int k = 0; k < NUM_TICKS; k++){
        avg = (boards[k%2][0] + boards[k%2][1] + boards[k%2][2]) * 0.333;
        if (addNoise){
            avg.addNoise();
        }
        int val = avg.project(true,0).check();
        if (val < best){
            best = val;
            bestBoard = avg;
        }
        if (k == NUM_TICKS-1 || best == 0){
            printf("%d\n", best);
            bestBoard.project(true,0).print(false);
            return 0;
        }
        for (int i = 0; i < 3; i++){
            projected[i] = (avg * 2 - boards[k%2][i]).project(true,i);
        }
        for (int i = 0; i < 3; i++){
            boards[(k+1)%2][i] = boards[k%2][i] + projected[i] - avg;
        }        

    }

}
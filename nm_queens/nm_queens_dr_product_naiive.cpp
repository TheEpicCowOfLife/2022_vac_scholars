#include <cstdio>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <time.h>
using namespace std;

int m;
int N;
const int NUM_TICKS = 3000;
int myRound(float x){
    if (x > 0.5){
        return 1;
    }
    return 0;
}
vector<float> projS(vector<float> &v, bool discrete){
    int n = v.size();
    vector<float> res(n,0);
    if (discrete){
        vector<pair<float,int>> temp;
        for (int i = 0; i < n; i++){
            temp.push_back({v[i],i});
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
vector<float> projH(vector<float> &v, bool discrete){
    int n = v.size();
    vector<float> res(n,0);
    
    if (discrete){
        vector<pair<float,int>> temp;
        for (int i = 0; i < n; i++){
            temp.push_back({v[i],i});
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

    Board projectHorizontal(bool discrete){
        Board result(n,false);
        for (int y = 0; y < n; y++){
            vector<float> v;
            for (int x = 0; x < n; x++){
                v.push_back(b[x][y]);
            }
            vector<float> temp = projS(v, discrete);
            for (int x = 0; x < n; x++){
                result.b[x][y] = temp[x];
            }
        }
        return result;
    }
    Board projectVertical(bool discrete){
        Board result(n,false);
        for (int x = 0; x < n; x++){
            result.b[x] = projS(b[x],discrete);
        }
        return result;
    }
    Board projectDiagonal1(bool discrete){
        Board result(n,false);
        result.b = b;
        for (int i = m; i < n; i++){

            vector<float> v1;
            vector<float> v2;
            for (int j = 0; j <= i; j++){
                v1.push_back(b[i-j][j]);
                if (i != n-1){
                    v2.push_back(b[n-1 - (i-j)][n-1 - j]);
                }
            }
            
            vector<float> t1;
            vector<float> t2;
            t1 = projH(v1,discrete);
            if (i != n-1){
                t2 = projH(v2,discrete);
            }

            for (int j = 0; j <= i; j++){
                result.b[i-j][j] = t1[j];
                if (i != n-1){
                    result.b[n-1-(i-j)][n-1-j] = t2[j];
                }
            }
        }
        return result;
    }
    Board projectDiagonal2(bool discrete){
    Board result(n,false);
        result.b = b;
        for (int i = m; i < n; i++){

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
            t1 = projH(v1,discrete);
            if (i != n-1){
                t2 = projH(v2,discrete);
            }

            for (int j = 0; j <= i; j++){
                result.b[i-j][n-1-j] = t1[j];
                if (i != n-1){
                    result.b[n-1-(i-j)][j] = t2[j];
                }
            }
        }
        return result;
    }

    bool check(){
        bool res = true;
        for (int i = 0; i < n; i++){
            int hcnt = 0;
            int vcnt = 0;
            for (int j = 0; j < n; j++){
                hcnt += myRound(b[j][i]);
                vcnt += myRound(b[i][j]);
            }
            if (hcnt != m){
                printf("Failed horizontal\n");
                res = false;
            }
            if (vcnt != m){
                printf("Failed vertical\n");
                res = false;

            }
        }
        for (int i = 0; i < n; i++){
            int cnt[4] = {0,0,0,0};

            for (int j = 0; j <= i; j++){
                cnt[0] += myRound(b[i-j][j]);
                cnt[1] += myRound(b[n-1 - (i-j)][n-1-j]);
                cnt[2] += myRound(b[n-1 - (i-j)][j]);
                cnt[3] += myRound(b[i-j][n-1-j]);
            }
            for (int j = 0; j < 4; j++){
                if (cnt[j] > m){
                    printf("Failed diag %d\n", j);
                    res = false;
                }
            }
        }
        return res;
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
                    printf("%d ", myRound(b[x][y]));
                }
            }
            printf("\n");
        }
    }
};


// Product space.
Board boards[2][4];

Board toProject[4];

Board projected[4];

int main(){
    printf("Enter N and M: ");
    scanf("%d %d", &N, &m);

    srand(time(NULL));
    boards[0][0] = Board(N,true);
    for (int i = 1; i < 4; i++){
        boards[0][i] = boards[0][0];
    }

    for (int k = 0; k < NUM_TICKS; k++){
        Board avg(N,false);

        avg =  (boards[k%2][0] + boards[k%2][1] + boards[k%2][2] + boards[k%2][3]) * 0.25;
        for (int i = 0; i < 4; i++){
            toProject[i] = avg * 2 - boards[k%2][i]; 
        }

        projected[0] = toProject[0].projectHorizontal(true);
        projected[1] = toProject[1].projectVertical(true);
        projected[2] = toProject[2].projectDiagonal1(true);
        projected[3] = toProject[3].projectDiagonal2(true);
        for (int i = 0; i < 4; i++){
            boards[(k+1)%2][i] = boards[k%2][i] * 0.5 + projected[i] - toProject[i] * 0.5;
        }
    }
    Board ans;
    for (int i = 0; i < 4; i++){
        ans = ans + boards[NUM_TICKS%2][i];
    // ans.print(true);

    }
    ans = ans * 0.25;
    ans.check();
    ans.print(false);
}
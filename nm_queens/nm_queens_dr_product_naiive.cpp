#include <cstdio>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <time.h>
#include <chrono>
#include <cmath>
using namespace std;
using namespace std::chrono;
int m;
int N;

// Maximum number of iterations to run.
const int NUM_TICKS = 5000;

// Two modes:
// Will read in data of a single board and output it.
// bool outputBoardMode = true;
// bool runTrialsMode = false;
bool outputBoardMode = false;
bool runTrialsMode = false;
bool collectData = true;

// Number of trials in runTrials mode
int num_trials = 2;

// When projecting to the discrete sets, there are multiple possible projections choose from.
// This option enables random selection of these candidates... in a kinda hacky and non-uniform way.
bool doRandomisation = false;

// There are many ways we can check if we've successfully solved an instance.
// If the below is true, it will check it by  projecting it to the discrete horizontal constraint
// then checking if it satisfies the rest of the constraints
// Otherwise it just rounds to the nearest integer and then check that.
bool projectCheckingOptimisation = false;

bool runExtension = true;
float myGamma = 0.50;

// If this is true, it adds a uniformly random float in [-noiseMagnitude,noiseMagnitude] to each cell every iteration
bool addNoise = false;
float noiseMagnitude = 0.01;



// If true, then in run trials mode will output the summary in a google-sheets friendly format.
// I used it for pasting into google sheets.
// Otherwise will give a more human-readable summary
bool outputSummaryCSV = false;

// Some misc stuff: If true, will calc the distance^2 from Pd(x_i) to Pc(Pd(x_i)) every iteration
// In other words it will output the rough "distance^2" from satisfying all constraints to distFilename.
bool calcDist = false;
char distFilename[105] = "output.txt";
char csvFileName[105] = "observations.csv";


float median(vector<float> v){

    int n = v.size();
    nth_element(v.begin(), v.begin() + n/2, v.end());
    if (n % 2 == 0){
        nth_element(v.begin(), v.begin() + n/2 + 1, v.end());
        return (v[n/2] + v[n/2 + 1])/2.0;
    }
    return v[n/2];
}


FILE * distFile;
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
inline vector<float> projS(vector<float> &v, bool discrete){
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
            res[i] = v[i] + ((float)m-s)/(float)n;
        }
    }
    return res;

}
inline vector<float> projH(vector<float> &v, bool discrete){
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
            res[i] = v[i] + min(0.f,((float)m-s)/(float)n);
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
    Board projectIdx(bool discrete, int idx){
        switch(idx){
            case 0:
            return projectHorizontal(discrete);

            case 1:
            return projectVertical(discrete);

            case 2:
            return projectDiagonal1(discrete);

            case 3:
            return projectDiagonal2(discrete);
        }
        Board wtf;
        return wtf;
    }
    bool check(bool debug){
        bool res = true;
        for (int i = 0; i < n; i++){
            int hcnt = 0;
            int vcnt = 0;
            for (int j = 0; j < n; j++){
                hcnt += myRound(b[j][i]);
                vcnt += myRound(b[i][j]);
            }
            if (hcnt != m){
                if (debug){
                    printf("Failed horizontal\n");
                }
                else{
                    return false;
                }
                res = false;
            }
            if (vcnt != m){
                if (debug){
                    printf("Failed vertical\n");
                }
                else{
                    return false;
                }
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
                    if (debug){
                        printf("Failed diag %d\n", j);

                    }
                    else{
                        return false;
                    }
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


// Product space.
Board boards[2][4];

Board toProject[4];

Board projected[4];

struct Result{
    bool success;
    int iterations;
    long long milliseconds;
};

long long getMillis(){
    return  duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
            ).count();
}

Result doBoard(int seed, bool output, FILE * output_file){

    if (output){
        fprintf(output_file, "%d\n", N);
    }
    if (calcDist){
        distFile = fopen(distFilename,"w");
    }

    srand(seed);
    long long start = getMillis();

    boards[0][0] = Board(N,true);
    for (int i = 1; i < 4; i++){
        boards[0][i] = boards[0][0];
    }

    for (int k = 0; k < NUM_TICKS; k++){
        Board avg(N,false);

        avg =  (boards[k%2][0] + boards[k%2][1] + boards[k%2][2] + boards[k%2][3]) * 0.25;
        
        if (calcDist){
            float d = 
            (avg - avg.projectHorizontal(true)).normSquared() +
            (avg - avg.projectVertical(true)).normSquared() +
            (avg - avg.projectDiagonal1(true)).normSquared() +
            (avg - avg.projectDiagonal2(true)).normSquared()
            ;            
            fprintf(distFile, "%f\n", d);
        }

        if (output){
            for (int y = 0; y < N; y++){
                for (int x = 0; x < N; x++){
                    fprintf(output_file, "%f ", avg.b[x][y]);
                    
                }
                fprintf(output_file,"\n");
            }
        }
        bool check;
        if (projectCheckingOptimisation){
            check = avg.projectVertical(true).check(false);
        }
        else{
            check = avg.check(false);
        }
        if (check){
            long long end = getMillis();
            if (output){
                fprintf(output_file,"-999999\n");
            }
            return {true,k, end - start};
        }

        for (int i = 0; i < 4; i++){
            toProject[i] = avg * 2 - boards[k%2][i]; 
        }

        projected[0] = toProject[0].projectHorizontal(true);
        projected[1] = toProject[1].projectVertical(true);
        projected[2] = toProject[2].projectDiagonal1(true);
        projected[3] = toProject[3].projectDiagonal2(true);
        for (int i = 0; i < 4; i++){
            boards[(k+1)%2][i] = boards[k%2][i] * 0.5 + projected[i] - toProject[i] * 0.5;
            if (addNoise){
                boards[(k+1)%2][i].addNoise();
            }
        }
    }
    if (output){
        fprintf(output_file,"-999999\n");
    }
    return {false,0, getMillis() - start};
}

Result doBoard(int seed){
    FILE * file;
    return doBoard(seed,false,file);
}

Result doExtension(int seed, bool output, FILE * output_file){
    srand(seed);
    if (output){
        fprintf(output_file, "%d\n", N);
    }
    if (calcDist){
        distFile = fopen(distFilename,"w");
    }

    long long start = getMillis();

    Board z[3];
    Board x[4];

    z[0] = Board(N, true);
    z[1] = z[0];
    z[2] = z[0];

    for (int k = 0; k < NUM_TICKS; k++){
        x[0] = z[0].projectIdx(true, 0);
        for (int i = 1; i <= 2; i++){
            x[i] = (z[i] - z[i-1] + x[i-1]).projectIdx(true,i);
        }
        x[3] = (x[0] + x[2] - z[2]).projectIdx(true,3);
        Board avg = (x[0] + x[1] + x[2] + x[3]) * 0.25;
        if (output){
            for (int y = 0; y < N; y++){
                for (int x = 0; x < N; x++){
                    fprintf(output_file, "%f ", avg.b[x][y]);
                    
                }
                fprintf(output_file,"\n");
            }
        }

        bool check;
        if (projectCheckingOptimisation){
            check = avg.projectVertical(true).check(false);
        }
        else{
            check = avg.check(false);
        }
        if (check){
            long long end = getMillis();
            if (output){
                fprintf(output_file,"-999999\n");
            }
            return {true,k, end - start};
        }

        for (int i = 0; i < 3; i++){
            z[i] = z[i] + (x[i+1] - x[i]) * myGamma;
            if (addNoise){
            z[i].addNoise();
            }
        }
        
    }
    if (output){
        fprintf(output_file,"-999999\n");
    }
    return {false,0, getMillis() - start};
}
Result doExtension(int seed){
    return doExtension(seed,false,NULL);
}
char temps[105];
char filename[10005];

FILE * dataCSV;

void runTrials(int seed){
    double avg_it = 0;
    int num_success = 0;
    
    double avg_time = 0;
    vector<float> mv;

    printf("\n\nn,m: %d %d\nExtension: %d\nprojectCheck:%d\nRandom: %d\n", N, m, runExtension, projectCheckingOptimisation, addNoise);

    for (int i = seed; i < num_trials+seed; i++){
        Result r;
        if (runExtension){
            r = doExtension(i);
        }
        else{
            r = doBoard(i);
        }
        printf("trial %d: %d %d %lld\n", i, r.success, r.iterations, r.milliseconds);
        if (r.success){
            avg_it += r.iterations;
            avg_time += r.milliseconds;
            num_success++;
            mv.push_back(r.iterations);
        }   
    }

    if (num_success > 0){
        if (!outputSummaryCSV){
            printf("%d/%d successful, Average: %lf iterations, %lf seconds, Median %lf iterations\n", 
            num_success, num_trials, avg_it/(double)num_success, avg_time/(double)num_success / 1000.0, median(mv));
        }
        else{
            printf("%d/%d,%lf,%lf,%lf\n", 
            num_success, num_trials, avg_it/(double)num_success, median(mv), avg_time/(double)num_success / 1000.0);
        }
    }

    if (collectData){
        fprintf(dataCSV,"%d,%d,%d,%d,%d,",
            N, m, runExtension, projectCheckingOptimisation, addNoise);
        if (num_success > 0){
            fprintf(dataCSV,"%d,%lf,%lf,%lf\n",             
                num_success, avg_it/(double)num_success, median(mv), avg_time/(double)num_success / 1000.0);
        }        
        else{
            fprintf(dataCSV,"\n");
        }
    }
}

void runTrials(){
    srand(time(NULL));
    runTrials(rand() % 1000000);
}

int main(){
    int seed;
    
    if (outputBoardMode){
        printf("Enter N, M, seed, and the output file name: ");
        scanf("%d %d %d %s", &N, &m, &seed, temps);
        sprintf(filename,"visualiser/inputs/%s.out",temps);
        FILE * file = fopen(filename, "w");
        if (runExtension){
        doExtension(seed,true,file);

        }
        else{
        doBoard(seed,true,file);

        }

    }
    else if (runTrialsMode){
        printf("Enter N, M, seed:");
        scanf("%d %d %d", &N, &m, &seed);
        runTrials(seed);    
    }
    else if (collectData){
        dataCSV = fopen(csvFileName, "w");
        fprintf(dataCSV, "N, M, isExtension, projectCheck, randomisation, success, avg it, median it, milliseconds\n");
        for (int msk = 0; msk < 32; msk++){
            if ((msk & 3) == 0){
                N = 20;
                m = 2;
            }
            if ((msk & 3) == 1){
                N = 50;
                m = 2;
            }
            if ((msk & 3) == 2){
                N = 20;
                m = 10;
            }
            if ((msk & 3) == 3){
                N = 50;
                m = 25;
            }
            
            addNoise = (msk & 4) > 0;
            projectCheckingOptimisation = (msk&8) > 0;
            runExtension = (msk&16) > 0;
            
            runTrials(0);
        }

    }


}
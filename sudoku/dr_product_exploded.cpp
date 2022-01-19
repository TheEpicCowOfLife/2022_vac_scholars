#include <cstdio>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cassert>
using namespace std;
using namespace std::chrono;
#define FOR3(x) for (int i = 0; i < n; i++){for (int j = 0; j < n; j++){for (int k = 0; k < n; k++) {x}}}
#define FOR2(x) for (int i = 0; i < n; i++){for (int j = 0; j < n; j++){x}}


const int NUM_TICKS = 10000;
const int n = 9;
// To slightly save on storage, we output the board floats as ints: multiplied by this outputFactor.
const int outputFactor = 100; 

// char fileName[1005] = "data/puzzles0_kaggle";
char fileName[1005] = "data/puzzles2_17_clue";

// char fileName[1005] = "data/puzzles3_magictour_top1465";
// char fileName[105] = "data/puzzles6_forum_hardest_1106";


bool doRandomisation = false;
const int maxBoardsScanned = 100; 
int boardsScanned = 100;
int allBoards[maxBoardsScanned][n][n];
int constraintBoard[n][n];

void setConstraintBoard(int k){
    FOR2(
        constraintBoard[i][j] = allBoards[k][i][j];
    )
}

inline int myRound(float x){
    if (x >= 0.5){
        return 1;
    }
    return 0;
}

inline int actuallyCastToIntRound(float x){
    return (int)round(x);
}

float randFloat(float l, float h){
    float x =(float) rand();
    return (h - l) * (x/(float)RAND_MAX) + l;
}
inline vector<float> projS(vector<float> &v, bool discrete){
    int n = v.size();
    vector<float> res(n,0);
    if (discrete){
        // argmax in c++, apparently.
        res[distance(v.begin(), max_element(v.begin(), v.end()))] = 1;
    }
    else{
        float s = 0;
        for (int i = 0; i < n; i++){
            s += v[i];
        }
        for (int i = 0; i < n; i++){
            res[i] = v[i] + (1-s)/n;
        }
    }
    return res;
}

struct Board{
    float b[n][n][n];

    Board(){
        FOR3(b[i][j][k] = 0;)
    }
    Board(bool initRand){
        if (initRand){
            FOR3(b[i][j][k] = rand() % 2;)
        }
        else{
            FOR3(b[i][j][k] = 0;)
        }
    }

    Board projectLine(bool discrete, int idx){
        Board projected;
        FOR2(
            vector<float> v;

            for (int k = 0; k < n; k++){
                if (idx == 0){
                    v.push_back(b[i][j][k]);
                }
                else if (idx == 1){
                    v.push_back(b[i][k][j]);
                }
                else if (idx == 2){
                    v.push_back(b[k][i][j]);
                }
                else if (idx == 3){
                    int bx = i % 3;
                    int by = i / 3;
                    int x = 3 * bx + k % 3;
                    int y = 3 * by + k / 3;
                    v.push_back(b[x][y][j]);
                }
            }

            vector<float> res = projS(v,discrete);
            for (int k = 0; k < n; k++){
                if (idx == 0){
                    projected.b[i][j][k] = res[k];
                }
                else if (idx == 1){
                    projected.b[i][k][j] = res[k];
                }
                else if (idx == 2){
                    projected.b[k][i][j] = res[k];
                }
                else if (idx == 3){
                    int bx = i % 3;
                    int by = i / 3;
                    int x = 3 * bx + k % 3;
                    int y = 3 * by + k / 3;
                    assert(0 <= x && x < n);
                    assert(0 <= y && y < n);

                    projected.b[x][y][j] = res[k];
                }
            }
        )
        return projected;
    }
    Board projectMaster(){
        Board projected;
        FOR2(
            if (constraintBoard[i][j] != -1){
                assert(0 <= constraintBoard[i][j] && constraintBoard[i][j] < n);
                projected.b[i][j][constraintBoard[i][j]] = 1;
            }
            else{
                for (int k = 0; k < n; k++){
                    projected.b[i][j][k] = b[i][j][k];
                }
            }
        );
        return projected;
    }   

    Board operator+(Board const &other){
        Board res;
        FOR3(
            res.b[i][j][k] = b[i][j][k] + other.b[i][j][k];
        )
        return res;
    }
    Board operator*(float const &other){
        Board res;
        FOR3(
            res.b[i][j][k] = b[i][j][k] * other;
        )
        return res;
    }
    Board operator-(Board const &other){
        Board res;
        FOR3(
            res.b[i][j][k] = b[i][j][k] - other.b[i][j][k];
        )
        return res;
    }

    bool check(){
        FOR2(
            int cnt[4];
            for (int k = 0;k < 4; k++){
                cnt[k] = 0;
            }
            for (int k = 0; k < n; k++){
                cnt[0] += myRound(b[i][j][k]);
                cnt[1] += myRound(b[i][k][j]);
                cnt[2] += myRound(b[k][i][j]);
                int bx = i % 3;
                int by = i / 3;
                int x = 3 * bx + k % 3;
                int y = 3 * by + k / 3;

                cnt[3] += myRound(b[x][y][j]);
            }
            for (int k = 0;k < 4; k++){
                if (cnt[k] != 1){
                    return false;
                }
            }
            if (constraintBoard[i][j] != -1){
                for (int k = 0; k < n; k++){
                    if ((constraintBoard[i][j] == k) != (myRound(b[i][j][k]) == 1)){
                        return false;
                    }
                }
            }
            
        )
        return true;
        
    }

    void printBoard(){
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                bool printed = false;
                for (int k = 0; k < n; k++){
                    if (myRound(b[i][j][k])){
                        printf("%d",k+1);
                        printed = true;
                        break;
                    }
                }
                if (!printed){
                    printf("0");
                }
            }
            printf("\n");
        }
        printf("\n");          
    }

    void outputBoard(FILE * outputFile){
        FOR2(
            for (int k = 0; k < n; k++){
                fprintf(outputFile, "%d ", actuallyCastToIntRound(b[i][j][k] * outputFactor));
            }
            fprintf(outputFile,"\n");
        )
    }
};

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

Board boards[2][5];
Board projected[5];

// precondition: the constraint board has been filled out already.
Result doBoard(int seed, bool doOutput, FILE * outputFile){
    srand(seed);

    if (doOutput){
        fprintf(outputFile,"%d %d\n", n, outputFactor);
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                fprintf(outputFile, "%d ", constraintBoard[i][j]);
            }
            fprintf(outputFile, "\n");
        }
    }

    long long start = getMillis();
    int successfulIteration = -1;
    boards[0][0] = Board(true);
    for (int i = 1; i < 5; i++){
        boards[0][i] = boards[0][0];
    }
    Board avg;
    for (int k = 0; k < NUM_TICKS; k++){
        avg = (boards[k%2][0] + boards[k%2][1] + boards[k%2][2] + boards[k%2][3] + boards[k%2][4]) * 0.2;

        if (doOutput){
            avg.outputBoard(outputFile);
        }
        if (avg.projectLine(true,0).check()){
            // avg.projectLine(true,0).printBoard();
            if (successfulIteration == -1){
                successfulIteration = k;
            }
            else if (k >= successfulIteration + 10){
                fprintf(outputFile, "-999999\n");
                return {true,successfulIteration, getMillis() - start};
            }

        }

        for (int i = 0; i < 4; i++){
            projected[i] = (avg * 2 - boards[k%2][i]).projectLine(true,i);
        }
        projected[4] = (avg * 2 - boards[k%2][4]).projectMaster();

        for (int i = 0; i < 5; i++){
            boards[(k+1)%2][i] = boards[k%2][i] + projected[i] - avg;
        }        
    }
    // avg.projectLine(true,0).printBoard();
    return {false, 0, getMillis() - start};

}

Result doBoard(int seed){
    FILE* dummy;
    return doBoard(seed,false,dummy);
}

void doTrials(){
    double avg_it = 0;
    int num_success = 0;
    int num_trials = boardsScanned;
    double avg_time = 0;
    for (int k = 0; k < boardsScanned; k++){
        FOR2(
            constraintBoard[i][j] = allBoards[k][i][j];
        );
        Result r = doBoard(k);
        printf("Trial (and seed) %d: %d %d %lld\n", k, r.success, r.iterations, r.milliseconds);
        if (r.success){
            avg_it += r.iterations;
            avg_time += r.milliseconds;
            num_success++;
        }   
    }
    if (num_success > 0){
        printf("%d/%d successful, Average: %lf iterations, %lf seconds\n", 
        num_success, num_trials, avg_it/(double)num_success, avg_time/(double)num_success / 1000.0 );
    }
    fflush(stdout);
}

FILE * inputFile;
char outputFilename[155];
char temps[105];
int main(){

    // Read in boards
    inputFile = fopen(fileName, "r");
    for (int _ = 0; _ < maxBoardsScanned; _++){
        char inputStr[105];
        while (true){
            fscanf(inputFile,"%[^\n]%*c", inputStr);
            if (inputStr[0] != '#'){
                break;
            }
        }
        if (inputStr[0] == 0){
            boardsScanned = _;
            break;
        }
        // printf("%s\n", inputStr);
        FOR2(
            if (inputStr[i * n + j] == '.'){
                allBoards[_][i][j] = -1;
            }
            else{
                allBoards[_][i][j] = inputStr[i*n+j] - '1';
            }
        )
    }
    // doTrials();
    // return 0;
    printf("Enter output file name, board, and seed: ");
    int boardNum;
    int seed;
    scanf("%s %d %d", temps, &seed, &boardNum);
    sprintf(outputFilename,"sudokuVis/inputs/%s.txt",temps);
    if (boardNum >= boardsScanned){
        printf("bro don't mess with the program like that\n");
        return 0;
    }
    else{
        FILE * anotherVariableNameForAFile = fopen(outputFilename,"w");
        setConstraintBoard(boardNum);
        Result r = doBoard(seed,true, anotherVariableNameForAFile);
        printf("%d %d %lld\n", r.success, r.iterations, r.milliseconds);
        fclose(anotherVariableNameForAFile);
        fflush(stdout);
    }




}
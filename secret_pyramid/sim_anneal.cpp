#include <cstdio>
#include <cstdlib>
#include <algorithm>
using namespace std;

int n;
float fn;
float THREE = 3e1;
bool a[100][100];
int c[100][2];
int d[200];
int cc[100][2];
int dc[200];

double temp = 1;
int dist;
int bdist;

int total = 0;

float fact[3] = {0,0,100};

bool best[100][100];

void copybest(){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            best[i][j] = a[i][j];
        }
    }
}

bool accept(int t, float p){
    if (t >= 2) return true;
    
    float prob = (fact[t] * temp * temp + p * temp * temp*5) * 50000;
    prob = min(prob,(float)99999);
    total += rand() % 100000 <= (int)prob;
    return (rand() % 100000 <= (int)prob);
    return false;
}

int main(){
    // freopen("test.in", "r", stdin);
    scanf("%d", &n);
    srand(1094);
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            a[i][j] = 1;
            best[i][j] = 1;
        }
    }
    fn = (float)n;
    for (int i = 0; i < n; i++){
        scanf("%d", &c[i][0]);
        dist += c[i][0];
    }
    for (int i = 0; i < n; i++){
        scanf("%d", &c[i][1]);
        dist += c[i][1];
    }
    for (int i = 0; i < 2*n-1; i++){
        scanf("%d", &d[i]);
        dist += d[i];
    }
    bdist = dist;
    while (temp > 0){
        int x = rand() % n;
        int y = rand() % n;
        if (c[x][0] == 0 || c[y][1] == 0 || d[x-y+n-1] == 0){
            temp -= 1/(5e6);
            continue;
        }    
        if (a[x][y]){
            int t = (cc[x][0] < c[x][0]);
            t += (cc[y][1] < c[y][1]);
            t += (dc[x-y+n-1] < d[x-y+n-1]);
            float p = ((float)c[x][0]/fn);
            p += (float)c[y][1]/fn;
            p += (float)d[x-y+n-1]/fn;
            p /= 3.0;

            // printf("%d %d %f\n",x,y,p);
            if (accept(t,p)){
                a[x][y] = 0;
                cc[x][0] ++;
                cc[y][1] ++;
                dc[x-y+n-1] ++;
                
                dist += 3 - 2*t;
            }
        }
        else{
            int t = (cc[x][0] > c[x][0]);
            t += (cc[y][1] > c[y][1]);
            t += (dc[x-y+n-1] > d[x-y+n-1]);
            float p = ((float)c[x][0]/fn);
            p += (float)c[y][1]/fn;
            p += (float)d[x-y+n-1]/fn;
            p /= 3.0;
            p = 1 - p;
            // printf("%f\n", p);
            if (accept(t,p)){
                a[x][y] = 1;
                cc[x][0] --;
                cc[y][1] --;
                dc[x-y+n-1] --;
                dist += 3 - 2*t;
            }
        }

        if (dist < bdist){
            bdist = dist;
            copybest();
            if (dist == 0) break;
        }

        temp -= 1/(5e6); //ten mil iterations?
    }
    for (int y = 0; y < n; y++){
        for (int x = 0; x < n; x++){
            printf("%d", best[x][y]);
        }
        printf("\n");
    }
    // printf("%d\n", total);
}   
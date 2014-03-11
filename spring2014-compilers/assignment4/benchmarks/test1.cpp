
#define N 10000

int loop(int a, int b){
    int i;
    int inv;
    int c[N][N];
    for(i=0; i<N; i++){
        for(j=0; j<N; j++){
            inv = a * b;
            c[i][j] = i * j * inv;
        }
    }
    return c[3][3];
}


void main(){
    int a = loop(34,45);

}

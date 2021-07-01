#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <chrono>
using namespace std;
typedef struct Mat {
    double *m;
    int sz;
} Matrix;

#define MR(mt, i, j) mt->m[i * mt->sz + j]

double matrix_multiply(Matrix* a, Matrix* b, Matrix* c) {
    assert(a->sz == b->sz);
    int sz = a->sz;
    chrono::system_clock::time_point start, end;

    start = chrono::system_clock::now();
    for(int i = 0 ; i < sz ; ++i) {
        for(int j = 0 ; j < sz ; ++j) {
            for(int k = 0 ; k < sz ; ++k) {
                MR(c, i, j) += MR(a, i, k) * MR(b, k, j);
            }
        }
    }
    end = chrono::system_clock::now();

    double time = static_cast<double>(chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0);
    return time;
}
double cache_blocking(Matrix* a, Matrix* b, Matrix* c) {
    assert(a->sz == b->sz);
    int sz = a->sz;
    int BLOCK = 32;
    chrono::system_clock::time_point start, end;
    
    start = chrono::system_clock::now();
    for(int i = 0 ; i < sz ; i+=BLOCK)
    for(int j = 0 ; j < sz ; j+=BLOCK)
    for(int k = 0 ; k < sz ; k+=BLOCK)
        for(int ii = i ; ii < i + BLOCK ; ++ii) {
            for(int jj = j ; jj < j + BLOCK ; ++jj) {
                for(int kk = k ; kk < k + BLOCK ; ++kk) {
                    MR(c, ii, jj) += MR(a, ii, kk) * MR(b, kk, jj);
                }
            }
        }
    end = chrono::system_clock::now();

    double time = static_cast<double>(chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0);
    return time;
}
int newMat(Matrix* Mat, int sz) {
    Mat->sz = sz;
    Mat->m = NULL;
    Mat->m = (double*)calloc(sz*sz, sizeof(double));
    return (Mat->m != NULL) ? 1 : 0;
}
void mat_set(Matrix* Mat, double val) {
    int sz = Mat->sz;
    for(int i = 0 ; i < sz ; ++i) {
        for(int j = 0 ; j < sz ; ++j) {
            MR(Mat, i, j) = val + (i+j)%2;
        }
    }
}
int main() {
    Matrix a, b, c;
    int sz = 512;
    int val = 99999;
    newMat(&a, sz);
    newMat(&b, sz);
    newMat(&c, sz);
    mat_set(&a, val);
    mat_set(&b, val);
    printf("no blocking: %.3lf ms\n", matrix_multiply(&a, &b, &c));
    printf("blocking: %.3lf ms\n", cache_blocking(&a, &b, &c));
}
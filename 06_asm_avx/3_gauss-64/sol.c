#include <stdio.h>
#include <immintrin.h>

void Sol(int n, double* matrix) {
    for (int i = 0; i < n; i++) {
        __m256d head = _mm256_broadcast_sd(matrix+i*(n+1)+i);
        for (int j = 0; j < n/4; j+=4) {
            __m256d operand = _mm256_loadu_pd(matrix+i*(n+1)+j);
            operand = _mm256_div_pd(operand, head);
            _mm256_storeu_pd(matrix+i*(n+1)+j, operand);
        }
        double head2 = matrix[i*(n+1)+i];
        for (int j = 0; j <= n%4; j++) {
            matrix[i*(n+1)+j]/=head2;
        }                
        /////////                                    
        for (int j = i+1; j < n; j++) {
            __m256d head = _mm256_broadcast_sd(matrix+j*(n+1)+i);
            for (int k = 0; k <n/4; k+=4) {
                __m256d el = _mm256_loadu_pd(matrix+(i)*(n+1)+k);
                __m256d res = _mm256_loadu_pd(matrix+j*(n+1)+k);
                el = _mm256_mul_pd(el, head);
                res = _mm256_sub_pd(res, el);
                _mm256_storeu_pd(matrix+j*(n+1)+k, res);
            }
            double head2 = matrix[(j)*(n+1)+i];
            for (int k = 0; k <=n%4; k++) {
                matrix[j*(n+1)+k]-=matrix[(i)*(n+1)+k]*head2;
            }
        }
    }
    for (int i = n-1; i>=0 ; i--) {
        for (int j = 0; j < i; j++) {
            matrix[j*(n+1)+n]-=matrix[i*(n+1)+n]*matrix[j*(n+1)+i];
        }
    }
}

void Out(int n, double* matrix) {
    for (int i = 0; i < n; i++) {
        printf("%lf\n", matrix[(i+1)*(n+1)-1]);
    }
}

double* In(int n) {
    double* matrix = (double*) malloc((n*n+n)*sizeof(double));
    for (int i = 0; i < n * n + n; i++) {
        scanf("%lf", &matrix[i]);
    }
    return matrix;
}

int main() {
    int n;
    scanf("%d", &n);
    double* matrix = In(n);
    Sol(n, matrix);
    Out(n, matrix);
    free(matrix);
    return 0;
}

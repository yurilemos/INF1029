#include <stdio.h>
#include <stdlib.h>
#include "matrix_lib.h"



int scalar_matrix_mult(float scalar_value, Matrix* matrix){
	long unsigned int i, h = matrix->height, w = matrix->width;

	if(matrix == NULL) return 0;

  	__m256 vec_scalar = _mm256_set1_ps(scalar_value); 
 	
 	float *nxt_rows = matrix->rows;
 	
 	for(i=0; i<h*w; i+=8, nxt_rows+=8){
 		__m256 vec_rows = _mm256_load_ps(nxt_rows);	
 		__m256 vec_result = _mm256_mul_ps(vec_scalar, vec_rows);
 		_mm256_store_ps(nxt_rows, vec_result);
	}	

	return 1;

};

int matrix_matrix_mult(Matrix *matrixA, Matrix *matrixB, Matrix *matrixC){

    if(matrixA == NULL || matrixB == NULL|| matrixC == NULL) return 0;

	long unsigned int i,j,k;
	long unsigned int h_a = matrixA->height , w_a = matrixA->width;;
	long unsigned int h_b = matrixB->height, w_b = matrixB->width;
	long unsigned int h_c = matrixC->height, w_c = matrixC->width;


	
	if(h_a*w_a != w_b*h_b){
		printf("O numero de colunas da matrizA deve ser igual ao numero de linhas da matrizB.");
		return 0;
	}
		
	float *nxt_rowA = matrixA->rows;
	float *nxt_rowB = matrixB->rows;
	float *nxt_rowC = matrixC->rows;
	
	__m256 vec_c = _mm256_setzero_ps();
	for(i=0; i<w_a*h_a; i++){
		if(i%w_a == 0)
			nxt_rowB = matrixB->rows;

		__m256 vec_aij = _mm256_set1_ps(nxt_rowA[i]);
		nxt_rowC = matrixC->rows + w_b * (i/w_a);

		for(j=0; j<w_b; j+=8, nxt_rowB+=8,nxt_rowC+=8){
			__m256 vec_b = _mm256_load_ps(nxt_rowB);
			__m256 vec_c = _mm256_load_ps(nxt_rowC);
			__m256 result = _mm256_fmadd_ps(vec_aij,vec_b,vec_c);

			_mm256_store_ps(nxt_rowC, result);
		}
	}

	return 1;
}

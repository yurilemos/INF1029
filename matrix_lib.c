#include <stdio.h>
#include <stdlib.h>
#include "matrix_lib.h"



int scalar_matrix_mult(float scalar_value, Matrix* matrix){
	unsigned long int m = matrix->height, n = matrix->width;
	int i=0,j=0;

	if((m%8!=0)||(n%8!=0)){
		return 0;
	}

	for(i=0;i<m; i++){
		for(j=0; j<n; j++){
			matrix->rows[i*n + j] *= scalar_value;
		}
	}
	return 1;

};

int matrix_matrix_mult(Matrix *matrixA, Matrix *matrixB, Matrix *matrixC){
	unsigned long int m = matrixA->height, q = matrixB->width, n = matrixA->width;
	float multiplicacao;
	if((m%8!=0)||(q%8!=0)||(n%8!=0)||(matrixA->width!=matrixB->height)){
		return 0;
	}

    for(int i=0; i<m; i++){
        for(int k=0;k<q;k++){
            for(int j=0; j<n; j++){
            	multiplicacao = matrixA->rows[i*matrixA->width + j] * matrixB->rows[j*matrixB->width + k];
            	matrixC->rows[i*matrixC->width + k] += multiplicacao;                            
            }
        }
        
    }

    return 1;
}

void fill_matrix(Matrix *matrix){
	unsigned long int m = matrix->height, n = matrix->width;
	int i=0,j=0;
	
	for(i=0;i<m; i++){
		for(j=0; j<n; j++){
			matrix->rows[i*n + j] = 0.0;
		}
	}
}

void mostra_matrix(Matrix *matrix){

	unsigned long int m = matrix->height, n = matrix->width;
	int i=0,j=0;

	printf("[ ");
	for(i=0;i<m; i++){
		for(j=0; j<n; j++){
			printf(" %f ",matrix->rows[i*n + j]);
		}
		printf("\n");
	}
	printf("]\n");

}
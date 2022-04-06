// Yuri Lemos 1610893
// Antônio Mesquita 2210269


#include <stdio.h>
#include <stdlib.h>
#include "matrix_lib.h"



int scalar_matrix_mult(float scalar_value, struct matrix* matrix){
	unsigned long int altura = matrix->height, largura = matrix->width;
	int i=0,j=0;

	if((altura%8!=0)||(largura%8!=0)){
		return 0;
	}

	for(i=0;i<altura; i++){
		for(j=0; j<largura; j++){
			matrix->rows[i*largura + j] *= scalar_value;
		}
	}
	return 1;

};

int matrix_matrix_mult(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC){
	unsigned long int alturaA = matrixA->height, larguraA = matrixA->width;
	unsigned long int alturaB = matrixB->height, larguraB = matrixB->width;
	float multiplicacao;
	if((larguraA!=alturaB)||(alturaA%8!=0)||(larguraB%8!=0)||(larguraA%8!=0)){
		return 0;
	}

    for(int i=0; i<alturaA; i++){
        for(int k=0;k<larguraB;k++){
            for(int j=0; j<larguraA; j++){
            	multiplicacao = matrixA->rows[i*larguraA + j] * matrixB->rows[j*larguraB + k];
            	matrixC->rows[i*matrixC->width + k] += multiplicacao;                            
            }
        }
        
    }

    return 1;
}

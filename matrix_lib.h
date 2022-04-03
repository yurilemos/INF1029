typedef struct matrix {
	unsigned long int height;
	unsigned long int width;
	float *rows;
} Matrix;


int scalar_matrix_mult(float scalar_value, Matrix *matrix);

int matrix_matrix_mult(Matrix *a, Matrix *b, Matrix *c);

void fill_matrix(Matrix *matrix);

void mostra_matrix(Matrix *matrix);
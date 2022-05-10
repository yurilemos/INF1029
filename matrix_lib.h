struct matrix {
	unsigned long int height;
	unsigned long int width;
	float *rows;
};

struct scalar_thread_args {
   int begin;
   int end;
   float scalar;
   struct matrix * matrix;
};

struct matrix_matrix_thread_args
{
	int begin;
	int end;
	struct matrix *A;
	struct matrix *B;
	struct matrix *C;
};

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c);

void set_number_threads(int n_threads);

unsigned long int get_number_threads(void);

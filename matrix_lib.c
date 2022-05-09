#include <immintrin.h>
#include <pthread.h>

#define VECTOR_SIZE 8

int global_num_thread;

struct matrix
{
	unsigned long int height;
	unsigned long int width;
	float *rows;
};

struct scalar_thread_args
{
	int begin;
	int end;
	float scalar;
	struct matrix *matrix;
};

struct matrix_matrix_thread_args
{
	int begin;
	int end;
	struct matrix *A;
	struct matrix *B;
	struct matrix *C;
};


void set_number_threads(int n_threads)
{
	if (n_threads > 0)
	{
		global_num_thread = n_threads;
	}
	else
	{
		global_num_thread = 1;
	}
}

int get_number_threads(void)
{
	return global_num_thread;
}

void *scalar_matrix_mult_avx_thread(void *args)
{
	unsigned long int i;
	struct scalar_thread_args *thread_args;
	thread_args = (struct scalar_thread_args *)args;

	/* Initialize the scalar vector with the scalar value */
	__m256 vec_scalar_value = _mm256_set1_ps(thread_args->scalar);

	/* Compute the product between the scalar value and the elements of the matrix */
	float *nxt_matrix_a = &thread_args->matrix->rows[thread_args->begin];
	float *nxt_result = &thread_args->matrix->rows[thread_args->begin];

	for (i = thread_args->begin;
		 i < thread_args->end;
		 i += 8, nxt_matrix_a += 8, nxt_result += 8)
	{
		/* Initialize the three argument vectors */
		__m256 vec_matrix_a = _mm256_load_ps(nxt_matrix_a);

		/* Compute the expression res = a * b + c between the three vectors */
		__m256 vec_result = _mm256_mul_ps(vec_scalar_value, vec_matrix_a);

		/* Store the elements of the result vector */
		_mm256_store_ps(nxt_result, vec_result);
	}

	pthread_exit(NULL);
}

int scalar_matrix_mult(float scalar_value, struct matrix *matrix)
{
	pthread_t threads[global_num_thread];
	pthread_attr_t attr;
	struct scalar_thread_args thread_args[global_num_thread];
	int matrix_size = matrix->height * matrix->width, result, i;
	void *status;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (matrix == NULL)
	{
		return 0;
	}

	for (i = 0; i < global_num_thread; i++)
	{
		thread_args[i].begin = (matrix_size / global_num_thread) * i;
		thread_args[i].end = (matrix_size / global_num_thread) + thread_args[i].begin;
		thread_args[i].matrix = matrix;
		thread_args[i].scalar = scalar_value;

		result = pthread_create(&threads[i], NULL, scalar_matrix_mult_avx_thread, (void *)&thread_args[i]);
		if (result)
		{
			exit(-1);
		}
	}

	pthread_attr_destroy(&attr);
	for (i = 0; i < global_num_thread; i++)
	{
		result = pthread_join(threads[i], &status);
		if (result)
		{
			exit(-1);
		}
	}

	return 1;
}

void *matrix_matrix_mult_avx_thread(void *args)
{
	unsigned long int i, j, k;
	float *nxt_a;
	float *nxt_b;
	float *nxt_c;

	struct matrix_matrix_thread_args *thread_args;
	thread_args = (struct matrix_matrix_thread_args *)args;

	for (i = thread_args->begin, nxt_a = &thread_args->A->rows[thread_args->begin];
		 i < thread_args->end;
		 i += 1)
	{
		/* Set nxt_b to the begining of matrixB */
		nxt_b = thread_args->B->rows;

		for (j = 0;
			 j < thread_args->A->width;
			 j += 1, nxt_a += 1)
		{
			/* Initialize the scalar vector with the next scalar value */
			__m256 vec_a = _mm256_set1_ps(*nxt_a);

			/* 
		 * Compute the product between the scalar vector and the elements of 
		 * a row of matrixB, 8 elements at a time, and add the result to the 
		 * respective elements of a row of matrixC, 8 elements at a time.
		 */
			for (k = 0, nxt_c = &thread_args->C->rows[thread_args->begin] + (thread_args->C->width * i);
				 k < thread_args->B->width;
				 k += VECTOR_SIZE, nxt_b += VECTOR_SIZE, nxt_c += VECTOR_SIZE)
			{
				/* Load part of b row (size of vector) */
				__m256 vec_b = _mm256_load_ps(nxt_b);

				/* Initialize vector c with zero or load part of c row (size of vector) */
				__m256 vec_c;

				if (j == 0)
				{ /* if vec_a is the first scalar vector, vec_c is set to zero */
					vec_c = _mm256_setzero_ps();
				}
				else
				{ /* otherwise, load part of c row (size of vector) to vec_c */
					vec_c = _mm256_load_ps(nxt_c);
				}

				/* Compute the expression res = a * b + c between the three vectors */
				vec_c = _mm256_fmadd_ps(vec_a, vec_b, vec_c);

				/* Store the elements of the result vector */
				_mm256_store_ps(nxt_c, vec_c);
			}
		}
	}

	pthread_exit(NULL);
}

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c)
{
	pthread_t threads[global_num_thread];
	pthread_attr_t attr;
	struct matrix_matrix_thread_args thread_args[global_num_thread];
	int matrix_size = c->height * c->width, result;
	void *status;

	unsigned long int NA, NB, NC, i;
	
	/* Check the numbers of the elements of the matrix */
	NA = a->height * a->width;
	NB = b->height * b->width;
	NC = c->height * c->width;

	/* Check the integrity of the matrix */
	if ((NA == 0 || a->rows == NULL) ||
		(NB == 0 || b->rows == NULL) ||
		(NC == 0 || c->rows == NULL))
		return 0;

	/* Check if we can execute de product of matrix A and matrib B */
	if ((a->width != b->height) ||
		(c->height != a->height) ||
		(c->width != b->width))
		return 0;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < global_num_thread; i++)
	{
		thread_args[i].begin = (matrix_size / global_num_thread) * i;
		thread_args[i].end = (matrix_size / global_num_thread) + thread_args[i].begin;
		thread_args[i].A = a;
		thread_args[i].B = b;
		thread_args[i].C = c;

		result = pthread_create(&threads[i], NULL, matrix_matrix_mult_avx_thread, (void *)&thread_args[i]);
		if (result)
		{
			exit(-1);
		}
	}

	pthread_attr_destroy(&attr);
	for (i = 0; i < global_num_thread; i++)
	{
		result = pthread_join(threads[i], &status);
		if (result)
		{
			exit(-1);
		}
	}

	return 1;
}
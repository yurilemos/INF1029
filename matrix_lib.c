// Antonio mesquita 2210269
// Yuri Lemos 1610893

#include <immintrin.h>
#include <pthread.h>
#include "matrix_lib.h"

#define VECTOR_SIZE 8

int global_num_thread;

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

unsigned long int get_number_threads(void)
{
	return global_num_thread;
}

void *scalar_matrix_mult_avx_thread(void *args)
{
	struct scalar_thread_args *thread_args;
	thread_args = (struct scalar_thread_args *)args;

  unsigned long int i;

	__m256 vec_scalar_value = _mm256_set1_ps(thread_args->scalar);

	float *nxt_matrix_a = &thread_args->matrix->rows[thread_args->begin];
	float *nxt_resultado = &thread_args->matrix->rows[thread_args->begin];

	for (i = thread_args->begin;
		 i < thread_args->end;
		 i += 8, nxt_matrix_a += 8, nxt_resultado += 8)
	{
		__m256 vec_matrix_a = _mm256_load_ps(nxt_matrix_a);

		__m256 vec_resultado = _mm256_mul_ps(vec_scalar_value, vec_matrix_a);

		_mm256_store_ps(nxt_resultado, vec_resultado);
	}

	pthread_exit(NULL);
}

int scalar_matrix_mult(float scalar_value, struct matrix *matrix)
{
  struct scalar_thread_args thread_args[global_num_thread];
	pthread_t threads[global_num_thread];
	pthread_attr_t attr;
	
	int tam_matrix = matrix->height * matrix->width, resultado, i;
	void *status;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (matrix == NULL)
	{
		return 0;
	}

	for (i = 0; i < global_num_thread; i++)
	{
		thread_args[i].begin = (tam_matrix / global_num_thread) * i;
		thread_args[i].end = (tam_matrix / global_num_thread) + thread_args[i].begin;
		thread_args[i].matrix = matrix;
		thread_args[i].scalar = scalar_value;

		resultado = pthread_create(&threads[i], NULL, scalar_matrix_mult_avx_thread, (void *)&thread_args[i]);
		if (resultado)
		{
			exit(-1);
		}
	}

	pthread_attr_destroy(&attr);
	for (i = 0; i < global_num_thread; i++)
	{
		resultado = pthread_join(threads[i], &status);
		if (resultado)
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

			__m256 vec_a = _mm256_set1_ps(*nxt_a);

			for (k = 0, nxt_c = &thread_args->C->rows[thread_args->begin] + (thread_args->C->width * i);
				 k < thread_args->B->width;
				 k += VECTOR_SIZE, nxt_b += VECTOR_SIZE, nxt_c += VECTOR_SIZE)
			{
				__m256 vec_b = _mm256_load_ps(nxt_b);

				__m256 vec_c;

				if (j == 0)
				{ 
					vec_c = _mm256_setzero_ps();
				}
				else
				{ 
					vec_c = _mm256_load_ps(nxt_c);
				}

				vec_c = _mm256_fmadd_ps(vec_a, vec_b, vec_c);

				_mm256_store_ps(nxt_c, vec_c);
			}
		}
	}

	pthread_exit(NULL);
}

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c)
{
  struct matrix_matrix_thread_args thread_args[global_num_thread];
	pthread_t threads[global_num_thread];
	pthread_attr_t attr;
	
	int tam_matrix = c->height * c->width, resultado;
	void *status;

	unsigned long int NA, NB, NC, i;
	
	NA = a->height * a->width;
	NB = b->height * b->width;
	NC = c->height * c->width;

	if ((NA == 0 || a->rows == NULL) ||
		(NB == 0 || b->rows == NULL) ||
		(NC == 0 || c->rows == NULL) || 
    (a->width != b->height) ||
		(c->height != a->height) ||
		(c->width != b->width))
		return 0;


	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < global_num_thread; i++)
	{
		thread_args[i].begin = (tam_matrix / global_num_thread) * i;
		thread_args[i].end = (tam_matrix / global_num_thread) + thread_args[i].begin;
		thread_args[i].A = a;
		thread_args[i].B = b;
		thread_args[i].C = c;

		resultado = pthread_create(&threads[i], NULL, matrix_matrix_mult_avx_thread, (void *)&thread_args[i]);
		if (resultado)
		{
			exit(-1);
		}
	}

	pthread_attr_destroy(&attr);
	for (i = 0; i < global_num_thread; i++)
	{
		resultado = pthread_join(threads[i], &status);
		if (resultado)
		{
			exit(-1);
		}
	}

	return 1;
}
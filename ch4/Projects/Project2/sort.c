#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "sort.h"

/* argument type for the sorter threads */
typedef struct {
	int *data;
	size_t size; /* size of data */
}s_parameter_t;

/* argument type for the merger thread */
typedef struct {
	int *s1;	/* a pointer to the first subarray */
	int *s2;    /* a pointer to the second subarray */
	int *dst;   /* a pointer to the destination array */
	size_t n;	/* size of s1 */
	size_t m;	/* size of s2 */
}m_parameter_t;

/* comparator function for qsort() */
static int cmpint(const void *arg1, const void *arg2) { return *(int *)arg1 - *(int *)arg2; }

/* thread function for the sorter threads */
static void *sorter(void *arg) {
	s_parameter_t *param = (s_parameter_t *)arg;

	qsort(param->data, param->size, sizeof(int), cmpint);
	pthread_exit(NULL);
}

/* thread function for the merger thread */
static void* merger(void *arg) {
	m_parameter_t *param = (m_parameter_t *)arg;

	size_t i = 0, j = 0;	/* indices in the subarrays */

	/* merge until one or both subarrays is/are exhausted */
	while(i < param->n && j < param->m) {
		if(param->s1[i] <= param->s2[j])
			*param->dst++ = param->s1[i++];
		else
			*param->dst++ = param->s2[j++];
	}

	/* copy the remaining elements */
	while (i < param->n) {
		*param->dst++ = param->s1[i++];
	}

	while (j < param->m) {
		*param->dst++ = param->s2[j++];
	}

	pthread_exit(NULL);
}

int sort(int *src, int *dst, size_t size) {

	if(!src || !dst)
		return -1;

	pthread_t tid[3];
	s_parameter_t s_params[2];
	m_parameter_t m_params;

	size_t n = size>>1;
	size_t m = size - n;

	/* launch the sorter threads */
	s_params[0].size = n;
	s_params[1].size = m;
	for(int i = 0; i < 2; ++i) {
		s_params[i].data = src + i * n;
		if(pthread_create(&tid[i], NULL, sorter, &s_params[i]) < 0)
			return -2;
	}

	/* wait for completion of the threads */
	for(int i = 0; i < 2; ++i) {
		if(pthread_join(tid[i], NULL) < 0)
			return -3;
	}

	/* launch merger thread */
	m_params.s1 = src;
	m_params.s2 = src + n;
	m_params.n = n;
	m_params.m = m;
	m_params.dst = dst;

	if(pthread_create(&tid[2], NULL, merger, &m_params) < 0)
		return -2;

	/* wait for completion of the merger thread */
	if(pthread_join(tid[2], NULL) < 0)
		return -3;

	return 0;
}

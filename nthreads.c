#include <stdio.h>
#include <stdlib.h>
#include "nthreads.h"

#ifdef __unix__
#include <pthread.h>
#else
#include <windows.h>
#endif

#ifdef _PTHREAD_H 

int nthread_start(struct NestThread *nthread, void *startpt(void *arg), void *data)
{
	int res;
	pthread_t *thread;
	thread = (pthread_t*)malloc(sizeof(pthread_t));
	res = pthread_create(thread, NULL, startpt, data);
	nthread->arg1 = (void*)thread;
	if (res != 0)
		free(nthread->arg1);
	return res;		
}

int nthread_join(struct NestThread *nthread)
{
	int res;
	res=pthread_join(*((pthread_t*)nthread->arg1), NULL);
	free(nthread->arg1);
	return res;
}

#else

int nthread_start(struct NestThread *nthread, void *startpt(void *arg), void *data)
{
	HANDLE hthread;
	DWORD *threadid;
	threadid = (DWORD*)malloc(sizeof(DWORD));
	hthread = CreateThread(NULL, 0, startpt, data, 0, threadid);
	if (hthread == NULL)
		free(threadid);
	else {
		nthread->arg1 = (void *)hthread;
		nthread->arg2 = (void *)threadid; 
	}
	return (hthread == NULL)? 1 : 0;		
}

int nthread_join(struct NestThread *nthread)
{
	int res;
	res = (int)WaitForSingleObject((HANDLE)nthread->arg1, INFINITE);
	free(nthread->arg1);
	free(nthread->arg2);
	return res;
}

#endif


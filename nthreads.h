struct NestThread {
	void *arg1, *arg2;
};

int nthread_start(struct NestThread *nthread, void *startpt(void *arg), void *data);
int nthread_join(struct NestThread *nthread);

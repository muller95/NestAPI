struct Figure* makeset(struct Figure *figs, int nfigs, int *setsize);
struct Figure figdup (struct Figure *src);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nmemb, size_t size);
void destrfig(struct Figure *fig);
void destrindiv(struct Individ *indiv);




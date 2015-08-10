#include <stdlib.h>
#include <stdio.h>
#include "figure.h"
#include "nest_structs.h"

struct Figure* makeset(struct Figure *figs, int nfigs, int *setsize);
struct Figure figdup (struct Figure *src);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nmemb, size_t size);
void destrindiv(struct Individ *indiv);

struct Figure* makeset(struct Figure *figs, int nfigs, int *setsize)
{
	int i, j, k, size;
	struct Figure *figset;

	size = 0;
	for (i = 0; i < nfigs; i++)
		size += figs[i].quant;

	figset = (struct Figure*)xmalloc(sizeof(struct Figure) * size);
	*setsize = size;

	for (i = 0, k = 0; i < nfigs; i++) 
		for (j = 0; j < figs[i].quant; j++, k++)
			figset[k] = figdup(&figs[i]);

	return figset;
}

struct Figure figdup(struct Figure *src)
{
	int i, j;
	struct Figure fig;

	fig.id = src->id;
	fig.nprims = src->nprims;
	fig.quant = src->quant;

	fig.corner.x = src->corner.x;
	fig.corner.y = src->corner.y;
	
	fig.gcenter.x = src->gcenter.x;
	fig.gcenter.y = src->gcenter.y;


	fig.prims = (struct Primitive*)xmalloc(sizeof(struct Primitive) * fig.nprims);

	for (i = 0 ; i < fig.nprims; i++) {
		int npts;
		npts = src->prims[i].npts; 
		fig.prims[i].npts = npts;
		fig.prims[i].pts = (struct Point*)xmalloc(sizeof(struct Point) * npts);
		
		for (j = 0; j < npts; j++) {
			fig.prims[i].pts[j].x = src->prims[i].pts[j].x;
			fig.prims[i].pts[j].y = src->prims[i].pts[j].y;
		}
	}
	
	return fig;
}

void destrfig(struct Figure *fig)
{
	int i;

	for (i = 0; i < fig->nprims; i++) {
		fig->prims[i].npts = 0;
		free(fig->prims[i].pts);
	}

	fig->nprims = 0;
	free(fig->prims);
}

void destrindiv(struct Individ *indiv) 
{
	indiv->gensize = 0;
	free(indiv->genom);
}

void *xmalloc(size_t size)
{
	void *data;

	data = malloc(size);

	if (data == NULL) {
		printf("Memory error\n");
		exit(1);
	}

	return data;
}

void *xrealloc(void *ptr, size_t size)
{
	void *data;

	data = realloc(ptr, size);

	if (data == NULL) {
		printf("Memory error\n");
		exit(1);
	}

	return data;
}


void *xcalloc(size_t nmemb, size_t size)
{
	void *data;

	data = calloc(nmemb, size);

	if (data == NULL) {
		printf("Memory error\n");
		exit(1);
	}

	return data;
}

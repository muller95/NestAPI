#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "figure.h"
#include "nest_structs.h"
#include "geometry.h"

struct Figure* makeset(struct Figure *figs, int nfigs, int *setsize);
struct Figure figdup (struct Figure *src);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nmemb, size_t size);
void destrindiv(struct Individ *indiv);
void figinit(struct Figure *fig);
void mtxmult(double mtx[3][3], struct Figure *fig);

void mtxmult(double mtx[3][3], struct Figure *fig)
{
	int i, j, r;
	double nmtx[3][3];

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			nmtx[i][j] = 0.0;
		}
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			for (r = 0; r < 3; r++) {
				nmtx[i][j] += mtx[i][r] * fig->mtx[r][j];
			}
		}
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			fig->mtx[i][j] = nmtx[i][j];
		}
	}
}

void figinit(struct Figure *fig)
{
	int i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			fig->mtx[i][j] = (i == j)? 1.0 : 0.0;
		}
	}

	move_to_zero(fig);
	gcenter(fig);
}

struct Figure* makeset(struct Figure *figs, int nfigs, int *setsize)
{
	int i, j, k, size;
	struct Figure *figset;

	size = 0;
	for (i = 0; i < nfigs; i++)
		size += figs[i].quant;

	figset = (struct Figure*)xmalloc(sizeof(struct Figure) * size);
	*setsize = size;

	for (i = 0, k = 0; i < nfigs; i++) { 
		for (j = 0; j < figs[i].quant; j++, k++) {
			figset[k] = figdup(&figs[i]);
		}
	}

	return figset;
}

struct Figure figdup(struct Figure *src)
{
	int i, j;
	struct Figure fig;

	fig.id = src->id;
	fig.nprims = src->nprims;
	fig.quant = src->quant;
	fig.angstep = src->angstep;

	fig.corner.x = src->corner.x;
	fig.corner.y = src->corner.y;
	
	fig.gcenter.x = src->gcenter.x;
	fig.gcenter.y = src->gcenter.y;

	fig.name = strdup(src->name); 

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

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			fig.mtx[i][j] = src->mtx[i][j];
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include "figure.h"
#include "nest_structs.h"
#include "geometry.h"
#include "gcenter.h"
#include "cmnfuncs.h"
#include "rotnest.h"
#include "cmnnest.h" 

#define FIG_SEPAR ":"
#define PRIM_SEPAR ""

#define STATE_NEWFIG 0
#define STATE_PRIM 1

void trim(char *str)
{
	int i;

	for (i = 0; i < strlen(str); i++) {
		if (iscntrl(str[i])) {
			str[i] = '\0';
		}
	}
}

void pfig(struct Figure fig)
{
	int i, j;
	
	printf("quant=%d\n", fig.quant);
	printf("nprims=%d\n", fig.nprims);
	for (i = 0; i < fig.nprims; i++) {
		printf("npts=%d\n", fig.prims[i].npts);
		for (j = 0; j < fig.prims[i].npts; j++) {
			struct Point pt;
			pt = fig.prims[i].pts[j];
			printf("%lf %lf\n", pt.x, pt.y);
		}
		printf("\n");
	}
}

void pfig2file(struct Figure *figs, int nfigs)
{
	int i, j, k;
	FILE *file;

	if (!(file = fopen("./drawout", "w+"))) {
		printf("error while creating file\n");
		exit(1);
	}
	
	for (i = 0; i < nfigs; i++) {
		for (j = 0; j < figs[i].nprims; j++) {
			for (k = 0; k < figs[i].prims[j].npts - 1; k++) {
				fprintf(file, "%lf\n", figs[i].prims[j].pts[k].x);
				fprintf(file, "%lf\n", figs[i].prims[j].pts[k].y);
				fprintf(file, "%lf\n", figs[i].prims[j].pts[k + 1].x);
				fprintf(file, "%lf\n", figs[i].prims[j].pts[k + 1].y);
			}
		}
	}

	fclose(file);
}

static int figcmp(const void *a, const void *b)
{
	double sqr1, sqr2;
	struct Figure *fig1, *fig2;

	fig1 = (struct Figure*)a;	
	fig2 = (struct Figure*)b;

	sqr1 = fig1->corner.x * fig1->corner.y;
	sqr2 = fig2->corner.x * fig2->corner.y;
	
	if (sqr1 > sqr2)
		return 1;
	else if (sqr1 < sqr2)
		return -1;
	else
		return 0;
}

static int gencmp(const void *a, const void *b) 
{
	struct Individ *indiv1, *indiv2;

	indiv1 = (struct Individ*)a;
	indiv2 = (struct Individ*)b;

	if (indiv1->gensize < indiv2->gensize)
		return 1;
	else if (indiv1->gensize > indiv2->gensize)
		return -1;
	else if (indiv1->gensize == indiv2->gensize && indiv1->height > indiv2->height)
		return 1;
	else if (indiv1->gensize == indiv2->gensize && indiv1->height < indiv2->height)
		return -1;
	else
		return 0;
}

int main(int argc, char **argv)
{
	int maxprims, maxfigs, maxpts, state, maxindivs;
	int nfigs, nprims, npts, quant, setsize;
	double width = 800.0, height = 800.0;
	struct Individ *indivs, tmp;
	struct Figure *figs, *figset;
	char *str;
	ssize_t chread;
	size_t n;
	int i, j, k, m, nindivs;
	struct NestAttrs attrs;
	
	str = NULL;
	n = 0;

	maxfigs = 128;
	state = STATE_NEWFIG;
	nfigs = nprims = npts = 0;
	attrs.logfile = fopen("./logfile", "w+");

	figs = (struct Figure*)xmalloc(sizeof(struct Figure) * maxfigs);

	while ((chread = getline(&str, &n, stdin)) != -1) {
		double x, y;
		trim(str);

		if (state == STATE_NEWFIG) {
			sscanf(str, "%d\n", &quant);
			state = STATE_PRIM;
			
			maxpts = 2048;
			maxprims = 128;

			figs[nfigs].quant = quant;
			figs[nfigs].prims = (struct Primitive*)xmalloc(sizeof(struct Primitive) * maxprims); 
			figs[nfigs].prims[nprims].pts = (struct Point*)xmalloc(sizeof(struct Point) * maxpts);

			free(str);
			str = NULL;
			continue;
		}
		
		if (strcmp(str, FIG_SEPAR) == 0) {
			state = STATE_NEWFIG;
			figs[nfigs].prims[nprims].npts = npts;
			nprims++;
			figs[nfigs].id = nfigs;
			figs[nfigs].nprims = nprims;
			nfigs++;
			
			if (nfigs == maxfigs) {
				maxfigs *= 2;
				figs = (struct Figure*)xrealloc(figs, sizeof(struct Figure) * maxfigs);
			}
			
			nprims = 0;
			npts = 0;

			free(str);
			str = NULL;
			continue;
		}

		if (strcmp(str, PRIM_SEPAR) == 0) {
			figs[nfigs].prims[nprims].npts = npts;
			nprims++;
			if (nprims == maxprims) {
				maxprims *= 2;
				figs[nfigs].prims = (struct Primitive*)xrealloc(figs[nfigs].prims, sizeof(struct Primitive) * maxprims);
			}

			maxpts = 2048;
			figs[nfigs].prims[nprims].pts = (struct Point*)xmalloc(sizeof(struct Point) * maxpts);
			npts = 0;

			free(str);
			str = NULL;
			continue;
		}
		
		sscanf(str, "%lf %lf\n", &x, &y);
		
		figs[nfigs].prims[nprims].pts[npts].x = x;
		figs[nfigs].prims[nprims].pts[npts].y = y;
		npts++;

		if (npts == maxpts) {
			maxprims *= 2;
			figs[nfigs].prims[nprims].pts = (struct Point*)xrealloc(figs[nfigs].prims[nprims].pts, sizeof(struct Point) * maxpts);
		}

		free(str);
		str = NULL;
	}

	for (i = 0; i < nfigs; i++) {
		move_to_zero(&figs[i]);
		gcenter2(&figs[i]);
	}

	attrs.width = width;
	attrs.height = height;
	attrs.angstep = 45.0;
	attrs.type = ROTNEST_DEFAULT;
		
	figset = makeset(figs, nfigs, &setsize);
	qsort(figset, setsize, sizeof(struct Figure), figcmp);

	maxindivs = 1024;
	indivs = (struct Individ*)xmalloc(sizeof(struct Individ) * maxindivs);
	indivs[0].genom = (int*)xmalloc(sizeof(int) * setsize);
	indivs[0].gensize = 0;
	rotnest(figset, setsize, &indivs[0], &attrs);
	
		
	nindivs = 1;	
	printf("\n\n");
	for (i = 0; i < 30; i++) {
		int nnew = 0, equal = 0, oldn;
		
		printf("nindivs=%d\n", nindivs);
		for (j = 0; j < nindivs; j++) {
			printf("ind=%d height=%lf gensize=%d genom: ", i, indivs[j].height, indivs[j].gensize);
			for (k = 0; k < indivs[j].gensize; k++) { 
				printf("%d ", indivs[j].genom[k]);
			}
			printf("\n");
		}

		printf("\n\n");

		oldn = nindivs;
		for (j = 0; j < oldn - 1; j++) {
			struct Individ heirs[2];
			if (indivs[j].gensize == indivs[j + 1].gensize && indivs[j].height != INFINITY && indivs[j + 1].height != INFINITY) {
				crossover(&indivs[j], &indivs[j + 1], &heirs[0], setsize);
				crossover(&indivs[j + 1], &indivs[j], &heirs[1], setsize);
			}
			else {
				break;
			}

			for (k = 0; k < 2; k++) {
				for (m = 0; m < nindivs; m++) {
					equal = gensequal(&heirs[k], &indivs[m]) || gensequal2(&heirs[k], &indivs[m], figset);
					if (equal)
						break;
				}
			

				if (!equal) {
					nnew++;
					rotnest(figset, setsize, &heirs[k], &attrs);
					indivs[nindivs] = heirs[k];
					nindivs++;

					if (nindivs == maxindivs) {
						maxindivs *= 2;
						indivs = (struct Individ*)xrealloc(indivs, sizeof(struct Individ) * maxindivs);
					}
				} else {
					destrindiv(&heirs[k]);
				}
			}
		}
		
	
		
		equal = 0;
		while (nnew == 0) {
			tmp = mutate(&indivs[0], setsize);
			equal = 0;
			for (j = 0; j < nindivs; j++) {
				equal = gensequal(&tmp, &indivs[j]) || gensequal2(&tmp, &indivs[j], figset);
				if (equal)
					break;
			}
			if (!equal) {
				nnew++;
				rotnest(figset, setsize, &tmp, &attrs);
				indivs[nindivs] = tmp;
				nindivs++;

				if (nindivs == maxindivs) {
					maxindivs *= 2;
					indivs = (struct Individ*)xrealloc(indivs, sizeof(struct Individ) * maxindivs);
				}
			} else {
				destrindiv(&tmp);
			}
		}
		qsort(indivs, nindivs, sizeof(struct Individ), gencmp);
	}

	printf("LAST NEST MIN_HEIGHT=%lf\n", indivs[0].height);
	rotnest(figset, setsize, &indivs[0], &attrs);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <NestAPI.h>

#define FIG_SEPAR ":"
#define PRIM_SEPAR ""

#define STATE_NEWFIG 0
#define STATE_PRIM 1

#define INDIVS_PER_ITER 30

struct ThreadData {
	int heirnum;
};

struct NestAttrs attrs;
struct Individ *indivs;
struct Individ heirs[INDIVS_PER_ITER + 1];
struct Figure *figset;
int setsize;

void trim(char *str)
{
	int i;

	for (i = 0; i < strlen(str); i++) {
		if (iscntrl(str[i])) {
			str[i] = '\0';
		}
	}
}

int main(int argc, char **argv)
{
	int maxprims, maxfigs, maxpts, state, namelen;
	int nfigs, nprims, npts;
	double angle;
	struct Figure *figs;
	struct NestMatrix res;
	char str[2048];
	int i, j;

	maxfigs = 128;
	namelen = 2048;
	state = STATE_NEWFIG;
	nfigs = nprims = npts = 0;
	figs = (struct Figure*)xmalloc(sizeof(struct Figure) * maxfigs);
	//printf("started writing\n");
	while (fgets(str, 2048, stdin)) {
		double x, y;
		trim(str);

		if (state == STATE_NEWFIG) {
			int quant, angstep;
			char name[2048];
			figs[nfigs].name = (char*)xmalloc(sizeof(char) * namelen);
			sscanf(str, "%s %d %d\n", figs[nfigs].name, &figs[nfigs].quant, &figs[nfigs].angstep);
			sscanf(str, "%s %d %d\n", name, &quant, &angstep);
			state = STATE_PRIM;
			
			maxpts = 2048;
			maxprims = 128;

			figs[nfigs].prims = (struct Primitive*)xmalloc(sizeof(struct Primitive) * maxprims); 
			figs[nfigs].prims[nprims].pts = (struct Point*)xmalloc(sizeof(struct Point) * maxpts);

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

			continue;
		}
		
		sscanf(str, "%lf %lf\n", &x, &y);
		
		figs[nfigs].prims[nprims].pts[npts].x = x;
		figs[nfigs].prims[nprims].pts[npts].y = y;
		npts++;

		if (npts == maxpts) {
			maxpts *= 2;
			figs[nfigs].prims[nprims].pts = (struct Point*)xrealloc(figs[nfigs].prims[nprims].pts, sizeof(struct Point) * maxpts);
		}
	}

	for (i = 0; i < nfigs; i++) {
		figinit(&figs[i]);
	}
	
	for (angle = 0.0; angle < 360; angle += figs[0].angstep) {
		rotate(&figs[0], figs[0].angstep);
		res = approxfig(&figs[0], 0);
		printf("%d %d\n", res.w, res.h);
		for (i = 0; i < res.h; i++) {
			for (j = 0; j < res.w; j++) 
				printf("%d", res.mtx[j][i]);
			printf("\n");
		}
	}
	
	return 0;
}

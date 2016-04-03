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
int setsize, bound, resize;

void trim(char *str)
{
	int i;

	for (i = 0; i < strlen(str); i++) {
		if (iscntrl(str[i])) {
			str[i] = '\0';
		}
	}
}


void ppos2file(char *path, struct Position *posits, int npos)
{
	int i, j, k;
	FILE *file;

	if (!(file = fopen(path, "w+"))) {
		//printf("error while creating file\n");
		exit(1);
	}
	
	for (i = 0; i < npos; i++) {
		fprintf(file, "%lf\n", posits[i].fig.gcenter.x + posits[i].x);
		fprintf(file, "%lf\n", posits[i].fig.gcenter.y + posits[i].y);
		fprintf(file, "%lf\n", posits[i].fig.gcenter.x + posits[i].x);
		fprintf(file, "%lf\n", posits[i].fig.gcenter.y + posits[i].y);
		for (j = 0; j < posits[i].fig.nprims; j++) {
			for (k = 0; k < posits[i].fig.prims[j].npts - 1; k++) {
				fprintf(file, "%lf\n", posits[i].fig.prims[j].pts[k].x + posits[i].x);
				fprintf(file, "%lf\n", posits[i].fig.prims[j].pts[k].y + posits[i].y);
				fprintf(file, "%lf\n", posits[i].fig.prims[j].pts[k + 1].x + posits[i].x);
				fprintf(file, "%lf\n", posits[i].fig.prims[j].pts[k + 1].y + posits[i].y);
			}
		}
	}

	fclose(file);
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
	
	if (sqr1 < sqr2)
		return 1;
	else if (sqr1 > sqr2)
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

void *thrdfunc(void *d)
{
	struct ThreadData *data;
	data = (struct ThreadData*)d;
	
	fprintf(stderr, "In thread heirnum=%d\n", data->heirnum);
	mtxnest(figset, setsize, bound, resize, &heirs[data->heirnum], &attrs);
	fflush(stderr);
	free(data);
	
	return NULL;
}

int main(int argc, char **argv)
{
	int maxprims, maxfigs, maxpts, state, maxindivs, namelen;
	int nfigs, nprims, npts, iters;
	int ext, nested, need, type, check;
	struct Figure *figs;
	char str[2048];
	struct NestThread thrds[INDIVS_PER_ITER + 1];
	int i, j, k, m, nindivs;

	maxfigs = 128;
	namelen = 2048;
	state = STATE_NEWFIG;
	nfigs = nprims = npts = 0;
	figs = (struct Figure*)xmalloc(sizeof(struct Figure) * maxfigs);
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
	
	attrs.width = atof(argv[1]);
	attrs.height = atof(argv[2]);
	iters = atoi(argv[3]);
	type = atoi(argv[4]);
	check = atoi(argv[5]);
	bound = atoi(argv[6]);
	resize = atoi(argv[7]);
	
	if (type) 
		attrs.type = MTXNEST_DEFAULT;
	else
		attrs.type = MTXNEST_FULL;
	
	
	attrs.checker = CHECK_HEIGHT;
	if (check == 1)
		attrs.checker = CHECK_RADIUS;
	else if (check == 2);
		attrs.checker = CHECK_SCALE;
	attrs.logfile = stderr;

		
	fprintf(stderr, "iters=%d\n", iters);
	fflush(stderr);
	figset = makeset(figs, nfigs, &setsize);
	need = setsize;
	nested  = 0;
	
	while (nested < need) {
	
		qsort(figset, setsize, sizeof(struct Figure), figcmp);

		maxindivs = 1024;
		indivs = (struct Individ*)xmalloc(sizeof(struct Individ) * maxindivs);
		indivs[0].genom = (int*)xmalloc(sizeof(int) * setsize);
		indivs[0].gensize = 0;

		fprintf(stderr, "start first nest\n");
		fflush(stderr);
		mtxnest(figset, setsize, bound, resize, &indivs[0], &attrs);
						
		nindivs = 1;		
		ext = 0;
		for (i = 0; i < iters && !ext; i++) {
			int nnew = 0, equal = 0, oldn;
				
			fprintf(stderr, "nindivs=%d\n", nindivs);
			for (j = 0; j < 1; j++) {
				fprintf(stderr, "ind=%d height=%lf gensize=%d genom: ", i, indivs[j].height, indivs[j].gensize);
				for (k = 0; k < indivs[j].gensize; k++) { 
					fprintf(stderr, "%d ", indivs[j].genom[k]);
				}
				fprintf(stderr, "\n");
			}

			fprintf(stderr, "\n");
			fflush(stderr);

			oldn = nindivs;
			for (j = 0; j < oldn - 1 && nnew < INDIVS_PER_ITER; j++) {
				struct Individ tmp[2];
				if (indivs[j].gensize == indivs[j + 1].gensize) {
					int res;
					res = crossover(&indivs[j], &indivs[j + 1], &tmp[0], setsize);
					crossover(&indivs[j + 1], &indivs[j], &tmp[1], setsize);
		
					if (res < 0) {
						break;
					}
				} else {
					break;
				}

				for (k = 0; k < 2; k++) {
					for (m = 0; m < nindivs; m++) {
						equal = gensequal(&tmp[k], &indivs[m]) || gensequal2(&tmp[k], &indivs[m], figset);
						if (equal) {
							break;
						}
					}
						

					if (!equal) {
						struct ThreadData *data;
						data = (struct ThreadData*)xmalloc(sizeof(struct ThreadData));
						data->heirnum = nnew;
						heirs[nnew] = tmp[k];
						fprintf(stderr, "k=%d data->heirnum=%d\n", k, data->heirnum);

						if (nthread_start(&thrds[nnew], thrdfunc, data) != 0) {
							perror("Error creating thread\n");
							exit(1);
						}

						nnew++;
						} else {
							destrindiv(&tmp[k]);
						}
					}
				}
				
				
				equal = 0;
				for (k = 0; k < 100000 && nnew == 0; k++) {
					int res;
					res = mutate(&indivs[0], &heirs[0], setsize);
					if (res < 0) {
						ext = 1;
						break;
					}
					equal = 0;
					for (j = 0; j < nindivs; j++) {
						equal = gensequal(&heirs[0], &indivs[j]) || gensequal2(&heirs[0], &indivs[j], figset);
						if (equal) {
							break;
						}
					}
					if (!equal) {
						struct ThreadData *data;
						data = (struct ThreadData*)xmalloc(sizeof(struct ThreadData));
						data->heirnum = 0;
						if (nthread_start(&thrds[nnew], thrdfunc, data) != 0) {
							perror("Error creating thread\n");
							exit(1);
						}
						nnew++;
					} else {
						destrindiv(&heirs[0]);
					}
				}

				fprintf(stderr, "\nnnew=%d\n", nnew);
				fflush(stderr); 
				for (j = 0; j < nnew; j++) {
					nthread_join(&thrds[j]);
					fprintf(stderr, "%d done\n", j);
					fflush(stderr);
					indivs[nindivs] = heirs[j];
					nindivs++;

					if (nindivs == maxindivs) {
						maxindivs *= 2;
						indivs = (struct Individ*)xrealloc(indivs, sizeof(struct Individ) * maxindivs);
					}
				}
				fprintf(stderr, "\n");
				fflush(stderr);
				qsort(indivs, nindivs, sizeof(struct Individ), gencmp);
		}
		


		for (i = 0; i < indivs[0].npos; i++) {
			double a, b, c, d, e, f;
			a = indivs[0].posits[i].fig.mtx[0][0];
			b = indivs[0].posits[i].fig.mtx[1][0];
			c = indivs[0].posits[i].fig.mtx[0][1];
			d = indivs[0].posits[i].fig.mtx[1][1];
			e = indivs[0].posits[i].fig.mtx[0][2];
			f = indivs[0].posits[i].fig.mtx[1][2];

			printf("%s\n", indivs[0].posits[i].fig.name);
			printf("matrix(%lf, %lf, %lf, %lf, %lf, %lf)\n:\n", a, b, c, d, e, f);
		}
		printf("-\n");
		
		for (i = 0, j = 0; i < setsize; i++) {
			int found = 0;
			for (k = 0; k < indivs[0].gensize; k++) {
				if (i == indivs[0].genom[k]) {
					found = 1;
					break;
				}
			}
			
				if (!found && i != j) {
				destrfig(&figset[j]);
				figset[j] = figset[i];
				j++;		
			} else if (!found) {
				j++;
			}
		}
		
		nested += indivs[0].gensize;
		setsize = need - nested;
	}
	fflush(stdout);
	
	return 0;
}

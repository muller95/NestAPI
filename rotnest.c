#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "figure.h"
#include "geometry.h"
#include "nest_structs.h"
#include "cmnfuncs.h"
#include "cmnnest.h"

#define ROTNEST_DEFAULT 0
#define ROTNEST_MORE	1
#define ROTNEST_FULL 	2


static int placefig2(struct Figure *figset, int fignum, struct Position *posits, int npos, double *minang, double width, double height, double angstep) 
{
	int placed = 0;
	double angle;
	double x, ypos, xpos;
	struct Figure currfig;

	for (angle = 0.0; angle < 360; angle += angstep) {
		currfig = figdup(&figset[fignum]);
		rotate(&currfig, angle);

		for (x = 0; x < width - currfig.corner.x; x += 1.0) {
			xpos = x;
			ypos = getstart(posits, npos, &currfig, x);
			ymove(&xpos, &ypos,	&currfig, posits, npos);
				
			if (checkpos(&currfig, &posits[npos], npos, xpos, ypos, height, width, &placed))
				*minang = angle;
			
			if (ypos == 0)
				break; 			
				
		}
		destrfig(&currfig);
	}

	return placed;
}

static int placefig1(struct Figure *figset, int fignum, struct Position *posits, int npos, double *minang, double width, double height, double angstep) 
{
	int placed = 0, count, i;
	double angle;
	double x, ypos, *xpos;
	struct Figure currfig;
	
	xpos = (double*)xcalloc((int)width, sizeof(double));	
	
	for (angle = 0.0; angle < 360; angle += angstep) {
		ypos = height;
		count = 0;
		currfig = figdup(&figset[fignum]);
		rotate(&currfig, angle);
		for (x = 0; x < width - currfig.corner.x; x += 1.0) {
			double ytmp;
			ytmp = getstart(posits, npos, &currfig, x);
			if (ytmp < ypos) {
				ypos = ytmp;
				count = 0;
			}	

			if (ytmp == ypos) {
				xpos[count] = x;
				count++;
			}
		}
		
		for (i = 0; i < count; i++) {
			double ytmp;
			ytmp = ypos;
			ymove(&xpos[i], &ytmp,	&currfig, posits, npos);

			if (checkpos(&currfig, &posits[npos], npos, xpos[i], ytmp, height, width, &placed))
				*minang = angle;
		}
				
		destrfig(&currfig);
	}

	free(xpos);

	return placed;

}
 
static int placefig0(struct Figure *figset, int fignum, struct Position *posits, int npos, double *minang, double width, double height, double angstep) 
{
	int placed = 0;
	double angle;
	double x, ypos, xpos;
	struct Figure currfig;
//	printf("\n");
	for (angle = 0.0; angle < 360; angle += angstep) {
		ypos = height;
		xpos = 0.0;
		currfig = figdup(&figset[fignum]);
		rotate(&currfig, angle);
		for (x = 0; x < width - currfig.corner.x; x += 1.0) {
			double ytmp;
			ytmp = getstart(posits, npos, &currfig, x);
			if (ytmp < ypos) {
				ypos = ytmp;
				xpos = x;
			}	
		}
		
		ymove(&xpos, &ypos,	&currfig, posits, npos);
		//printf("fignum=%d angle=%lf x=%lf y=%lf corner.y=%lf height=%lf\n", fignum, angle, xpos, ypos, currfig.corner.y, ypos + currfig.corner.y);
		if (checkpos(&currfig, &posits[npos], npos, xpos, ypos, height, width, &placed)) {
			//printf("\n");		
			//printf("prevang=%lf ang=%lf\n", *minang, angle);
			*minang = angle;
		}
				
		destrfig(&currfig);
	}

	//printf("\n");

	return placed;
}

void rotnest(struct Figure *figset, int setsize, struct Individ *indiv, struct NestAttrs *attrs, struct NestResult *out)
{
	int i, j, npos;
	int *mask;
	double minang, tmpheight;
	double width, height, angstep;
	struct Position *posits;
	int (*placefig)(struct Figure *figset, int fignum, struct Position *posits, int npos, double *minang, double width, double height, double angstep);
	FILE *logfile;

	logfile = attrs->logfile;
	width = attrs->width;
	height = attrs->height;
	angstep = attrs->angstep;
	
	placefig = placefig0;
	if (attrs->type == ROTNEST_MORE) {
		placefig = placefig1;
	}
	else if (attrs->type == ROTNEST_FULL) {
		placefig = placefig2;
	}

	mask = (int*)xcalloc(setsize, sizeof(int));
	posits = (struct Position*)xmalloc(sizeof(struct Position) * setsize);
	
	npos = 0;
	
	tmpheight = 0;

	for (i = 0; i < indiv->gensize; i++) {
		int fignum;
		
		fignum = indiv->genom[i];
	
		if (!placefig(figset, fignum, posits, npos, &minang, width, height, angstep)) {
			fprintf(logfile, "fail to position %d\n", fignum);
			continue;
		}


		mask[fignum] = 1;
		npos++;
		
		tmpheight = (tmpheight > posits[npos - 1].fig.corner.y + posits[npos - 1].y)? tmpheight : posits[npos - 1].fig.corner.y + posits[npos - 1].y;
		fprintf(logfile, "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", fignum, npos, minang, tmpheight, posits[npos - 1].x, posits[npos - 1].y);
		sprintf(indiv->nestlog[npos - 1], "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", fignum, npos, minang, tmpheight, posits[npos - 1].x, posits[npos - 1].y);
	}

	if (npos < indiv->gensize) {
		indiv->height = INFINITY;
	//	indiv->posits = posits;
	//	indiv->npos = npos;	
		free(posits);
		return;
	}
	
	for (i = 0; i < setsize; i++) {
		if (mask[i] == -1 ||  mask[i] == 1) {
			continue;
		}

		if (!placefig(figset, i, posits, npos, &minang, width, height, angstep)) {
			for (j = i; j < setsize; j++) {
				if (figset[i].id == figset[j].id) {
					mask[j] = -1;
				}
			}
			continue;
		}

		mask[i] = 1;
		npos++;
		
		tmpheight = (tmpheight > posits[npos - 1].fig.corner.y + posits[npos - 1].y)? tmpheight : posits[npos - 1].fig.corner.y + posits[npos - 1].y;
		fprintf(logfile, "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", i, npos, minang, tmpheight, posits[npos - 1].x, posits[npos - 1].y);	
		sprintf(indiv->nestlog[npos - 1], "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", i, npos, minang, tmpheight, posits[npos - 1].x, posits[npos - 1].y);
		indiv->genom[npos - 1] = i;
	}
	
	indiv->gensize = npos;
	indiv->height = tmpheight;
	indiv->posits = posits;
	indiv->npos = npos;	

	if (out == NULL) {
//		free(posits);
	} else { 
		out->posits = posits;
		out->npos = npos;
	}
}

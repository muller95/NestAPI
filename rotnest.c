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


static int placefig2(struct Figure *figset, int fignum, struct Position *posits, int npos, double width, double height) 
{
	int placed = 0, angstep;
	double angle;
	double x, ypos, xpos;
	struct Figure currfig;

	angstep = figset[fignum].angstep == 0 ? 360 : figset[fignum].angstep;
	for (angle = 0; angle < 360; angle += angstep) {
		currfig = figdup(&figset[fignum]);
		rotate(&currfig, angle);

		for (x = 0; x < width - currfig.corner.x; x += 1.0) {
			xpos = x;
			ypos = getstart(posits, npos, &currfig, x);
			ymove(&xpos, &ypos,	&currfig, posits, npos);
				
			if (checkpos(&currfig, &posits[npos], xpos, ypos, height, width, &placed))
				posits[npos].angle = angle;
			
			if (ypos == 0)
				break; 			
				
		}
		destrfig(&currfig);
	}

	return placed;
}

static int placefig1(struct Figure *figset, int fignum, struct Position *posits, int npos, double width, double height) 
{
	int placed = 0, count, i, angstep;
	double angle;
	double x, ypos, *xpos;
	struct Figure currfig;
	
	xpos = (double*)xcalloc((int)width, sizeof(double));	
	
	angstep = figset[fignum].angstep == 0 ? 360 : figset[fignum].angstep;
	for (angle = 0; angle < 360; angle += angstep) {
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

			if (checkpos(&currfig, &posits[npos], xpos[i], ytmp, height, width, &placed))
				posits[npos].angle = angle;
		}
				
		destrfig(&currfig);
	}

	free(xpos);

	return placed;

}
 
static int placefig0(struct Figure *figset, int fignum, struct Position *posits, int npos, double width, double height) 
{
	int placed = 0, angstep;
	double angle;
	double x, ypos, xpos;
	struct Figure currfig;
	
	angstep = figset[fignum].angstep == 0 ? 360 : figset[fignum].angstep;
	for (angle = 0; angle < 360; angle += angstep) {
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
		if (checkpos(&currfig, &posits[npos], xpos, ypos, height, width, &placed)) {
			posits[npos].angle = angle;
		}
				
		destrfig(&currfig);
	}

	return placed;
}

void rotnest(struct Figure *figset, int setsize, struct Individ *indiv, struct NestAttrs *attrs)
{
	int i, j, k, npos;
	int *mask;
	double tmpheight;
	double width, height;
	struct Position *posits;
	static int (*placefig)(struct Figure *figset, int fignum, struct Position *posits, int npos, double width, double height);
	FILE *logfile;
	double mtx[3][3];

	logfile = attrs->logfile;
	width = attrs->width;
	height = attrs->height;
	
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
	
		if (!placefig(figset, fignum, posits, npos, width, height)) {
			fprintf(logfile, "fail to position %d\n", fignum);
			continue;
		}

		for (j = 0; j < 3; j++) {
			for (k = 0; k < 3; k++) {
				mtx[j][k] = (j == k)? 1.0 : 0.0;
			}
		}
		mtx[0][2] = posits[npos].x;
		mtx[1][2] = posits[npos].y;
		mtxmult(mtx, &posits[npos].fig);

		mask[fignum] = 1;
		npos++;
		
		tmpheight = (tmpheight > posits[npos - 1].fig.corner.y + posits[npos - 1].y)? tmpheight : posits[npos - 1].fig.corner.y + posits[npos - 1].y;
		fprintf(logfile, "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", fignum, npos, posits[npos - 1].angle, tmpheight, posits[npos - 1].x, posits[npos - 1].y);
	}

	if (npos < indiv->gensize) {
		indiv->height = INFINITY;
		indiv->posits = NULL;
		free(posits);
		return;
	}
	
	for (i = 0; i < setsize; i++) {
		if (mask[i] == -1 ||  mask[i] == 1) {
			continue;
		}

		if (!placefig(figset, i, posits, npos, width, height)) {
			for (j = i; j < setsize; j++) {
				if (figset[i].id == figset[j].id) {
					mask[j] = -1;
				}
			}
			continue;
		}

		for (j = 0; j < 3; j++) {
			for (k = 0; k < 3; k++) {
				mtx[j][k] = (j == k)? 1.0 : 0.0;
			}
		}
		mtx[0][2] = posits[npos].x;
		mtx[1][2] = posits[npos].y;
		mtxmult(mtx, &posits[npos].fig);

		mask[i] = 1;
		npos++;
		
		tmpheight = (tmpheight > posits[npos - 1].fig.corner.y + posits[npos - 1].y)? tmpheight : posits[npos - 1].fig.corner.y + posits[npos - 1].y;
		fprintf(logfile, "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", i, npos, posits[npos - 1].angle, tmpheight, posits[npos - 1].x, posits[npos - 1].y);	
		indiv->genom[npos - 1] = i;
	}
	
	indiv->gensize = npos;
	indiv->height = tmpheight;
	indiv->posits = posits;
	indiv->npos = npos;	
}

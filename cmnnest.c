#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include "figure.h"
#include "nest_structs.h"
#include "crosscheck.h"
#include "cmnfuncs.h"
#include "nestdefs.h"

double getstart(struct Position *posits, int npos, struct Figure *currfig, double x);
void ymove(double *xpos, double *ypos, struct Figure *currfig, struct Position *posits, int npos);
void xmove(double *xpos, double *ypos, struct Figure *currfig, struct Position *posits, int npos);
int checkpos(struct Figure *currfig, struct Position *lastpos, double xpos, double ypos, double height, double width, int *placed);
int mutate(struct Individ *src, struct Individ *mutant, int setsize);
int gensequal(struct Individ *indiv1, struct Individ *indiv2);
int gensequal2(struct Individ *indiv1, struct Individ *indiv2, struct Figure *figset);
int crossover(struct Individ *par1, struct Individ *par2, struct Individ *child, int setsize);

int crossover(struct Individ *par1, struct Individ *par2, struct Individ *child, int setsize)
{
	int i, j;
	int g1, g2;

	if (par1->gensize < 3)
		return SMALL_INDIVID;

	if (par1->gensize != par2->gensize)
		return DIFFERENT_SIZE;

	srand(time(NULL));

	g1 = rand() % (par1->gensize);
	g2 = rand() % (par2->gensize);

	while (g1 == g2) 
		g2 = rand() % (par1->gensize);

	child->par1 = par1->genom;
	child->par2 = par2->genom;
	child->gensize1 = par1->gensize;
	child->gensize2 = par2->gensize;
	child->g1 = g1;
	child->g2 = g2;
	
	child->genom = (int*)xmalloc(sizeof(int) * setsize);
	child->gensize = par1->gensize;
	
	child->genom[g1] = par1->genom[g1];
	child->genom[g2] = par1->genom[g2];

	for (i = 0, j = 0; i < par2->gensize && j < par2->gensize; i++, j++) {
		if (j == g1 || j == g2) {
			i--;
			continue;
		}

		if (par2->genom[i] == child->genom[g1] || par2->genom[i] == child->genom[g2]) {
			j--;
			continue;
		}

		child->genom[j] = par2->genom[i];
	}
	
	return SUCCESSFUL;	
}

int gensequal2(struct Individ *indiv1, struct Individ *indiv2, struct Figure *figset) 
{
	int i;

	if (indiv1->gensize != indiv2->gensize)
		return 0;

	for (i = 0; i < indiv1->gensize; i++) {
		int a, b;

		a = indiv1->genom[i];
		b = indiv2->genom[i];

		if (figset[a].id != figset[b].id)
			return INDIVIDS_UNEQUAL;
	}

	return INDIVIDS_EQUAL;
}

int gensequal(struct Individ *indiv1, struct Individ *indiv2) 
{
	int i;

	if (indiv1->gensize != indiv2->gensize)
		return 0;

	for (i = 0; i < indiv1->gensize; i++)
		if (indiv1->genom[i] != indiv2->genom[i])
			return INDIVIDS_UNEQUAL;

	return INDIVIDS_EQUAL;
}

int mutate(struct Individ *src, struct Individ *mutant, int setsize)
{
 	int n1, n2, tmp, i;

	if (src->gensize == 1)
		return SMALL_INDIVID;
	
	mutant->gensize = src->gensize;
	mutant->genom = (int*)xmalloc(sizeof(int*) * setsize);
    
	srand(time(NULL));

	n1 = rand() % (src->gensize - 1);
	n2 = rand() % (src->gensize);

	while (n1 == n2) 
		n2 = rand() % (src->gensize);

	
	mutant->par1 = src->genom;
	mutant->par2 = NULL;
	mutant->gensize1 = src->gensize;
	mutant->g1 = n1;
	mutant->g2 = n2;
	    
	for (i = 0; i < src->gensize; i++)
		mutant->genom[i] = src->genom[i];
		
	tmp = mutant->genom[n1];
	mutant->genom[n1] = mutant->genom[n2];
	mutant->genom[n2] = tmp;

	return SUCCESSFUL;
}

int checkpos(struct Figure *currfig, struct Position *lastpos, double xpos, double ypos, double height, double width, int *placed)
{
	int res = 0;
	double hcurr, wcurr;

	hcurr = currfig->corner.y + ypos;
	wcurr = xpos + currfig->corner.x;

	if (hcurr >= height ||  wcurr >= width)				
		return 0;



	if (*placed == 0) {
		*placed = 1;
		res = 1;
		lastpos->fig = figdup(currfig);
		lastpos->x = xpos;
		lastpos->y = ypos;
	} else if (*placed == 1) {
		double hprev, wprev, gy, mingy;

		hprev = lastpos->y + lastpos->fig.corner.y;
		wprev = lastpos->x + lastpos->fig.corner.x;

		gy = currfig->gcenter.y + ypos;
		mingy = lastpos->fig.gcenter.y + lastpos->y;

		if (hcurr < hprev || 
			((fabs(hcurr - hprev) < DBL_EPSILON && gy < mingy) || 
			(fabs(hcurr - hprev) < DBL_EPSILON && fabs(gy - mingy) < DBL_EPSILON && wcurr < wprev))) {
			res = 1;
			destrfig(&(lastpos->fig));
			lastpos->fig = figdup(currfig);
			lastpos->x = xpos;
			lastpos->y = ypos;
		}
	}

	return res;
}


double getstart(struct Position *posits, int npos, struct Figure *currfig, double x)
{
	int i;
	double ystart;

	ystart = 0;
	for (i = 0; i < npos; i++) {
		double xl1, xr1, xl2, xr2, ycurr;

		xl1 = posits[i].x;
		xr1 = posits[i].x + posits[i].fig.corner.x;
            
		xl2 = x;
		xr2 = x + currfig->corner.x;


		if (xl1 > xr2  || xl2 > xr1)
			continue;

		ycurr = posits[i].fig.corner.y + posits[i].y;
		ystart = (ycurr > ystart)? ycurr : ystart;
	}

	return ystart;
}

void xmove(double *xpos, double *ypos, struct Figure *currfig, struct Position *posits, int npos)
{
	int i;
	double x, y, xprev;

	xprev = *xpos;
	y = *ypos;
	
	for (x = *xpos; x >= 0; x -= 1.0) {
		int res = 0;
			
		for (i = 0; i < npos; i++) {
			struct Point offset, posoffset;
			
			offset.x = x;
			offset.y = y;
			posoffset.x = posits[i].x;
			posoffset.y = posits[i].y;
			
			res = crosscheck(currfig, &posits[i].fig, offset, posoffset);

			if (res == 1){
				break;
			}
		}
		
		if (res == 1)
			break;

		*xpos = x;
	}
	
	if (fabs(xprev - *xpos) > DBL_EPSILON)
		ymove(xpos, ypos, currfig, posits, npos);    
}


void ymove(double *xpos, double *ypos, struct Figure *currfig, struct Position *posits, int npos)
{
	int i;
	double x, y;

	x = *xpos;
	
	for (y = *ypos; y >= 0; y -= 1.0) {
		int res = 0;

		//printf("y=%lf\n", y);
			
		for (i = 0; i < npos; i++) {
			struct Point offset, posoffset;

			offset.x = x;
			offset.y = y;
			posoffset.x = posits[i].x;
			posoffset.y = posits[i].y;
			
			res = crosscheck(currfig, &posits[i].fig, offset, posoffset);

			if (res == 1) {
				break;
			}
		}
		
		if (res == 1)
			break;

		*ypos = y;
	}

	xmove(xpos, ypos, currfig, posits, npos); 	
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "figure.h"
#include "nest_structs.h"
#include "crosscheck.h"
#include "cmnfuncs.h"

double getstart(struct Position *posits, int npos, struct Figure *currfig, double x);
void ymove(double *xpos, double *ypos, struct Figure *currfig, struct Position *posits, int npos);
void xmove(double *xpos, double *ypos, struct Figure *currfig, struct Position *posits, int npos);
int checkpos(struct Figure *currfig, struct Position *lastpos, int npos, double xpos, double ypos, double height, double width, int *placed);
struct Individ mutate(struct Individ *src, int setsize);
int gensequal(struct Individ *indiv1, struct Individ *indiv2);
int gensequal2(struct Individ *indiv1, struct Individ *indiv2, struct Figure *figset);
void crossover(struct Individ *par1, struct Individ *par2, struct Individ childs[2], int setsize);

void crossover(struct Individ *par1, struct Individ *par2, struct Individ childs[2], int setsize)
{
	int i, j;
	int g1, g2;

	srand(time(NULL));
//	g1 = rand() % (par1->gensize - 1);
//	g2 = g1 + 1;
   // g1 = par1->gensize / 2;
   // g2 = g1 + 1;


	g1 = rand() % (par1->gensize);
	g2 = rand() % (par2->gensize);

	while (g1 == g2) 
		g2 = rand() % (par1->gensize);

	
	childs[0].genom = (int*)xmalloc(sizeof(int) * setsize);
	childs[0].gensize = par1->gensize;
	
	childs[0].genom[g1] = par1->genom[g1];
	childs[0].genom[g2] = par1->genom[g2];

	for (i = 0, j = 0; i < par2->gensize && j < par2->gensize; i++, j++) {
		if (j == g1 || j == g2) {
			i--;
			continue;
		}

		if (par2->genom[i] == childs[0].genom[g1] || par2->genom[i] == childs[0].genom[g2]) {
			j--;
			continue;
		}

		childs[0].genom[j] = par2->genom[i];
	}

/*	childs[0].genom[g1] = par1->genom[g1];
	childs[0].genom[g2] = par1->genom[g2];
    
	count = 0;
	for (i = 0; i < par2->gensize; i++) {
		if (count == g1 || count == g2) {
			count++;
			i--;
			continue;
		}
			
		if (par2->genom[i] == par1->genom[g1] || par2->genom[i] == par1->genom[g2]) 
			continue;
				
		childs[0].genom[count] = par2->genom[i];
		count++;
	}*/
	
	childs[1].genom = (int*)xmalloc(sizeof(int) * setsize);
	childs[1].gensize = par2->gensize;

	childs[1].genom[g1] = par2->genom[g1];
	childs[1].genom[g2] = par2->genom[g2]; 

	for (i = 0, j = 0; i < par1->gensize && j < par1->gensize; i++, j++) {
		if (j == g1 || j == g2) {
			i--;
			continue;
		}

		if (par1->genom[i] == childs[1].genom[g1] || par1->genom[i] == childs[1].genom[g2]) {
			j--;
			continue;
		}

		childs[1].genom[j] = par1->genom[i];
	}

    
/*	count = 0;
	for (i = 0; i < par2->gensize; i++) {
		if (count == g1 || count == g2) {
			count++;
			i--;
			continue;
		}

		if (par1->genom[i] == par2->genom[g1] || par1->genom[i] == par2->genom[g2])
			continue;

		childs[1].genom[count] = par1->genom[i];
		count++;
	}*/
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
			return 0;
	}

	return 1;
}

int gensequal(struct Individ *indiv1, struct Individ *indiv2) 
{
	int i;

	if (indiv1->gensize != indiv2->gensize)
		return 0;

	for (i = 0; i < indiv1->gensize; i++)
		if (indiv1->genom[i] != indiv2->genom[i])
			return 0;

	return 1;
}

struct Individ mutate(struct Individ *src, int setsize)
{
 	int n1, n2, tmp, i;
	struct Individ new;

	new.gensize = src->gensize;
	new.genom = (int*)xmalloc(sizeof(int*) * setsize);
    
	srand(time(NULL));

	n1 = rand() % (src->gensize);
	n2 = rand() % (src->gensize);

	while (n1 == n2) 
		n2 = rand() % (src->gensize);
    
	for (i = 0; i < src->gensize; i++)
		new.genom[i] = src->genom[i];
		
	tmp = new.genom[n1];
	new.genom[n1] = new.genom[n2];
	new.genom[n2] = tmp;

	return new;
}

int checkpos(struct Figure *currfig, struct Position *lastpos, int npos, double xpos, double ypos, double height, double width, int *placed)
{
	int res = 0;
	double ycurr, yprev;


	if (currfig->corner.y + ypos >= height)				
		return 0;

	if (xpos + currfig->corner.x >= width)
		return 0;

	if (*placed == 1) {
		ycurr = ypos + currfig->corner.y;
		yprev = lastpos->y + lastpos->fig.corner.y;
	}

	if (*placed == 0 || ycurr < yprev) {
		*placed = 1;
		res = 1;
		lastpos->fig = figdup(currfig);
		lastpos->x = xpos;
		lastpos->y = ypos;
	} else if (*placed == 1) {
		double gx, gy, mingx, mingy;

		gx = currfig->gcenter.x;
		gy = currfig->gcenter.y;
		mingx = lastpos->fig.gcenter.x;
		mingy = lastpos->fig.gcenter.y;

		if (ycurr == yprev && (gy < mingy || (gy == mingy && gx < mingx))) {
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
				//printf("npos=%d x=%lf y=%lf\n", npos, x, y);
				break;
			}
		}
		
		if (res == 1)
			break;

		*xpos = x;
	}
	
	if (xprev != *xpos)
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
			//	printf("npos=%d x=%lf y=%lf\n", npos, x, y);
				break;
			}
		}
		
		if (res == 1)
			break;

		*ypos = y;
	}

	xmove(xpos, ypos, currfig, posits, npos); 	
}

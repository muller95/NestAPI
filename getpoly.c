#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "figure.h"
#include "nest_structs.h"
#include "cmnfuncs.h"

int getpoly(struct Figure *fig, struct Point **pts);

static struct Point getvec(struct Point p1, struct Point p2)
{
	struct Point vec;
	vec.x = p2.x - p1.x;
	vec.y = p2.y - p1.y;
	return vec;
}

static double veclen(struct Point vec)
{	
	double len;
	len = sqrt(pow(vec.x, 2) + pow(vec.y, 2));
	return len;
}

static struct Point getstart(struct Figure *fig)
{
	struct Point p;
	double x, y;
	double xmin, ymin;
	int i, j;
	
	p = fig->prims[0].pts[0];
	xmin = fig->prims[0].pts[0].x;
	ymin = fig->prims[0].pts[0].y;
	
	for (i = 0; i < fig->nprims; i++) {
		for (j = 0; j < fig->prims[i].npts; j++) {
			x = fig->prims[i].pts[j].x;
			y = fig->prims[i].pts[j].y;

			if (xmin > x) {
				p = fig->prims[i].pts[j];
				xmin = x;
				ymin = y;
			}
			else if ((xmin == x) && (ymin > y)) {
				p = fig->prims[i].pts[j];
				ymin = y;
			}
		}
	}


	return p;
}


int getpoly(struct Figure *fig, struct Point **pts)
{
	int npts = 0;
	int n = 0;
	int i, j;
	double crossprod;
	struct Point vec, tmpvec;
	struct Point start, curr, tmp, next;

	for (i = 0; i < fig->nprims; i++) {
		npts += fig->prims[i].npts;
	}
	
	*pts = (struct Point*)xmalloc(sizeof(struct Point) * npts);
		
	start = getstart(fig);
	curr = start;
	tmp = fig->prims[0].pts[0];

	do {
		vec = getvec(curr, tmp);
		
		for (i = 0; i < fig->nprims; i++) {
			for (j = 0; j < fig->prims[i].npts; j++) {
				tmp = fig->prims[i].pts[j];
			
				if (tmp.x == curr.x && tmp.y == curr.y)
					continue;
			
				tmpvec = getvec(curr, tmp);
				crossprod = vec.x * tmpvec.y - vec.y * tmpvec.x;
				
				if (crossprod < 0.0) {
					vec = tmpvec;
					next = tmp;
				}	else if (fabs(crossprod) < DBL_EPSILON && veclen(tmpvec) > veclen(vec)) {
					vec = tmpvec;
					next = tmp;
				}
			}
		}
		(*pts)[n] = curr;
		n++;
		curr = next;
	} while ((start.x != curr.x || start.y != curr.y) && n < npts-1);

	(*pts)[n] = start;
	n++;
	
	return n;
}

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "figure.h"
#include "getpoly.h"
#include "nest_structs.h"
#include "cmnfuncs.h"
#include "trigon.h"

void gcenter(struct Figure *fig);
void move_to_zero(struct Figure *fig);
void rotate(struct Figure *fig, int angle);

void gcenter(struct Figure *fig)
{
	int i, n;
	double xsum, ysum;
	struct Point *pts;

	pts = NULL;
	n = getpoly(fig, &pts);

	xsum = ysum = 0.0;
	for (i = 0; i < n; i++) {
		xsum += pts[i].x;
		ysum += pts[i].y;
	}
	
	fig->gcenter.x = xsum / (double)n;
	fig->gcenter.y = ysum / (double)n;
}

void move_to_zero(struct Figure *fig)
{
	int i, j;
	double xmin, ymin, xmax, ymax;
	double mtx[3][3];

	xmin = xmax = fig->prims[0].pts[0].x;
	ymin = ymax = fig->prims[0].pts[0].y;

	for (i = 0; i < fig->nprims; i++) {
		for (j = 0; j < fig->prims[i].npts; j++) {
			struct Point pt;
			
			pt = fig->prims[i].pts[j];
			
			xmax = (pt.x > xmax)? pt.x : xmax;
			ymax = (pt.y > ymax)? pt.y : ymax;

			xmin = (pt.x < xmin)? pt.x : xmin;
			ymin = (pt.y < ymin)? pt.y : ymin;
		}
	}

	for (i = 0; i < fig->nprims; i++) {
		for (j = 0; j < fig->prims[i].npts; j++) {
			fig->prims[i].pts[j].x -= xmin;
			fig->prims[i].pts[j].y -= ymin;
		}
	}

	fig->corner.x = xmax - xmin;
	fig->corner.y = ymax - ymin;
	fig->gcenter.x -= xmin;
	fig->gcenter.y -= ymin;


	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mtx[i][j] = (i == j)? 1.0 : 0.0;
		}
	}
	mtx[0][2] = (-1) * xmin;
	mtx[1][2] = (-1) * ymin;
	mtxmult(mtx, fig);
}

void rotate(struct Figure *fig, int angle)
{
	int i, j;
	struct Point gravp;
	double mtx[3][3];

	if (angle < 0) {
		angle = (-1) * (angle % 360) + 360;
	} else {
		angle %= 360;
	}

	gravp = fig->gcenter;
	fig->gcenter.x = gravp.x * cosine[angle] - gravp.y * sine[angle];
	fig->gcenter.y = gravp.x * sine[angle] + gravp.y * cosine[angle]; 

	for (i = 0; i < fig->nprims; i++) {  
		for (j = 0; j < fig->prims[i].npts; j++) {
			struct Point p;
			p = fig->prims[i].pts[j];
			fig->prims[i].pts[j].x = p.x * cosine[angle] - p.y  * sine[angle];
			fig->prims[i].pts[j].y = p.x * sine[angle] + p.y * cosine[angle]; 
		}
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			mtx[i][j] = (i == j)? 1.0 : 0.0;
		}
	}
	
	mtx[0][0] = cosine[angle];
	mtx[0][1] = (-1) * sine[angle];
	mtx[1][0] = sine[angle];
	mtx[1][1] = cosine[angle];
	mtxmult(mtx, fig);

	move_to_zero(fig);
}


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
	char trans[512];

	memset(trans, 0, sizeof(trans));

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
	sprintf(trans, " translate(%lf,%lf)", (-1) * xmin, (-1) * ymin);
	strcat(fig->trfrms, trans);
}

void rotate(struct Figure *fig, int angle)
{
	int i, j;
	struct Point gravp;
	char rot[512];

	memset(rot, 0, sizeof(rot));
	
	if (angle < 0) {
		angle = (-1) * (angle % 360) + 360;
	} else {
		angle %= 360;
	}

	gravp = fig->gcenter;
	fig->gcenter.x = gravp.x * cosinus[angle] - gravp.y * sinus[angle];
	fig->gcenter.y = gravp.x * sinus[angle] + gravp.y * cosinus[angle]; 

	for (i = 0; i < fig->nprims; i++) {  
		for (j = 0; j < fig->prims[i].npts; j++) {
			struct Point p;
			p = fig->prims[i].pts[j];
			fig->prims[i].pts[j].x = p.x * cosinus[angle] - p.y  * sinus[angle];
			fig->prims[i].pts[j].y = p.x * sinus[angle] + p.y * cosinus[angle]; 
		}
	}
	
	sprintf(rot, " rotate(%d)", angle);
	strcat(fig->trfrms, rot);
	move_to_zero(fig);
}


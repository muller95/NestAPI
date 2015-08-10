#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "figure.h"

void move_to_zero(struct Figure *fig);
void rotate(struct Figure *fig, double angle);

void move_to_zero(struct Figure *fig)
{
	int i, j;
	double xmin, ymin, xmax, ymax;

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
}

void rotate(struct Figure *fig, double angle)
{
	int i, j;
	double anglerads;
	struct Point gravp; 
	anglerads = angle * M_PI / 180;

	gravp = fig->gcenter;
	fig->gcenter.x = gravp.x * cos(anglerads) - gravp.y * sin(anglerads);
	fig->gcenter.y = gravp.x * sin(anglerads) + gravp.y * cos(anglerads); 

	for (i = 0; i < fig->nprims; i++) {  
		for (j = 0; j < fig->prims[i].npts; j++) {
			struct Point p;
			p = fig->prims[i].pts[j];
			fig->prims[i].pts[j].x = p.x * cos(anglerads) - p.y  * sin(anglerads);
			fig->prims[i].pts[j].y = p.x * sin(anglerads) + p.y * cos(anglerads); 
		}
	}

	move_to_zero(fig);
}


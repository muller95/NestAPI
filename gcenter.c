#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "figure.h"
#include "getpoly.h"
#include "nest_structs.h"
#include "cmnfuncs.h"

void gcenter(struct Figure *fig)
{
	int i, n, start, step, end, nsquares;
	double *squares;
	double squaresum, xsum, ysum;
	struct Point *pts, *centers, tmppt, tmppt1;

	pts = NULL;
	n = getpoly(fig, &pts);
	squares = (double*)xmalloc(sizeof(double) * n);
	centers = (struct Point*)xmalloc(sizeof(struct Point) * n - 1);

	tmppt = pts[0];
	tmppt1 = pts[1];

	start = (tmppt.y <= tmppt1.y)? 0 : n - 1;
	step = (tmppt.y <= tmppt1.y)? 1 : -1;
	end = (tmppt.y <= tmppt1.y)? n : 0;

	nsquares = 0;
	for (i = start; i != end; i += step) {
		struct Point p1, p2, rect_gcenter, triang_gcenter;
		double rect_square, triangle_square;
		double xproj, ymin, ymax;
	
		p1 = pts[i];
		p2 = pts[i + step];
		  
		if (p1.x == p2.x) {
			squares[nsquares] = 0;
			centers[nsquares].x = p1.x;
			centers[nsquares].y = (p1.y + p2.y) / 2;
			nsquares++;
			continue;
		} else if (p1.y == p2.y) {
			squares[nsquares] = (p2.x - p1.x) * p1.y;
			centers[nsquares].x = (p1.x + p2.x) / 2;
			centers[nsquares].y = p1.y / 2;
			nsquares++;
			continue;
		}
		
		xproj = p2.x - p1.x;
		ymin = (p1.y < p2.y)? p1.y : p2.y;
		ymax = (p1.y > p2.y)? p1.y : p2.y;
		
		rect_square = xproj * ymin;
		triangle_square = xproj * (ymax - ymin) / 2;
		rect_gcenter.x = xproj / 2;
		rect_gcenter.y = ymin / 2;
		
		triang_gcenter.y = (ymin + ymin + ymax) / 3;
		if (p1.y > p2.y)
			triang_gcenter.x = (p1.x + p1.x + p2.x) / 3;
		else 
			triang_gcenter.x = (p1.x + p2.x + p2.x) / 3; 
			

		squares[nsquares] = rect_square + triangle_square;
		centers[nsquares].x = (fabs(rect_square) * rect_gcenter.x + triangle_square * fabs(triang_gcenter.x)) / (fabs(rect_square + triangle_square));
		centers[nsquares].y = (fabs(rect_square) * rect_gcenter.y + triangle_square * fabs(triang_gcenter.y)) / (fabs(rect_square + triangle_square));
		nsquares++;
	}
	
	squaresum = xsum = ysum = 0.0;
	for (i = 0; i < nsquares; i++) {
		squaresum += squares[i];
		xsum += centers[i].x * squares[i];
		ysum += centers[i].y * squares[i];
	}

	fig->gcenter.x = xsum / squaresum;
	fig->gcenter.y = ysum / squaresum;
}

void gcenter2(struct Figure *fig)
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





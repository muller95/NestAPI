#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include "figure.h"
#include "geometry.h"
#include "nest_structs.h"
#include "cmnfuncs.h"
#include "cmnnest.h"


static void evalline(struct Point *p1, struct Point *p2, double *outk, double *outb)
{
	*outk = (p1->y - p2->y) / (p1->x - p2->x);
	*outb = p1->y - *outk * p1->x;
}

static double calcx(struct Point *p1, struct Point *p2, double y)
{
	return ((-1.0) * (p1->x * p2->y - p2->x * p1->y) - (p2->x - p1->x) * y) / (p1->y - p2->y);
}

struct NestMatrix approxfig(struct Figure *fig)
{
	struct NestMatrix res;
	int i, j, w, h;
	
	w = (int)trunc(fig->corner.x + 1.0);
	h = (int)trunc(fig->corner.y + 1.0);

	res.mtx = (int **)xmalloc(sizeof(int *) * w);

	for (i = 0; i < w; i++) 
		res.mtx[i] = (int *)xcalloc(h, sizeof(int));

	for (i = 0; i < fig->nprims; i++) {
		for (j = 0; j < fig->prims[i].npts - 1; j++) {
			struct Point p1, p2, pt, pb;
			int x1, x2, x, step;
			double y;
			
			pb = (p1.y < p2.y)? p1 : p2;
			pt = (p1.y > p2.y)? p1 : p2;
						
			if (fabs(pt.y - pb.y) > DBL_EPSILON) {

				x1 = (int)calcx(&p1, &p2, pb.y);
				x2 = (int)calcx(&p1, &p2, ceil(pb.y));
				step = (x1 < x2)? 1 : -1;
				for (x = x1; x != x2 + step; x += step)
					res.mtx[x][(int)pb.y] = 1;
					
				pb.y = ceil(pb.y);
				pt.y = floor(pt.y);	
				
				for (y = ceil(pb.y); y < floor(pt.y); y += 1.0) {

					x1 = (int)calcx(&p1, &p2, pb.y);
					x2 = (int)calcx(&p1, &p2, ceil(pb.y));
					step = (x1 < x2)? 1 : -1;
					for (x = x1; x != x2 + step; x += step)
						res.mtx[x][(int)y] = 1;
				}

				x1 = (int)calcx(&p1, &p2, floor(pt.y));
				x2 = (int)calcx(&p1, &p2, pt.y);
				step = (x1 < x2)? 1 : -1;
				for (x = x1; x != x2 + step; x += step)
					res.mtx[x][(int)pt.y] = 1;
			} else {
				step = (p1.x < p2.x)? 1 : -1;
				for (x = p1.x; x != p2.x + step; x += step)
					res.mtx[x][(int)p1.y] = 1;
			}
		}
	}

	return res;
}

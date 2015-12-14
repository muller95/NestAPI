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

static double calcx(struct Point *p1, struct Point *p2, double y)
{
	return ((-1.0) * (p1->x * p2->y - p2->x * p1->y) - (p2->x - p1->x) * y) / (p1->y - p2->y);
}

static void floodfill(struct NestMatrix *mtx, int i, int j)
{
	if (mtx->mtx[i][j] > 0)
		return;
			
	mtx->mtx[i][j] = 2;

	if (i - 1 > 0)
		floodfill(mtx, i - 1, j);

	if (i + 1 < mtx->w)
		floodfill(mtx, i + 1, j);

	if (j - 1 > 0)
		floodfill(mtx, i, j - 1);

	if (j + 1 < mtx->h)
		floodfill(mtx, i, j + 1);
}

static void floodmtx(struct NestMatrix *mtx)
{
	int i;
	for (i = 0; i < mtx->w; i++)
		if (mtx->mtx[i][0] == 0) {
			floodfill(mtx, i, 0);
		}
}

struct NestMatrix approxfig(struct Figure *fig)
{
	struct NestMatrix res;
	int i, j, w, h;
	
	w = (int)trunc(fig->corner.x + 1.0);
	h = (int)trunc(fig->corner.y + 1.0);

	res.mtx = (int **)xmalloc(sizeof(int *) * w);
	res.w = w;
	res.h = h;
	for (i = 0; i < w; i++) 
		res.mtx[i] = (int *)xcalloc(h, sizeof(int));

	for (i = 0; i < fig->nprims; i++) {
		for (j = 0; j < fig->prims[i].npts - 1; j++) {
			struct Point p1, p2, pt, pb;
			int x1, x2, x, step;
			double y;

			p1 = fig->prims[i].pts[j];
			p2 = fig->prims[i].pts[j + 1];
			
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
				
				for (y = pb.y; y < pt.y; y += 1.0) {

					x1 = (int)calcx(&p1, &p2, y);
					x2 = (int)calcx(&p1, &p2, y + step);
					step = (x1 < x2)? 1.0 : -1.0;
					for (x = x1; x != x2 + step; x += step) {
					//	printf("x=%d y=%d\n", x, (int)y);
						res.mtx[x][(int)y] = 1;
					}
				}

				x1 = (int)calcx(&p1, &p2, floor(pt.y));
				x2 = (int)calcx(&p1, &p2, pt.y);
				step = (x1 < x2)? 1.0 : -1.0;
				for (x = x1; x != x2 + step; x += step)
					res.mtx[x][(int)pt.y] = 1;
			} else {
				step = (p1.x < p2.x)? 1.0 : -1.0;
				for (x = p1.x; x != p2.x + step; x += step)
					res.mtx[x][(int)p1.y] = 1;
			}
		}
	}

	floodmtx(&res);

	return res;
}

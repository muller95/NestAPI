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
#include "nestdefs.h"

int (*checkpos)(struct Figure *currfig, struct Position *posits, int npos, double xpos, double ypos, double height, double width, int *placed);

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
	for (i = 0; i < mtx->w; i++) {
		if (mtx->mtx[i][0] == 0) 
			floodfill(mtx, i, 0);
		if (mtx->mtx[i][mtx->h-1] == 0) 
			floodfill(mtx, i, mtx->h - 1);
	}

	for (i = 0; i < mtx->h; i++) {
		if (mtx->mtx[0][i] == 0) 
			floodfill(mtx, 0, i);
		if (mtx->mtx[mtx->w - 1][i] == 0) 
			floodfill(mtx, mtx->w - 1, i);
	}

}

static void destrmtx(struct NestMatrix *mtx)
{	
	int i;

	for (i = 0; i < mtx->w; i++)
		free(mtx->mtx[i]);
	
	free(mtx->mtx);
}

struct NestMatrix mtxdup(struct NestMatrix *src)
{
	int i, j;
	struct NestMatrix res;
	res.w = src->w;
	res.h = src->h;
	res.mtx = (int **)xmalloc(sizeof(int *) * res.w);

	for (i = 0; i < res.w; i++) 
		res.mtx[i] = (int *)xcalloc(res.h, sizeof(int));
	for (i = 0; i < res.w; i++) 
		for (j = 0; j < res.h; j++)
			res.mtx[i][j] = src->mtx[i][j];

	return res;
}

struct NestMatrix approxfig(struct Figure *fig, int resize)
{
	struct NestMatrix res, res2;
	int i, j, w, h;
	
	w = (int)ceil(fig->corner.x) + 1;
	h = (int)ceil(fig->corner.y) + 1;

	res.mtx = (int **)xmalloc(sizeof(int *) * w);
	res.w = w;
	res.h = h;
	for (i = 0; i < w; i++) 
		res.mtx[i] = (int *)xcalloc(h, sizeof(int));

	for (i = 0; i < fig->nprims; i++) {
		for (j = 0; j < fig->prims[i].npts - 1; j++) {
			struct Point p1, p2, pt, pb;
			double x1, x2;
			int x, step;
			double y;

			p1 = fig->prims[i].pts[j];
			p2 = fig->prims[i].pts[j + 1];
			
			pb = (p1.y < p2.y)? p1 : p2;
			pt = (p1.y > p2.y)? p1 : p2;
						
			if (trunc(pt.y) - trunc(pb.y) > 0) {
				x1 = calcx(&p1, &p2, pb.y);
				x2 = calcx(&p1, &p2, ceil(pb.y));
				if (x1 > x2)
					x1 = ceil(x1);
				else if (x2 > x1)
					x2 = ceil(x2);
				step = (x1 < x2)? 1 : -1;
				for (x = (int)x1; x != (int)x2 + step; x += step)
					res.mtx[x][(int)pb.y] = 1;
					
		/*		pb.y = ceil(pb.y);
				pt.y = floor(pt.y);	*/
				
				for (y = ceil(pb.y); y < floor(pt.y); y += 1.0) {

					x1 = calcx(&p1, &p2, y);
					x2 = calcx(&p1, &p2, y + 1.0);
					if (x1 > x2)
						x1 = ceil(x1);
					else if (x2 > x1)
						x2 = ceil(x2);
					step = (x1 < x2)? 1.0 : -1.0;
					for (x = (int)x1; x != (int)x2 + step; x += step) {
						res.mtx[x][(int)y] = 1;
					}
				}

				x1 = calcx(&p1, &p2, floor(pt.y));
				x2 = calcx(&p1, &p2, pt.y);
				if (x1 > x2)
					x1 = ceil(x1);
				else if (x2 > x1)
					x2 = ceil(x2);
				step = (x1 < x2)? 1.0 : -1.0;
				for (x = (int)x1; x != (int)x2 + step; x += step)
					res.mtx[x][(int)pt.y] = 1;
			} else {
				x1 = p1.x;
				x2 = p2.x;
				if (x1 > x2)
					x1 = ceil(x1);
				else if (x2 > x1)
					x2 = ceil(x2);
				step = (x1 < x2)? 1.0 : -1.0;
				for (x = (int)x1; x != (int)x2 + step; x += step)
					res.mtx[x][(int)p1.y] = 1;
			}
		}
	}

	for (i = 0; i < res.w; i++) {
		for (j = 0; j < res.h; j++) 
			if (res.mtx[i][j] == 1) {
				if (i + 1 < res.w)
					res.mtx[i + 1][j] = 2;
				if (i - 1 > 0)
					res.mtx[i - 1][j] = 2;		
				if (j + 1 < res.h)	
					res.mtx[i][j + 1] = 2;
				if (j - 1 > 0)
					res.mtx[i][j - 1] = 2;
			}	
		}

	for (i = 0; i < res.w; i++) 
		for (j = 0; j < res.h; j++) 
			if (res.mtx[i][j] == 2) 
				res.mtx[i][j] = 1;



	floodmtx(&res);
	for (i = 0; i < res.w; i++) 
		for (j = 0; j < res.h; j++) {
			if (res.mtx[i][j] == 2) 
				res.mtx[i][j] = 0;
			else 
				res.mtx[i][j] = 1;
		}

	if (resize > 1) {
		res2.w = res.w / resize + (res.w % resize > 0 ? 1 : 0);
		res2.h = res.h / resize + (res.h % resize > 0 ? 1 : 0);
		res2.mtx = (int **)xmalloc(sizeof(int *) * res2.w);
		for (i = 0; i < res2.w; i++) 
			res2.mtx[i] = (int *)xcalloc(res2.h, sizeof(int));

		for (i = 0; i < res.w; i++) 
			for (j = 0; j < res.h; j++) {
				res2.mtx[i / resize][j / resize] += res.mtx[i][j];
				res2.mtx[i / resize][j / resize] = res2.mtx[i / resize][j / resize] >= 1 ? 1 : 0; 
			}
					
		return res2;
	}

	return res;
}

static int placefig0(struct Figure *figset, int fignum, int resize, struct Position *posits, int **place, int npos, int width, int height) 
{
	int placed = 0, angstep, first = 1;
	double angle;
	int x, y;
	struct Figure currfig;
	struct NestMatrix minmtx;
	
	angstep = figset[fignum].angstep == 0 ? 360 : figset[fignum].angstep;
	for (angle = 0; angle < 360; angle += angstep) {
		struct NestMatrix mtx;
		currfig = figdup(&figset[fignum]);
		rotate(&currfig, angle);
		
		mtx = approxfig(&currfig, resize);
		if (mtx.w > width / resize || mtx.h > height / resize) 
			continue;
		for (y = 0; y < height / resize - mtx.h; y++) {
			for (x = 0; x < width / resize - mtx.w; x++) {
				int x1, y1, sum = 0;

				for (y1 = 0; y1 < mtx.h; y1++) { 
					for (x1 = 0; x1 < mtx.w; x1++) {
						sum += place[x + x1][y + y1] * mtx.mtx[x1][y1];
						if (sum > 0)
							break;
					}
					if (sum > 0)
						break;
				}

				if (sum > 0) 
					continue;

				if (checkpos(&currfig, posits, npos, (double)(x * resize), (double)(y * resize), (double)height, (double)width, &placed)) {
					posits[npos].angle = angle;
					if (!first)
						destrmtx(&minmtx);
					first = 0;
					minmtx= mtxdup(&mtx);
				}
				
				x = width;
				y = height;
			}
		}
		destrmtx(&mtx);
		destrfig(&currfig);
	}
	if (placed) {
		for (y = 0; y < minmtx.h; y++) 
			for (x = 0; x < minmtx.w; x++)
				place[x + (int)posits[npos].x / resize][y + (int)posits[npos].y / resize] += minmtx.mtx[x][y];
	}
	return placed;
}


static int placefig1(struct Figure *figset, int fignum, int resize, struct Position *posits, int **place, int npos, int width, int height) 
{
	int placed = 0, angstep, first = 1;
	double angle;
	int x, y;
	struct Figure currfig;
	struct NestMatrix minmtx;
	
	angstep = figset[fignum].angstep == 0 ? 360 : figset[fignum].angstep;
	for (angle = 0; angle < 360; angle += angstep) {
		struct NestMatrix mtx;
		currfig = figdup(&figset[fignum]);
		rotate(&currfig, angle);
		
		mtx = approxfig(&currfig, resize);
		if (mtx.w > width / resize || mtx.h > height / resize) 
			continue;
		for (y = 0; y < height / resize - mtx.h; y++) {
			for (x = 0; x < width / resize - mtx.w; x++) {
				int x1, y1, sum = 0;

				for (y1 = 0; y1 < mtx.h; y1++) { 
					for (x1 = 0; x1 < mtx.w; x1++) {
						sum += place[x + x1][y + y1] * mtx.mtx[x1][y1];
						if (sum > 0)
							break;
					}
					if (sum > 0)
						break;
				}

				if (sum > 0) 
					continue;

				if (checkpos(&currfig, posits, npos, (double)(x * resize), (double)(y * resize), (double)height, (double)width, &placed)) {
					posits[npos].angle = angle;
					if (!first)
						destrmtx(&minmtx);
					first = 0;
					minmtx= mtxdup(&mtx);
				}


				if (x == 0) 
					y = height;
				
				break;
			}
		}
		destrmtx(&mtx);
		destrfig(&currfig);
	}
	if (placed) {
		for (y = 0; y < minmtx.h; y++) 
			for (x = 0; x < minmtx.w; x++)
				place[x + (int)posits[npos].x / resize][y + (int)posits[npos].y / resize] += minmtx.mtx[x][y];
	}
	return placed;
}


void mtxnest(struct Figure *figset, int setsize, int resize, struct Individ *indiv, struct NestAttrs *attrs)
{
	int i, j, k, npos;
	int *mask;
	int **place;
	double tmpheight;
	int width, height;
	struct Position *posits;
	FILE *logfile;
	double mtx[3][3];
	int (*placefig)(struct Figure *figset, int fignum, int resize, struct Position *posits, int **place, int npos, int width, int height); 
	
	logfile = attrs->logfile;
	width = (int)attrs->width;
	height = (int)attrs->height;
	
		
	checkpos = checkpos_height;
	if (attrs->checker == CHECK_RADIUS) 
		checkpos = checkpos_radius;
	else if (attrs->checker == CHECK_SCALE)
		checkpos = checkpos_scale;

	if (attrs->type == MTXNEST_FULL) {
		placefig = placefig1;
	} else {
		checkpos = checkpos_height;
		placefig = placefig0;
	}

	mask = (int*)xcalloc(setsize, sizeof(int));
	posits = (struct Position*)xmalloc(sizeof(struct Position) * setsize);
	place = (int**)xmalloc(sizeof(int*) * width / resize);

	for (i = 0; i < width / resize; i++)
		place[i] = (int *)xcalloc(height / resize, sizeof(int));
	
	npos = 0;
	
	tmpheight = 0;

	for (i = 0; i < indiv->gensize; i++) {
		int fignum;
		
		fignum = indiv->genom[i];
	
		if (!placefig(figset, fignum, resize, posits, place, npos, width, height)) {
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
//		fprintf(logfile, "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", fignum, npos, posits[npos - 1].angle, tmpheight, posits[npos - 1].x, posits[npos - 1].y);
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

		if (!placefig(figset, i, resize, posits, place, npos, width, height)) {
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
//		fprintf(logfile, "nested_id=%d positioned=%d angle=%lf height=%lf x=%lf y=%lf \n", i, npos, posits[npos - 1].angle, tmpheight, posits[npos - 1].x, posits[npos - 1].y);	
		indiv->genom[npos - 1] = i;
	}
	
	indiv->gensize = npos;
	indiv->height = tmpheight;
	indiv->posits = posits;
	indiv->npos = npos;
	
	free(mask);	
	for (i = 0; i < width / resize; i++)
		free(place[i]);
	free(place);
	


/*	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) 
			fprintf(logfile, "%d", place[i][j]);
		fprintf(logfile, "\n");
	}
	fprintf(logfile, "\n");*/
//	fflush(logfile);
}

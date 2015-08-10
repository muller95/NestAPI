#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "figure.h"


static double vecprod(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}

static int crosscheck_line(struct Point lp1, struct Point lp2, struct Point vp1, struct Point vp2)
{
    return (vecprod(lp2.x - lp1.x, lp2.y - lp1.y, vp1.x - lp1.x, vp1.y - lp1.y) * vecprod(lp2.x - lp1.x, lp2.y - lp1.y, vp2.x - lp1.x, vp2.y - lp1.y)) <= 0;
}

int crosscheck(struct Figure *currfig, struct Figure *posfig, struct Point offset, struct Point posoffset)
{
	double xl1, xr1, yt1, yb1;
	double xl2, xr2, yt2, yb2;
	int i, j, k, m;

	xl1 = offset.x;
	xr1 = currfig->corner.x + offset.x;
	yb1 = offset.y;
	yt1 = currfig->corner.y + offset.y;

	xl2 = posoffset.x;
	xr2 = posfig->corner.x + posoffset.x;
	yb2 = posoffset.y;
	yt2 = posfig->corner.y + posoffset.y;

	if (xl1 >= xr2 || xr1 <= xl2 || yb1 >= yt2 || yt1 <= yb2)
		return 0;
	
	for (i = 0; i < currfig->nprims; i++) { 		
		for (j = 0; j < currfig->prims[i].npts - 1; j++) {
			struct Point cp1, cp2;
			cp1 = currfig->prims[i].pts[j];
			cp1.x += offset.x;
			cp1.y += offset.y;

			cp2 = currfig->prims[i].pts[j + 1];
			cp2.x += offset.x;
			cp2.y += offset.y;

			xl1 = (cp1.x < cp2.x)? cp1.x : cp2.x;
			xr1 = (cp1.x > cp2.x)? cp1.x : cp2.x;
			yb1 = (cp1.y < cp2.y)? cp1.y : cp2.y;
			yt1 = (cp1.y > cp2.y)? cp1.y : cp2.y;


			for (k = 0; k < posfig->nprims; k++) {
				for (m = 0; m < posfig->prims[k].npts; m++) {
					struct Point pp1, pp2;
	
					pp1 = posfig->prims[k].pts[m];
					pp1.x += posoffset.x;
					pp1.y += posoffset.y;
			
					pp2 = posfig->prims[k].pts[m + 1];
					pp2.x += posoffset.x;
					pp2.y += posoffset.y;

					xl2 = (pp1.x < pp2.x)? pp1.x : pp2.x;
					xr2 = (pp1.x > pp2.x)? pp1.x : pp2.x;
					yb2 = (pp1.y < pp2.y)? pp1.y : pp2.y;
					yt2 = (pp1.y > pp2.y)? pp1.y : pp2.y;
		
					if (xl1 > xr2 || xr1 < xl2 || yb1 > yt2 || yt1 < yb2)
			    	continue;
            
					if (!crosscheck_line(pp1, pp2, cp1, cp2))
						continue;
					else if (!crosscheck_line(cp1, cp2, pp1, pp2))
						continue;
					else 
						return 1;
				}
			}
		}
	}
	return 0;
}


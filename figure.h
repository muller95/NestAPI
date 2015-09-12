struct Point {
	double x, y;
};

struct Primitive {
	struct Point *pts;
	int npts;
};

struct Figure {
	int id;
	char *name, trfrms[1024];
	int nprims, quant, angstep;
	struct Primitive *prims;
	struct Point gcenter, corner;
};

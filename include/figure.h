struct Point {
	double x, y;
};

struct Primitive {
	struct Point *pts;
	int npts;
};

struct Figure {
	int id;
	char *name;
	int nprims, quant, angstep;
	struct Primitive *prims;
	struct Point gcenter, corner;
	struct Point t1, t2;
};

struct Point {
	double x, y;
};

struct Primitive {
	struct Point *pts;
	int npts;
};

struct Figure {
	int id;
	double mtx[3][3];
	char *name;
	int nprims, quant, angstep;
	struct Primitive *prims;
	struct Point gcenter, corner;
};

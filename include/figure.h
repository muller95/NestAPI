struct Point {
	double x, y;
};

struct Primitive {
	struct Point *pts;
	int npts;
};

struct Figure {
	int id;
	int nprims, quant;
	struct Primitive *prims;
	struct Point gcenter, corner;
};

struct Position {
	struct Figure fig;
	double x, y, angle;
	int i;
};

struct Individ {
    int *genom;
    int gensize;
    double height, fitness;
	struct Position *posits;
	int *par1, *par2;
	int gensize1, gensize2;
	int g1, g2;
	char nestlog[24][1024];
	int npos;
};

struct NestAttrs {
	int type;
	double width, height;
	FILE *logfile;
};

struct Position {
	struct Figure fig;
	double x, y;
	int i;
};

struct Individ {
    int *genom;
    int gensize;
    double height, fitness;
};

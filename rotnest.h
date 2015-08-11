struct NestAttrs {
	int type;
	double width, height;
	double angstep;
	FILE *logfile;
};

#define ROTNEST_DEFAULT 0
#define ROTNEST_FULL 	1

void rotnest(struct Figure *figset, int setsize, struct Individ *indiv, struct NestAttrs *attrs);

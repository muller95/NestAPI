double getstart(struct Position *posits, int npos, struct Figure *currfig, double x);
void ymove(double *xstart, double *ystart, struct Figure *currfig, struct Position *posits, int npos);
void xmove(double *xstart, double *ystart, struct Figure *currfig, struct Position *posits, int npos);
int checkpos(struct Figure *currfig, struct Position *lastpos, int npos, double xpos, double ypos, double height, double width, int *placed);
struct Individ mutate(struct Individ *src, int setsize);
int gensequal(struct Individ *indiv1, struct Individ *indiv2); 
int gensequal2(struct Individ *indiv1, struct Individ *indiv2, struct Figure *figset);
void crossover(struct Individ *par1, struct Individ *par2, struct Individ childs[2], int setsize);

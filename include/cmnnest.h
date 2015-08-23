int mutate(struct Individ *src, struct Individ *mutant, int setsize);
int gensequal(struct Individ *indiv1, struct Individ *indiv2); 
int gensequal2(struct Individ *indiv1, struct Individ *indiv2, struct Figure *figset);
int crossover(struct Individ *par1, struct Individ *par2, struct Individ *child, int setsize);


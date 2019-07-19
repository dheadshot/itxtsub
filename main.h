#ifndef __MAIN_H__
#define __MAIN_H__ 1

#define MAXBUFFER 512

typedef char ** ntsa;

void freentsa(ntsa a);
void freesubs();
void freetxtimg();
int loadtxtimg(char *afn);
void gettxtimgstats();
int makesubs(char *substring);
int addsubs();
int writetxtimg(char *afn);
char *readsubs(char *afn);


#endif

extern int tickcount;
#pragma zpsym("tickcount");

#define MIN(a,b)		(((a)<(b))?(a):(b))
#define MAX(a,b)		(((a)>(b))?(a):(b))
#define CLAMP(a,lo,hi)	(MIN(MAX((lo),(a)),(hi)))

extern unsigned int crand(unsigned int lo, unsigned int hi);

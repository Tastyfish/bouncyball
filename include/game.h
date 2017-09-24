extern unsigned int tickcount;
#pragma zpsym("tickcount");
extern char VBLANK_FLAG;
#pragma zpsym("VBLANK_FLAG");

#define MIN(a,b)		(((a)<(b))?(a):(b))
#define MAX(a,b)		(((a)>=(b))?(a):(b))
#define CLAMP(a,lo,hi)	(MIN(MAX((lo),(a)),(hi)))

extern unsigned int crand(unsigned int lo, unsigned int hi);

extern void g_Run(void);

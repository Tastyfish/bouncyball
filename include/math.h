#define MIN(a,b)		(((a)<(b))?(a):(b))
#define MAX(a,b)		(((a)>=(b))?(a):(b))
#define CLAMP(a,lo,hi)	(MIN(MAX((lo),(a)),(hi)))

typedef int decimal_t;
#define INT2DEC(x)		((decimal_t)((x)<<4))
#define DEC2INT(x)		(((int)x)>>4)

extern unsigned int crand(unsigned int lo, unsigned int hi);
extern unsigned char sqrtc(unsigned char x);
extern int sqrti(int x);

extern signed char dotc(signed char x1, signed char y1, signed char x2, signed char y2);
extern void reflectc(signed char *vx, signed char *vy, signed char nx, signed char ny);
extern void normalizec(signed char *x, signed char *y);

extern int doti(int x1, int y1, int x2, int y2);
extern void reflecti(int *vx, int *vy, int nx, int ny);
extern void normalizei(int *x, int *y);

extern decimal_t dotd(decimal_t x1, decimal_t y1, decimal_t x2, decimal_t y2);
extern void reflectd(decimal_t *vx, decimal_t *vy, decimal_t nx, decimal_t ny);
extern void normalized(decimal_t *x, decimal_t *y);

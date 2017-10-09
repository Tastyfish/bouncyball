extern unsigned int tickcount;
#pragma zpsym("tickcount");
extern char VBLANK_FLAG;
#pragma zpsym("VBLANK_FLAG");

extern void g_Run(void);
extern void g_Yield(void);

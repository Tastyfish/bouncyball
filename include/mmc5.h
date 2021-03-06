typedef void (*ScanlineCallback)(unsigned char line);
typedef void* scanlinecb_t;
typedef unsigned char line_t;

extern char vc_chr_lbank;
#pragma zpsym("vc_chr_lbank");
extern char vc_chr_ubank;
#pragma zpsym("vc_chr_ubank");
extern char vc_bg_bank;
#pragma zpsym("vc_bg_bank");

extern char mmc5_nt_mapping;
extern char mmc5_bg_bank;

scanlinecb_t vm_AddScanlineCallback(line_t line, ScanlineCallback callback);
void vm_RemoveScanlineCallback(scanlinecb_t cb);

void vm_SetNametableMirroring(char code);
void vbm_SetNametableMirroring(char code);

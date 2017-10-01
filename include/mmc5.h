typedef void (*ScanlineCallback)(unsigned char line);
typedef void* HScanlineCB;
typedef unsigned char line_t;

extern char vc_chr_lbank;
#pragma zpsym("vc_chr_lbank");
extern char vc_chr_ubank;
#pragma zpsym("vc_chr_ubank");
extern char vc_bg_bank;
#pragma zpsym("vc_bg_bank");

HScanlineCB vm_AddScanlineCallback(line_t line, ScanlineCallback callback);
void vm_RemoveScanlineCallback(HScanlineCB cb);

void __fastcall__ vm_SetNametableMirroring(char code);
void __fastcall__ vbm_SetNametableMirroring(char code);

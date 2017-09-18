typedef void (*ScanlineCallback)(unsigned char line);
typedef void* HScanlineCB;
typedef unsigned char line_t;

HScanlineCB vm_AddScanlineCallback(line_t line, ScanlineCallback callback);
void vm_RemoveScanlineCallback(HScanlineCB cb);

void __fastcall__ vm_SetNametableMirroring(char code);
void __fastcall__ vbm_SetNametableMirroring(char code);

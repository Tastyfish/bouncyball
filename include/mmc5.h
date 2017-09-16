typedef void (*ScanlineCallback)(unsigned char line);
typedef void* HScanlineCB;
typedef unsigned char line_t;

HScanlineCB vm_AddScanlineCallback(line_t line, ScanlineCallback callback);
void vm_RemoveScanlineCallback(HScanlineCB cb);

#include <string.h>
#include <stdlib.h>

#include "mmc5.h"

#define NUM_CALLBACKS	(8)
#define EMPTY_CALLBACK_LINE	(0xFF)

typedef struct {
	line_t line;
	ScanlineCallback callback;
} ScanlineCB_Entry;

extern ScanlineCB_Entry* vc_sl_ptr;
#pragma zpsym("vc_sl_ptr");
extern unsigned char mmc5_sl_counter;
extern unsigned char mmc5_irq_ctrl;

extern char scanlineCount = 0;
extern ScanlineCB_Entry scanline_callbacks[NUM_CALLBACKS];
ScanlineCB_Entry scanline_callbacks[NUM_CALLBACKS];

extern void handle_hblank(void);

int sorterFn(const void* a, const void* b)  {
	return ((ScanlineCB_Entry*)(a))->line < ((ScanlineCB_Entry*)(b))->line;
}

void resort(void) {
	qsort(scanline_callbacks, scanlineCount, sizeof(ScanlineCB_Entry), sorterFn);
}

scanlinecb_t vm_AddScanlineCallback(line_t line, ScanlineCallback callback) {
	ScanlineCB_Entry* entry;

	if(scanlineCount == NUM_CALLBACKS)
		return NULL;

	entry = &scanline_callbacks[scanlineCount++];
	entry->line = line;
	entry->callback = callback;

	if(scanlineCount == 1) {
		// setup a first scanline counter
		mmc5_sl_counter = line;
		mmc5_irq_ctrl = 0x80;
	} else {
		// we might have to move the current callback ptr
		if(line < vc_sl_ptr->line)
			++vc_sl_ptr;

		resort();
	}
}

void vm_RemoveScanlineCallback(scanlinecb_t cb) {
	ScanlineCB_Entry* entry = (ScanlineCB_Entry*)cb;

	if(scanlineCount == 1) {
		// shutdown the callback
		mmc5_irq_ctrl = 0x00;
	} else {
		entry->line = 0xFF; // move it to the end in resort

		// we might have to move the current callback ptr
		if(entry->line < vc_sl_ptr->line)
			--vc_sl_ptr;

		resort();
	}

	--scanlineCount;
}

/*void handle_hblank(void) {
	if(!scanlineCount)
		return;

	// Call the current CB and setup next
	ScanlineCB_Entry* entry = mmc_sl_ptr++;
	entry->callback(entry->line);

	if(++mmc_sl_i == scanlineCount)
		mmc_sl_ptr = scanline_callbacks;

	mmc5_sl_counter = mmc_sl_ptr->line - entry->line;
	mmc5_irq_ctrl = 0x80;
}*/

#include "entity.h"
#include "entities.h"
#include "xdata.h"
#include "map.h"

// An entity handling xdata MUST be loaded before it's triggered by an ent_XData
xdata_handler_t xdata_handler;

extern map_header_t* map_header;

typedef struct xdata_s {
	void* entries[8];
} xdata_t;

void xdata_SetHandler(xdata_handler_t handler) {
	xdata_handler = handler;
}

void xdata_Trigger(char id) {
	xdata_handler(map_header->xdata->entries[id]);
}

void ent_XData(entity_t* this, va_list args) {
	char tile;

	// throw out X,Y
	va_arg(args, int);
	va_arg(args, int);

	// ID is lowest 2 bits and LSB of upper byte
	tile = va_arg(args, char);
	xdata_Trigger((tile & 3) | ((tile >> 2) & 4));

	e_Destroy(this);
}
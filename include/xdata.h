#include <stdarg.h>

// Allow entities to have a parameter in map data, pointing to an xdata ref

typedef void (*xdata_handler_t)(const void* xdata);

extern void xdata_SetHandler(xdata_handler_t handler);
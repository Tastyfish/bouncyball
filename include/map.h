typedef enum {
	MO_VERTICAL = 0,
	MO_HORIZONTAL = 1
} MapOrientation;

void map_Load(const char* base, int rx, int ry);
void map_SetOrientation(MapOrientation orientation);
void map_MoveTo(int rx, int ry);

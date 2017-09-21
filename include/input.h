typedef enum {
	INPUT_PLAYER_0	= 0x16,
	INPUT_PLAYER_1	= 0x17
} i_player_t;

typedef enum {
	INPUT_A			= 0x80,
	INPUT_B			= 0x40,
	INPUT_SELECT	= 0x20,
	INPUT_START		= 0x10,
	INPUT_UP		= 0x08,
	INPUT_DOWN		= 0x04,
	INPUT_LEFT		= 0x02,
	INPUT_RIGHT		= 0x01
} input_t;

input_t __fastcall__ i_GetStandardInput(i_player_t player);

typedef char i_player_t;
enum {
	INPUT_PLAYER_0	= (i_player_t)0x16,
	INPUT_PLAYER_1	= (i_player_t)0x17
};

typedef char input_t;
enum {
	INPUT_A			= (input_t)0x80,
	INPUT_B			= (input_t)0x40,
	INPUT_SELECT	= (input_t)0x20,
	INPUT_START		= (input_t)0x10,
	INPUT_UP		= (input_t)0x08,
	INPUT_DOWN		= (input_t)0x04,
	INPUT_LEFT		= (input_t)0x02,
	INPUT_RIGHT		= (input_t)0x01
};

input_t __fastcall__ i_GetStandardInput(i_player_t player);

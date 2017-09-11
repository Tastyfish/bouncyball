typedef unsigned char i_player_t;
typedef unsigned char input_t;

#define INPUT_PLAYER_0	((i_player_t)0x16)
#define INPUT_PLAYER_1	((i_player_t)0x17)

#define INPUT_A			((input_t)0x80)
#define INPUT_B			((input_t)0x40)
#define INPUT_SELECT	((input_t)0x20)
#define INPUT_START		((input_t)0x10)
#define INPUT_UP		((input_t)0x08)
#define INPUT_DOWN		((input_t)0x04)
#define INPUT_LEFT		((input_t)0x02)
#define INPUT_RIGHT		((input_t)0x01)

input_t __fastcall__ i_GetStandardInput(i_player_t player);

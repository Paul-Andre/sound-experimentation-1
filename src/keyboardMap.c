#include "keyboardMap.h"


OptionalKeyPosition mapScancode(SDL_Scancode s){
	KeyPosition k;
	if (s>=SDL_SCANCODE_1 && s<=SDL_SCANCODE_0){
		k.x=s-SDL_SCANCODE_1;
		k.y=3;
	}
	else {
		switch(s){
			case SDL_SCANCODE_LSHIFT: case SDL_SCANCODE_NONUSBACKSLASH:
				k=(KeyPosition){-1,0};
			break;	
			case SDL_SCANCODE_Z:
				k=(KeyPosition){0,0};
			break;	
			case SDL_SCANCODE_X:
				k=(KeyPosition){1,0};
			break;	
			case SDL_SCANCODE_C:
				k=(KeyPosition){2,0};
			break;	
			case SDL_SCANCODE_V:
				k=(KeyPosition){3,0};
			break;
			case SDL_SCANCODE_B:
				k=(KeyPosition){4,0};
			break;	
			case SDL_SCANCODE_N:
				k=(KeyPosition){5,0};
			break;	
			case SDL_SCANCODE_M:
				k=(KeyPosition){6,0};
			break;	
			case SDL_SCANCODE_COMMA:
				k=(KeyPosition){7,0};
			break;	
			case SDL_SCANCODE_PERIOD:
				k=(KeyPosition){8,0};
			break;	
			case SDL_SCANCODE_SLASH:
				k=(KeyPosition){9,0};
			break;	
			case SDL_SCANCODE_RSHIFT:
				k=(KeyPosition){10,0};
			break;	
			
			
		
			case SDL_SCANCODE_A:
				k=(KeyPosition){0,1};
			break;	
			case SDL_SCANCODE_S:
				k=(KeyPosition){1,1};
			break;	
			case SDL_SCANCODE_D:
				k=(KeyPosition){2,1};
			break;	
			case SDL_SCANCODE_F:
				k=(KeyPosition){3,1};
			break;	
			case SDL_SCANCODE_G:
				k=(KeyPosition){4,1};
			break;
			case SDL_SCANCODE_H:
				k=(KeyPosition){5,1};
			break;	
			case SDL_SCANCODE_J:
				k=(KeyPosition){6,1};
			break;	
			case SDL_SCANCODE_K:
				k=(KeyPosition){7,1};
			break;	
			case SDL_SCANCODE_L:
				k=(KeyPosition){8,1};
			break;
			case SDL_SCANCODE_SEMICOLON:
				k=(KeyPosition){9,1};
			break;	
			case SDL_SCANCODE_APOSTROPHE:
				k=(KeyPosition){10,1};
			break;	
			
			
		
			case SDL_SCANCODE_Q:
				k=(KeyPosition){0,2};
			break;	
			case SDL_SCANCODE_W:
				k=(KeyPosition){1,2};
			break;	
			case SDL_SCANCODE_E:
				k=(KeyPosition){2,2};
			break;	
			case SDL_SCANCODE_R:
				k=(KeyPosition){3,2};
			break;	
			case SDL_SCANCODE_T:
				k=(KeyPosition){4,2};
			break;
			case SDL_SCANCODE_Y:
				k=(KeyPosition){5,2};
			break;	
			case SDL_SCANCODE_U:
				k=(KeyPosition){6,2};
			break;	
			case SDL_SCANCODE_I:
				k=(KeyPosition){7,2};
			break;
			case SDL_SCANCODE_O:
				k=(KeyPosition){8,2};
			break;
			case SDL_SCANCODE_P:
				k=(KeyPosition){9,2};
			break;
			case SDL_SCANCODE_LEFTBRACKET:
				k=(KeyPosition){10,2};
			break;
			case SDL_SCANCODE_RIGHTBRACKET:
				k=(KeyPosition){11,2};
			break;
			
			
			case SDL_SCANCODE_MINUS:
				k=(KeyPosition){10,3};
			break;
			case SDL_SCANCODE_EQUALS:
				k=(KeyPosition){11,3};
			break;
			case SDL_SCANCODE_BACKSPACE:
				k=(KeyPosition){12,3};
			break;
			
			
			
			default:
				return (OptionalKeyPosition){false, (KeyPosition){0, 0}};
			
		}
	}
	
	return (OptionalKeyPosition){true, k};


}

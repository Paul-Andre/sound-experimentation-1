#ifndef KEYBOARD_MAP_H
#define KEYBOARD_MAP_H
#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

typedef struct KeyPosition{
	int x;
	int y;
}KeyPosition;

typedef struct OptionalKeyPosition{
	bool some;
	KeyPosition position;
}OptionalKeyPosition;

OptionalKeyPosition mapScancode(SDL_Scancode);


#endif

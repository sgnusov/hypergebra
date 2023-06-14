#include "graphics.h"

int main() {

	if(initGraphics())
		return -1;

	return mainLoop();
}

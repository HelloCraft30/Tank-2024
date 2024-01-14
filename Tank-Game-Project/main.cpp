#include<SFML\Graphics.hpp>
#include "game.h"
#include "mainmenu.h"
#include<iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include "app.h"

int main() {
	SoundManager sm;
	sm.Switch_Sound_Theme();
	app App;
	App.run(sm);

	return 0;
}
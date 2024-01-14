#pragma once
#include<SFML\Graphics.hpp>
#include "game.h"
#include "mainmenu.h"
#include<iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include "SoundEffects.h"

struct app
{
	void run(SoundManager& sm);
};


#pragma once
#include "game.h"

struct mainmenu{
	mainmenu();
	std::string screenCode;
	sf::Vector2i coords;
	sf::RenderWindow* window;
	sf::Texture bg_texture;
	sf::RectangleShape bg;
	std::vector<sf::Text> texts;
	sf::Font fnt;
	
	void display(SoundManager& sm);
};

struct newgame {
	newgame();
	sf::RenderWindow* window;
	sf::Texture bg_texture;
	sf::RectangleShape bg;
	sf::Text title;
	std::vector<sf::Text> texts;
	sf::Font fnt;
	sf::RectangleShape nameBox;
	sf::Vector2f nameCoord;
	//output
	std::string screenCode;
	sf::Vector2i coords;

	std::string name;
	int difficulty;

	void display(SoundManager& sm);
};
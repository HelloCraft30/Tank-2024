#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <fstream>
#include <iostream>


class SoundManager {
public:

	//Global song check if it is ON or OFF
	bool Sound_On = true;
	bool Sound_Theme_On = true;

	SoundManager();

	//SOUND
	
	//Play sound
	void Awarding_Bonus_Play();
	void Bred_Bricks_Hit_Play();
	void Button_Play();
	void Displayed_Score_Play();
	void Enemy_Hit_Play();
	void Enemy_Tank_Explosion_Play();
	void Fire_Play();
	void Game_Over_Play();
	void Player_Being_Hit_Play();
	void Player_Respawn_Play();
	void Portal_Sound_Play();
	void Start_Up_Play();
	void Wall_Destroyed_Play();

	//Switch sound effects/theme - turn off sound effects/theme when it is playing or turn on when it is not playing
	void Switch_Sound_Effects();
	void Switch_Sound_Theme();

	//Play/Stop Sound Theme
	void Sound_Theme_Play();
	void Stop_Sound_Theme();

private:
	//Turn off/on all sound/music in the game
	void Turn_Off_Sound();
	void Turn_On_Sound();

	//initial soundBuffer and Sound of each sound effect
	//'sb' stand for 'sound buffer' 

	sf::SoundBuffer sb_Awarding_Bonus;
	sf::Sound Awarding_Bonus;

	sf::SoundBuffer sb_Bred_Bricks_Hit;
	sf::Sound Bred_Bricks_Hit;

	sf::SoundBuffer sb_Displayed_Score;
	sf::Sound Displayed_Score;

	sf::SoundBuffer sb_Button;
	sf::Sound Button;

	sf::SoundBuffer sb_Enemy_Hit;
	sf::Sound Enemy_Hit;

	sf::SoundBuffer sb_Enemy_Tank_Explosion;
	sf::Sound Enemy_Tank_Explosion;

	sf::SoundBuffer sb_Fire;
	sf::Sound Fire;

	sf::SoundBuffer sb_Game_Over;
	sf::Sound Game_Over;

	sf::SoundBuffer sb_Player_Being_Hit;
	sf::Sound Player_Being_Hit;

	sf::SoundBuffer sb_Player_Respawn;
	sf::Sound Player_Respawn;

	sf::SoundBuffer sb_Portal_Sound;
	sf::Sound Portal_Sound;

	sf::SoundBuffer sb_Start_Up;
	sf::Sound Start_Up;

	sf::SoundBuffer sb_Wall_Destroyed;
	sf::Sound Wall_Destroyed;

	//Sound Theme playing in background
	sf::SoundBuffer sb_Sound_Theme;
	sf::Sound Sound_Theme;
};

#include "SoundEffects.h"

//Initial sound 
SoundManager::SoundManager() {

	//Sound Effects
	if (!sb_Awarding_Bonus.loadFromFile("sound/Awarding_Bonus.wav")) {
		std::cout << "ERROR: sounds/Awarding_Bonus.wav didn't load" << std::endl;
	}
	else {
		Awarding_Bonus.setBuffer(sb_Awarding_Bonus);
	}

	if (!sb_Bred_Bricks_Hit.loadFromFile("sound/Bred_Bricks_Hit.wav")) {
		std::cout << "ERROR: sounds/Bred_Bricks_Hit.wav didn't load" << std::endl;
	}
	else {
		Bred_Bricks_Hit.setBuffer(sb_Bred_Bricks_Hit);
	}

	if (!sb_Displayed_Score.loadFromFile("sound/Displayed_Score.wav")) {
		std::cout << "ERROR: sounds/Displayed_Score.wav didn't load" << std::endl;
	}
	else {
		Displayed_Score.setBuffer(sb_Displayed_Score);
	}

	if (!sb_Button.loadFromFile("sound/Button.wav")) {
		std::cout << "ERROR: sounds/Button.wav didn't load" << std::endl;
	}
	else {
		Button.setBuffer(sb_Button);
	}

	if (!sb_Enemy_Hit.loadFromFile("sound/Enemy_Hit.wav")) {
		std::cout << "ERROR: sounds/Enemy_Hit.wav didn't load" << std::endl;
	}
	else {
		Enemy_Hit.setBuffer(sb_Enemy_Hit);
	}

	if (!sb_Enemy_Tank_Explosion.loadFromFile("sound/Enemy_Tank_Explosion.wav")) {
		std::cout << "ERROR: sounds/Enemy_Tank_Explosion.wav didn't load" << std::endl;
	}
	else {
		Enemy_Tank_Explosion.setBuffer(sb_Enemy_Tank_Explosion);
	}

	if (!sb_Fire.loadFromFile("sound/Fire.wav")) {
		std::cout << "ERROR: sounds/Fire.wav didn't load" << std::endl;
	}
	else {
		Fire.setBuffer(sb_Fire);
	}

	if (!sb_Game_Over.loadFromFile("sound/Game_Over.wav")) {
		std::cout << "ERROR: sounds/Game_Over.wav didn't load" << std::endl;
	}
	else {
		Game_Over.setBuffer(sb_Game_Over);
	}

	if (!sb_Player_Being_Hit.loadFromFile("sound/Player_Being_Hit.wav")) {
		std::cout << "ERROR: sounds/Player_Being_Hit.wav didn't load" << std::endl;
	}
	else {
		Player_Being_Hit.setBuffer(sb_Player_Being_Hit);
	}

	if (!sb_Player_Respawn.loadFromFile("sound/Player_Respawn.wav")) {
		std::cout << "ERROR: sounds/Player_Respawn.wav didn't load" << std::endl;
	}
	else {
		Player_Respawn.setBuffer(sb_Player_Respawn);
	}

	if (!sb_Portal_Sound.loadFromFile("sound/Portal_Sound.wav")) {
		std::cout << "ERROR: sounds/Portal_Sound.wav didn't load" << std::endl;
	}
	else {
		Portal_Sound.setBuffer(sb_Portal_Sound);
	}

	if (!sb_Start_Up.loadFromFile("sound/Start_Up.wav")) {
		std::cout << "ERROR: sounds/Start_Up.wav didn't load" << std::endl;
	}
	else {
		Start_Up.setBuffer(sb_Start_Up);
	}

	if (!sb_Wall_Destroyed.loadFromFile("sound/Wall_Destroyed.wav")) {
		std::cout << "ERROR: sounds/Wall_Destroyed.wav didn't load" << std::endl;
	}
	else {
		Wall_Destroyed.setBuffer(sb_Wall_Destroyed);
	}

	//Sound Theme
	if (!sb_Sound_Theme.loadFromFile("sound/Sound_Theme.wav")) {
		std::cout << "ERROR: sounds/Sound_Theme.wav didn't load" << std::endl;
	}
	else {
		Sound_Theme.setBuffer(sb_Sound_Theme);
	}
}

//Play sound effects
void SoundManager::Awarding_Bonus_Play() { Awarding_Bonus.play(); }

void SoundManager::Bred_Bricks_Hit_Play() { Bred_Bricks_Hit.play(); }

void SoundManager::Button_Play() { Button.play(); }

void SoundManager::Displayed_Score_Play() { Displayed_Score.play(); }

void SoundManager::Enemy_Hit_Play() { Enemy_Hit.play(); }

void SoundManager::Enemy_Tank_Explosion_Play() { Enemy_Tank_Explosion.play(); }

void SoundManager::Fire_Play() { Fire.play(); }

void SoundManager::Game_Over_Play() { Game_Over.play(); }

void SoundManager::Player_Being_Hit_Play() { Player_Being_Hit.play(); }

void SoundManager::Player_Respawn_Play() { Player_Respawn.play(); }

void SoundManager::Portal_Sound_Play() { Portal_Sound.play(); }

void SoundManager::Start_Up_Play() { Start_Up.play(); }

void SoundManager::Wall_Destroyed_Play() { Wall_Destroyed.play(); }

//Play sound theme
void SoundManager::Sound_Theme_Play() {
	Sound_Theme.setLoop(true);
	Sound_Theme.setVolume(70);
	Sound_Theme.play();
}


void SoundManager::Stop_Sound_Theme() { Sound_Theme.stop(); }

//Turn off/on all sound/music in the game
void SoundManager::Turn_Off_Sound() {
	sf::Listener::setGlobalVolume(0.0f);
}

void SoundManager::Turn_On_Sound() {
	sf::Listener::setGlobalVolume(100.0f);
}

//Switch Sound Effects/Theme
void SoundManager::Switch_Sound_Effects() {
	if (Sound_On) Turn_Off_Sound();
	else Turn_On_Sound();

	Sound_On = !Sound_On;
}

void SoundManager::Switch_Sound_Theme() {
	if (Sound_Theme_On) Sound_Theme_Play();
	else Stop_Sound_Theme();

	Sound_Theme_On = !Sound_Theme_On;
}

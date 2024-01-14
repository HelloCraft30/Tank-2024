#pragma once
#include<SFML/Graphics.hpp>
#include <vector>
#include<fstream>
#include<string>
#include<Windows.h>
#include <iostream>
#include "SoundEffects.h"

#define SIZEMAP 640

bool checkCollision(const sf::RectangleShape& a, const sf::RectangleShape& b);

struct game_data {
	game_data();
	game_data(int nE, int nM, int diff);
	void update();
	int diffCode;
	float enemy_speed;
	int enemy_shoot_cooldown;
	int n_enemy_to_keel;
	int n_enemy_max_screen;
	int enemy_bullet_spd;
	int lifes;
};

struct player_data {
	player_data();
	player_data(float spd, int rld, int n, int spdB, const std::string& name, int score);

	std::string name;
	int stats[4]{};
	int score;
};

class game
{
public:
	game();
	game(game_data& gData, player_data& pData, int level, int saveSlot);

	void init();

	void loop(SoundManager& sm);

	void draw();

	void collision(SoundManager& sm);

	//------------------------------------------Window

	std::string screenCode = "MAIN MENU";

	sf::RenderWindow* _window;

	sf::Texture tank_texture;

	sf::RectangleShape background;

	//------------------------------------------GAME

	int saveSlot;

	bool game_win = false;
	bool game_lose = false;


	sf::Text game_wl_text;

	bool infinite_enemies = false;

	bool mute_music_all = false;
	bool mute_music_bg = false;

	//------------------------------------------Pause
	bool game_pause = false;
	sf::Text pause_text;

	sf::RectangleShape pause_box;
	std::vector<sf::Text> pause_options;
	int pause_select = 0;

	//------------------------------------------Data-map
#pragma region 
	int levelCode = 0;
	int diffCode = 0;
	std::vector<std::vector<int>> mapData;
	void map_read_from_file(const std::string& nameFile);

	sf::RectangleShape mapShape;
	sf::Texture map_background;
	void map_set_game();
#pragma endregion
	//------------------------------------------player
#pragma region
	bool player_dead;
	int* player_score;
	int* lifes;
	std::string* player_name;
	sf::RectangleShape player;
	sf::IntRect player_rect;
	int player_dir;
	float player_spd;
	float* player_spd_basic;
	sf::Vector2f player_movement;

	bool immortal = false;
	int immortal_cooldown = 180;
	int immortal_t = 0;

	sf::RectangleShape shield;
	sf::Texture shield_texture;

	sf::RectangleShape spawn_box;
	sf::Texture spawn_box_texture;
	bool is_ef_spawn = false;
	int t_ef_spawn = 30;
	int t_ef_spawn_t = 0;

	bool player_is_debuff;
	int player_debuff_time;
	int player_debuff_time_t;

	int player_respawn_t;
	int player_respawn_count_t;

	sf::Vector2f player_spawn_point;

	int* player_n_bullets;
	int player_n_bullets_t;
	int player_n_bullet_cooldown;
	int player_n_bullet_cooldown_t;

	bool player_bullet_ready;
	int* player_shoot_cooldown_t;
	int player_shoot_cooldown_count_t;
#pragma endregion
	//------------------------------------------bullet
#pragma region
	std::vector<sf::RectangleShape> bullets;
	std::vector<sf::Vector2f> bullets_movement;
	float* bullet_spd;

	int bullet_pause_portal = 60;
	std::vector<int> bullet_pause_portal_t;


	sf::RectangleShape m_bullet;
#pragma endregion
	//------------------------------------------Enemy-Bullet
#pragma region
	std::vector<sf::RectangleShape> eBullets;
	std::vector<sf::Vector2f> eBullets_movement;
	float eBullet_spd;

	int eBullet_pause_portal = 60;
	std::vector<int> eBullet_pause_portal_t;

	sf::RectangleShape m_eBullet;
#pragma endregion
	//------------------------------------------enemy
#pragma region
	int n_enemy_game;

	int n_enemy_limit_spawn;
	int n_enemy_current;

	std::vector<sf::RectangleShape> enemies;
	std::vector<sf::IntRect> enemies_rect;
	std::vector<int> enemies_dir;
	std::vector<sf::Vector2f> enemies_movement;
	float enemy_spd;
	//randome dir
	std::vector<int> enemies_random_dir_t;
	std::vector<int> enemies_count_t;
	//random shoot
	int enemy_shoot_change;
	std::vector<int> enemies_random_shoot_t;
	std::vector<int> enemies_count_shoot_t;

	sf::RectangleShape m_enemy;
	sf::IntRect m_enemy_rect;
#pragma endregion
	//------------------------------------------Spawner
#pragma region
	std::vector<sf::Vector2f> spawners;

	int spawner_cooldown;
	int spawner_cooldown_t;

#pragma endregion
	//------------------------------------------Brick
#pragma region
	std::vector<sf::RectangleShape> bricks;
	std::vector<sf::IntRect> bricks_rect;

	sf::RectangleShape m_brick;
	int m_brick_hp;
	sf::IntRect m_brick_rect;
	std::vector<int> bricks_hp;
	sf::Texture brick_texture;
#pragma endregion
	//------------------------------------------Grass
#pragma region
	std::vector<sf::RectangleShape> grasses;
	sf::RectangleShape m_grass;
	sf::Texture grass_texture;
#pragma endregion
	//------------------------------------------Bedrock
#pragma region
	std::vector<sf::RectangleShape> bedrocks;
	sf::RectangleShape m_bedrock;
	sf::Texture bedrock_texture;
#pragma endregion
	//-----------------------------------------Portal
#pragma region
	std::vector<sf::RectangleShape> portals;
	std::vector<int> portals_des;

	std::vector<int> portals_ready;
	int portal_cooldown = 180;
	std::vector<int> portals_cooldown_t;

	std::vector<sf::IntRect> portals_rect;

	sf::RectangleShape m_portal;
	sf::IntRect m_portal_rect;

	sf::Texture portal_texture;
#pragma endregion
	//-----------------------------------------Home
#pragma region
	sf::RectangleShape home;
	sf::Texture home_texture;
	bool home_dead;
	bool home_exist;
#pragma endregion
	//-----------------------------------------Water
#pragma region
	std::vector<sf::RectangleShape> waters;

	sf::RectangleShape m_water;
	sf::Texture water_texture;

	float water_debuff;
#pragma endregion
	//-----------------------------------------Info board
	sf::Texture info_texture;
	sf::RectangleShape info_board;

	sf::Font fnt;
	std::vector<sf::Text> info_texts;

	//enemies - info
	int info_n_enemies;
	int info_n_less;
	std::vector<sf::RectangleShape> info_mini_enemies;
	std::vector<sf::IntRect> info_mini_rect;
	sf::Texture mini_enemy_texture;

	//leveling bars
	std::vector<std::vector<sf::RectangleShape>> bars;
	int* stats[4]{ };
	int max_stats[4]{ 5, 5, 10, 5 };
	bool maxed[4]{};

	sf::Text infinite_text;

	//------------------------------------------STATUS GAME
	//-------------SAVE
	void saveStatus(const std::string& fileName);
	//-------------LOAD
	void loadStatus(const std::string& fileName);

	//------------------------------------------Effects
	sf::RectangleShape m_shoot_effect;
	sf::IntRect m_shoot_ef_rect;
	sf::Texture m_shoot_texture;

	bool is_ef_shoot = false;
	int t_ef_shoot = 10;
	int t_ef_shoot_t = 0;
	//------------------------bomb
	sf::RectangleShape m_bomb_effect;
	sf::IntRect m_bomb_ef_rect;
	sf::Texture m_bomb_texture;

	std::vector<sf::RectangleShape> bombs_ef;
	std::vector<sf::IntRect> bombs_ef_rect;

	//75 tick
	std::vector<int> t_bombs_t;
	int t_bomb = 75;

	//enemies shoot effect haiza
	sf::RectangleShape m_Eshoot_effect;
	sf::IntRect m_Eshoot_ef_rect;
	sf::Texture m_Eshoot_texture;

	std::vector<sf::RectangleShape> eshoots_effect;
	std::vector<sf::IntRect> eshoots_effect_rect;
	std::vector<int> t_Eshoot_t;
	std::vector<bool> is_ef_eshoots;

};

class stats_screen {
public:
	int* stats;
	int max_stats[4]{ 5, 5, 10, 5 };
	int* currency;

	sf::RenderWindow* window;
	sf::Texture bg_texture;
	sf::RectangleShape bg;

	//visible

	std::vector<std::vector<sf::RectangleShape>> bars;

	std::vector<sf::Text> buttons;

	std::vector<sf::Text> texts;
	sf::Font fnt;
	int select;
	bool isMaxed[4]{};

	sf::Text hint;

	stats_screen();
	stats_screen(int* pointer, int* score);

	void loop(SoundManager& sm);
	void draw();
};


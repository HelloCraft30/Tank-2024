#include "game.h"

#define mapOffSet 30

game_data::game_data() {
	n_enemy_max_screen = n_enemy_to_keel = 1;
	enemy_speed = 1.5f;
	enemy_shoot_cooldown = 120;
	enemy_bullet_spd = 3.f;
	diffCode = 0;
	lifes = 5;
}

void game_data::update() {
	switch (diffCode) {
	case 0://easy
		enemy_speed = 1.5f;
		enemy_shoot_cooldown = 0;
		enemy_bullet_spd = 3.f;
		lifes = 5;
		break;
	case 1://medium
		enemy_speed = 3.f;
		enemy_shoot_cooldown = 30;
		enemy_bullet_spd = 6.f;
		lifes = 3;
		break;
	case 2://hard
		enemy_speed = 4.f;
		enemy_shoot_cooldown = 60;
		enemy_bullet_spd = 10.f;
		lifes = 1;
		break;
	default:
		enemy_speed = 1.5f;
		enemy_shoot_cooldown = 120;
		enemy_bullet_spd = 3.f;
		lifes = 5;
	}
}

game_data::game_data(int nE, int nM, int diff) {
	diffCode = diff;
	switch (diff) {
	case 0://easy
		enemy_speed = 1.5f;
		enemy_shoot_cooldown = 0;
		enemy_bullet_spd = 3.f;
		lifes = 5;
		break;
	case 1://medium
		enemy_speed = 3.f;
		enemy_shoot_cooldown = 30;
		enemy_bullet_spd = 6.f;
		lifes = 3;
		break;
	case 2://hard
		enemy_speed = 4.f;
		enemy_shoot_cooldown = 60;
		enemy_bullet_spd = 10.f;
		lifes = 1;
		break;
	default:
		enemy_speed = 1.5f;
		enemy_shoot_cooldown = 120;
		enemy_bullet_spd = 3.f;
		lifes = 5;
	}
	n_enemy_max_screen = nM;
	n_enemy_to_keel = nE;
}

player_data::player_data() {
	stats[0] = stats[1] = stats[2] = stats[3] = score = 0;
	name = "";
}

player_data::player_data(float spd, int rld, int n, int spdB, const std::string& _name, int _score) {
	stats[0] = spd;
	stats[1] = rld;
	stats[2] = n;
	stats[3] = spdB;
	name = _name;
	score = _score;
}

game::game() {
	enemy_spd = 0;
	n_enemy_game = 0;
	n_enemy_limit_spawn = 0;
	enemy_shoot_change = 0;
	eBullet_spd = 0;
	levelCode = 0;
	diffCode = 0;
	player_score = 0;
	lifes = 0;
	saveSlot = 0;
	init();
	map_set_game();
}

game::game(game_data& gData, player_data& pData, int level, int saveS) {
	levelCode = level;
	diffCode = gData.diffCode;
	enemy_spd = gData.enemy_speed;
	n_enemy_game = gData.n_enemy_to_keel;
	n_enemy_limit_spawn = gData.n_enemy_max_screen;
	enemy_shoot_change = gData.enemy_shoot_cooldown;
	eBullet_spd = gData.enemy_bullet_spd;
	//player
	player_score = &pData.score;
	player_name = &pData.name;
	player_spd_basic = new float;
	player_shoot_cooldown_t = new int;
	player_n_bullets = new int;
	bullet_spd = new float;
	lifes = &gData.lifes;
	for (int i = 0; i < 4; i++) {
		stats[i] = &pData.stats[i];
	}
	if (n_enemy_game == -1) infinite_enemies = true;
	saveSlot = saveS;
	init();
	map_set_game();
}

void addText(std::vector<sf::Text>& des, const std::string& text, sf::Font& fnt, int x, int y, sf::Color cl) {
	sf::Text tmp(text.c_str(), fnt, 30);
	int n = des.size();
	int yoffset = 30;
	//setup text
	tmp.setFillColor(cl);
	tmp.setString(text);
	tmp.setPosition({ x - (float)tmp.getLocalBounds().width / 2,(float)(y + yoffset + n * 35) });
	des.push_back(tmp);
}

void game::init() {
	//------------------------------------------window
	_window = new sf::RenderWindow(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
	_window->setFramerateLimit(60);
	tank_texture.loadFromFile("png/tank.png");
	map_background.loadFromFile("png/background.png");
	mapShape.setSize({ 800,640 });
	mapShape.setTexture(&map_background);
	mapShape.setOutlineThickness(5);
	mapShape.setOutlineColor(sf::Color::White);
	mapShape.setPosition({ 30,30 });

	pause_text.setCharacterSize(50);
	pause_text.setFillColor(sf::Color::Black);
	pause_text.setString("PAUSED");
	pause_text.setFont(fnt);
	pause_text.setOutlineColor(sf::Color::White);
	pause_text.setOutlineThickness(4);
	pause_text.setPosition(mapShape.getSize().x / 2 - 70,
		mapShape.getSize().y / 2 - 25);

	game_wl_text;
	game_wl_text.setCharacterSize(50);
	game_wl_text.setFillColor(sf::Color::Black);
	game_wl_text.setString("COMPLETED MAP");
	game_wl_text.setFont(fnt);
	game_wl_text.setOutlineColor(sf::Color::White);
	game_wl_text.setOutlineThickness(4);
	game_wl_text.setPosition(
		30 + (mapShape.getSize().x) / 2 - 220,
		30 + (mapShape.getSize().y) / 2 - 25);
	//-------------------------------------------------------GAME BOARD
#pragma region GAME_BOARD
	//------------------------------------------player
	player.setSize({ 32,32 });
	player_rect.top = 0; player_rect.left = 0; player_rect.width = 32; player_rect.height = 32;
	player.setTextureRect(player_rect);
	player.setTexture(&tank_texture);
	player.setPosition({ 0 + mapOffSet,0 + mapOffSet });

	//update depend on stats
		//[0]
	(*player_spd_basic) = (*stats[0]) * 2.f;
	//[1]
	(*player_shoot_cooldown_t) = 115 - (15 * *stats[1]);
	//[2]
	(*player_n_bullets) = *stats[2];
	//[3]
	(*bullet_spd) = 3 + *stats[3];

	player_dir = 0;
	player_spd = *player_spd_basic;
	player_dead = false;
	player_respawn_count_t = 0;
	player_respawn_t = 300;
	player_shoot_cooldown_count_t = 0;
	player_n_bullets_t = *player_n_bullets;
	player_n_bullet_cooldown = 4;
	player_n_bullet_cooldown_t = 0;
	player_is_debuff = false;
	player_debuff_time = 45;
	player_debuff_time_t = 0;
	immortal = true;
	shield_texture.loadFromFile("png/shield.png");
	shield.setSize({ 32,32 });
	shield.setTexture(&shield_texture);
	spawn_box_texture.loadFromFile("png/spawning_effect.png");
	spawn_box.setSize({ 32,32 });
	spawn_box.setTexture(&spawn_box_texture);
	//------------------------------------------bedrock
	bedrock_texture.loadFromFile("png/bedrock.png");
	m_bedrock.setSize({ 32,32 });
	m_bedrock.setTexture(&bedrock_texture);
	//------------------------------------------grass
	grass_texture.loadFromFile("png/grass.png");
	m_grass.setSize({ 32,32 });
	m_grass.setTexture(&grass_texture);
	//------------------------------------------brick
	brick_texture.loadFromFile("png/brick.png");
	m_brick.setSize({ 32,32 });
	m_brick_rect.top = 0; m_brick_rect.left = 0; m_brick_rect.width = 32; m_brick_rect.height = 32;
	m_brick.setTextureRect(m_brick_rect);
	m_brick.setTexture(&brick_texture);
	m_brick_hp = 2;
	//------------------------------------------portal
	portal_texture.loadFromFile("png/portal.png");
	m_portal.setSize({ 32,32 });
	m_portal_rect.top = 0; m_portal_rect.left = 0; m_portal_rect.width = 32; m_portal_rect.height = 32;
	m_portal.setTextureRect(m_portal_rect);
	m_portal.setTexture(&portal_texture);
	//------------------------------------------bullet
	m_bullet.setSize({ 6,6 });
	m_bullet.setFillColor(sf::Color::White);
	m_bullet.setOutlineThickness(2);
	m_bullet.setOutlineColor(sf::Color::Black);
	m_bullet.setOrigin({ -12,-12 });
	//------------------------------------------eBullet
	m_eBullet.setSize({ 6,6 });
	m_eBullet.setFillColor(sf::Color::Red);
	m_eBullet.setOutlineThickness(2);
	m_eBullet.setOutlineColor(sf::Color::Black);
	m_eBullet.setOrigin({ -12,-12 });

	//------------------------------------------enemy
	m_enemy.setSize({ 32,32 });
	m_enemy_rect.top = 0; m_enemy_rect.left = 128; m_enemy_rect.width = 32; m_enemy_rect.height = 32;
	m_enemy.setTextureRect(m_enemy_rect);
	player.setTexture(&tank_texture);
	//------------------------------------------home
	home_dead = false;
	home.setSize({ 64,64 });
	home_texture.loadFromFile("png/home.png");
	home.setTexture(&home_texture);
	home_exist = false;
	//-----------------------------------------water
	water_texture.loadFromFile("png/water.png");
	m_water.setSize({ 32,32 });
	m_water.setTexture(&water_texture);
	water_debuff = 1.5f;
	//-----------------------------------------spawner
	n_enemy_current = 0;

	spawner_cooldown = 120;
	spawner_cooldown_t = 0;

#pragma endregion
	//-------------------------------------------------------INFO BOARD
	info_texture.loadFromFile("png/info_background.png");
	info_board.setSize({ 348,640 });
	info_board.setPosition({ 870,30 });
	info_board.setTexture(&info_texture);
	info_board.setOutlineColor(sf::Color::White);
	info_board.setOutlineThickness(5);
	//texts
	fnt.loadFromFile("font/MobileFont.ttf");
	sf::Vector2f info_coord = info_board.getPosition();
	info_coord.x = info_coord.x + info_board.getSize().x / 2;
	std::string temp;
	if (levelCode != -1)
		temp = "LEVEL:    " + std::to_string(levelCode);
	else temp = "LEVEL: Infinitie";
	addText(info_texts, temp, fnt, info_coord.x, info_coord.y, sf::Color::White);
	info_texts[0].setOutlineThickness(3);
	info_texts[0].setOutlineColor(sf::Color::Black);
	temp = "MODE:    ";
	switch (diffCode) {
	case 0: temp += "EASY"; addText(info_texts, temp, fnt, info_coord.x, info_coord.y, sf::Color::Green);
		break;
	case 1: temp += "MEDIUM"; addText(info_texts, temp, fnt, info_coord.x, info_coord.y, sf::Color::Yellow);
		break;
	case 2: temp += "HARD"; addText(info_texts, temp, fnt, info_coord.x, info_coord.y, sf::Color::Red);
		break;
	}
	info_texts[1].setOutlineThickness(3);
	info_texts[1].setOutlineColor(sf::Color::Black);
	addText(info_texts, "ENEMIES:", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	addText(info_texts, "", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	addText(info_texts, "", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	addText(info_texts, "PLAYER INFO:", fnt, info_coord.x, info_coord.y, sf::Color::Blue);
	addText(info_texts, *player_name, fnt, info_coord.x, info_coord.y, sf::Color::Green);
	info_texts[6].setOutlineThickness(3);
	info_texts[6].setOutlineColor(sf::Color::Black);
	addText(info_texts, "Score:   ", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	info_texts[7].setPosition(info_board.getPosition().x + 30, info_texts[7].getPosition().y);

	addText(info_texts, "Lifes:  ", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	info_texts[8].setPosition(info_board.getPosition().x + 30, info_texts[8].getPosition().y);

	addText(info_texts, "Speed:", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	info_texts[9].setPosition(info_board.getPosition().x + 30, info_texts[9].getPosition().y);
	addText(info_texts, "", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	addText(info_texts, "Reload:", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	info_texts[11].setPosition(info_board.getPosition().x + 30, info_texts[11].getPosition().y);
	addText(info_texts, "", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	addText(info_texts, "Bullets:", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	info_texts[13].setPosition(info_board.getPosition().x + 30, info_texts[13].getPosition().y);
	addText(info_texts, "", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	addText(info_texts, "Canon-strength:", fnt, info_coord.x, info_coord.y, sf::Color::Black);
	info_texts[15].setPosition(info_board.getPosition().x + 30, info_texts[15].getPosition().y);
	addText(info_texts, "", fnt, info_coord.x, info_coord.y, sf::Color::Black);

	//enemies info
	mini_enemy_texture.loadFromFile("png/mini_tank.png");
	sf::RectangleShape m_mini_enemy;
	sf::IntRect m_mini_enemy_rect;
	m_mini_enemy_rect.top = m_mini_enemy_rect.left = 0;
	m_mini_enemy_rect.width = m_mini_enemy_rect.height = 8;
	m_mini_enemy.setSize({ 24,24 });
	m_mini_enemy.setTextureRect(m_mini_enemy_rect);
	m_mini_enemy.setTexture(&mini_enemy_texture);
	//{ x - (float)tmp.getLocalBounds().width / 2,(float)(y + yoffset + n * 35) }
	info_n_less = n_enemy_game;
	for (int i = 0; i < n_enemy_game; i++) {
		info_mini_enemies.push_back(m_mini_enemy);
		info_mini_rect.push_back(m_mini_enemy_rect);
		if (i <= 9) {
			info_mini_enemies[info_mini_enemies.size() - 1].setPosition(
				info_coord.x + i * 30 - 140, info_coord.y + 30 + 3 * 35
			);
		}
		else {
			info_mini_enemies[info_mini_enemies.size() - 1].setPosition(
				info_coord.x + (i - 10) * 30 - 140, info_coord.y + 30 + 4 * 35
			);
		}
	}
	//stats
	float bar_width = 280;
	float bar_coordX = info_board.getPosition().x + 30;
	float bar_coordY[4]{};
	for (int i = 0; i < 4; i++) {
		bar_coordY[i] = (float)(info_coord.y + 30 + (9 + (i + (float)1 / 2) * 2) * 35) + 10;
	}
	for (int i = 0; i < 4; i++) {
		std::vector<sf::RectangleShape> bar;
		float dot_width = (bar_width * 4) / (5 * max_stats[i]);
		sf::RectangleShape dot;
		dot.setSize({ dot_width, 20 });
		dot.setFillColor(sf::Color::Black);
		for (int j = 0; j < max_stats[i]; j++) {
			bar.push_back(dot);
		}
		for (int j = 0; j < max_stats[i]; j++) {
			bar[j].setPosition({ bar_coordX + j * (dot_width + dot_width / 4), bar_coordY[i] });
		}
		bars.push_back(bar);
	}

	infinite_text.setFont(fnt);
	infinite_text.setCharacterSize(50);
	infinite_text.setString("Infinity");
	infinite_text.setFillColor(sf::Color::White);
	infinite_text.setOutlineColor(sf::Color::Black);
	infinite_text.setOutlineThickness(2);
	infinite_text.setPosition(info_texts[3].getPosition().x - 100, info_texts[3].getPosition().y);


	//----------------------------------------stage pause
	pause_box.setSize({ 330, 450 });
	pause_box.setPosition({ 879, 640 - 450 + 20 });
	pause_box.setTexture(&info_texture);
	pause_box.setOutlineColor(sf::Color::Black);
	pause_box.setOutlineThickness(2);

	int _pause_x = pause_box.getPosition().x + pause_box.getSize().x / 2;
	int _pause_y = pause_box.getPosition().y + 32;
	sf::Text m_text("RESUME", fnt, 35);
	m_text.setFillColor(sf::Color::Black);
	m_text.setPosition(_pause_x, _pause_y + 30);
	m_text.setPosition(
		m_text.getPosition().x - m_text.getLocalBounds().width / 2, m_text.getPosition().y
	);
	pause_options.push_back(m_text);
	m_text.setString("MUSIC:   ON");
	m_text.setFillColor(sf::Color::Black);
	m_text.setPosition(_pause_x, _pause_y + 64 + 30);
	m_text.setPosition(
		m_text.getPosition().x - m_text.getLocalBounds().width / 2, m_text.getPosition().y
	);
	pause_options.push_back(m_text);
	m_text.setString("BG MUSIC:   ON");
	m_text.setFillColor(sf::Color::Black);
	m_text.setPosition(_pause_x, _pause_y + 64 * 2 + 30);
	m_text.setPosition(
		m_text.getPosition().x - m_text.getLocalBounds().width / 2, m_text.getPosition().y
	);
	pause_options.push_back(m_text);
	m_text.setString("UPGRADE TANK");
	m_text.setFillColor(sf::Color::Black);
	m_text.setPosition(_pause_x, _pause_y + 64 * 3 + 30);
	m_text.setPosition(
		m_text.getPosition().x - m_text.getLocalBounds().width / 2, m_text.getPosition().y
	);
	pause_options.push_back(m_text);
	m_text.setString("SAVE AND QUIT");
	m_text.setFillColor(sf::Color::Black);
	m_text.setPosition(_pause_x, _pause_y + 64 * 4 + 30);
	m_text.setPosition(
		m_text.getPosition().x - m_text.getLocalBounds().width / 2, m_text.getPosition().y
	);
	pause_options.push_back(m_text);


	//Effects
	m_shoot_effect.setSize({ 32,32 });
	m_shoot_texture.loadFromFile("png/shoot_effect.png");
	m_shoot_ef_rect.top = 0;
	m_shoot_ef_rect.left = 0;
	m_shoot_ef_rect.width = m_shoot_ef_rect.height = 32;
	m_shoot_effect.setTextureRect(m_shoot_ef_rect);
	m_shoot_effect.setTexture(&m_shoot_texture);

	m_bomb_effect.setSize({ 32,32 });
	m_bomb_texture.loadFromFile("png/bomb_effect.png");
	m_bomb_ef_rect.top = m_bomb_ef_rect.left = 0;
	m_bomb_ef_rect.width = m_bomb_ef_rect.height = 32;
	m_shoot_effect.setTextureRect(m_bomb_ef_rect);
	m_bomb_effect.setTexture(&m_bomb_texture);

	m_Eshoot_effect.setSize({ 32,32 });
	m_Eshoot_texture.loadFromFile("png/eshoot_effect.png");
	m_Eshoot_ef_rect.top = 0;
	m_Eshoot_ef_rect.left = 0;
	m_Eshoot_ef_rect.width = m_Eshoot_ef_rect.height = 32;
	m_Eshoot_effect.setTextureRect(m_Eshoot_ef_rect);
	m_Eshoot_effect.setTexture(&m_Eshoot_texture);
}

void game::loop(SoundManager& sm) {
	while (_window->isOpen()) {
		sf::Event event;
		while (_window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				_window->close();
				exit(0);
			}
			//-------------------------------Pause screen
			if (game_pause) {
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::W:
						if (pause_select == 0) pause_select = pause_options.size() - 1;
						else pause_select--;
						break;
					case sf::Keyboard::S:
						if (pause_select == pause_options.size() - 1) pause_select = 0;
						else pause_select++;
						break;
					case sf::Keyboard::Enter:
						sm.Button_Play();
						switch (pause_select) {
						case 0:
							game_pause = false;
							break;
						case 1:
							mute_music_all = !mute_music_all;
							sm.Switch_Sound_Effects();
							break;
						case 2:
							mute_music_bg = !mute_music_bg;
							sm.Switch_Sound_Theme();
							break;
						case 3://update tank
						{
							stats_screen st(*stats, player_score);
							st.loop(sm);
							break;
						}
						case 4:
							std::string path = "save/bin/" + std::to_string(saveSlot) + ".bin";
							saveStatus(path.c_str());
							screenCode = "save";
							_window->close();
							break;
						}

						break;
					}
				}
			}
			//-------------------------------player-event
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape) game_pause = !game_pause;
				if (!player_dead) {
					if (event.key.code == sf::Keyboard::W) {
						player_movement.y = -player_spd;
					}
					else if (event.key.code == sf::Keyboard::D) {
						player_movement.x = player_spd;
					}
					else if (event.key.code == sf::Keyboard::S) {
						player_movement.y = player_spd;
					}
					else if (event.key.code == sf::Keyboard::A) {
						player_movement.x = -player_spd;
					}
					if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Enter) {
						if (player_bullet_ready) {
							player_n_bullets_t = 0;
							player_bullet_ready = false;
						}
					}
					if (event.key.code == sf::Keyboard::E) {
						if (*player_score >= 5) {
							(*player_score) -= 5;
							sf::Vector2f _pos_box{ round((player.getPosition().x - 30) / 32), round((player.getPosition().y - 30) / 32) };
							switch (player_dir) {
							case 0:
								_pos_box.y--; break;
							case 1:
								_pos_box.x++; break;
							case 2:
								_pos_box.y++; break;
							case 3:
								_pos_box.x--; break;
							}
							bricks.push_back(m_brick);
							bricks[bricks.size() - 1].setPosition({ _pos_box.x * 32 + mapOffSet, _pos_box.y * 32 + mapOffSet });
							bricks_hp.push_back(m_brick_hp);
							bricks_rect.push_back(m_brick_rect);
						}

					}
				}
				//only V-H movement
				if (player_movement.x && player_movement.y) player_movement.y = 0;
			}
			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S) {
					player_movement.y = 0;
				}
				else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::A) {
					player_movement.x = 0;
				}
			}
		}
		_window->clear(sf::Color::Black);
		if (game_pause) goto ___game_draw;
		//---------------------------------------Update
		mute_music_all = !sm.Sound_On;
		if (!infinite_enemies) {
			if (game_win) {
				draw();
				_window->display();
				Sleep(3000); _window->close();
			}
		}
		if (game_lose) {
			draw();
			_window->display();
			sm.Game_Over_Play();
			Sleep(3000); _window->close();
		}

		//---------------------------------------Collision
		collision(sm);
		//---------------------------------------Update-bullets
		for (int i = 0; i < bullets.size(); i++) {
			if (bullet_pause_portal_t[i] < bullet_pause_portal) {
				bullet_pause_portal_t[i]++;
			}
			bullets[i].move(bullets_movement[i]);
		}
		//---------------------------------------Update-eBullets
		for (int i = 0; i < eBullets.size(); i++) {
			if (eBullet_pause_portal_t[i] < eBullet_pause_portal) {
				eBullet_pause_portal_t[i]++;
			}
			eBullets[i].move(eBullets_movement[i]);
		}
		//---------------------------------------Update-spawners
		if (n_enemy_game <= 0 && n_enemy_current <= 0) game_win = true;
		if ((n_enemy_game > 0 || infinite_enemies) && n_enemy_current < n_enemy_limit_spawn) {
			if (spawner_cooldown_t >= spawner_cooldown) {
				spawner_cooldown_t = 0;
				//spawn enemy
				n_enemy_game--; n_enemy_current++;
				if (n_enemy_game < 0) n_enemy_game = 0;
				int n_spawners = spawners.size();
				enemies.push_back(m_enemy);
				enemies[enemies.size() - 1].setPosition(spawners[rand() % n_spawners]);
				enemies_dir.push_back(rand() % 4);
				enemies_rect.push_back(enemies[enemies.size() - 1].getTextureRect());
				enemies_movement.push_back({ 0,0 });
				enemies_random_dir_t.push_back(rand() % 61 + 60);
				enemies_count_t.push_back(0);
				enemies_random_shoot_t.push_back(rand() % 31 + 120 - enemy_shoot_change);
				enemies_count_shoot_t.push_back(0);
				//effect
				eshoots_effect.push_back(m_Eshoot_effect);
				eshoots_effect_rect.push_back(m_Eshoot_ef_rect);
				t_Eshoot_t.push_back(0);
				is_ef_eshoots.push_back(0);
			}
			else spawner_cooldown_t++;
		}

		//---------------------------------------Update-enemies
		//random dir
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies_count_t[i] >= enemies_random_dir_t[i]) {
				enemies_count_t[i] = 0;
				if (rand() % 2) {
					if (rand() % 2) enemies_movement[i] = { 0, enemy_spd };
					else enemies_movement[i] = { 0, -enemy_spd };
				}
				else {
					if (rand() % 2) enemies_movement[i] = { enemy_spd,0 };
					else enemies_movement[i] = { -enemy_spd ,0 };
				}
			}
			else enemies_count_t[i]++;
		}
		//random shoot
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies_count_shoot_t[i] >= enemies_random_shoot_t[i]) {
				enemies_count_shoot_t[i] = 0;
				eBullets.push_back(m_eBullet);
				//animation
				if (is_ef_eshoots[i] == false) {
					is_ef_eshoots[i] = true;
					t_Eshoot_t[i] = 0;
				}
				switch (enemies_dir[i]) {
				case 0:
					eshoots_effect[i].setPosition({ enemies[i].getPosition().x , enemies[i].getPosition().y - 32 });
					eshoots_effect_rect[i].left = 0;
					eshoots_effect[i].setTextureRect(eshoots_effect_rect[i]);
					eshoots_effect[i].setTexture(&m_Eshoot_texture);
					break;
				case 1:
					eshoots_effect[i].setPosition({ enemies[i].getPosition().x +32, enemies[i].getPosition().y });
					eshoots_effect_rect[i].left = 32;
					eshoots_effect[i].setTextureRect(eshoots_effect_rect[i]);
					eshoots_effect[i].setTexture(&m_Eshoot_texture);
					break;
				case 2:
					eshoots_effect[i].setPosition({ enemies[i].getPosition().x , enemies[i].getPosition().y + 32 });
					eshoots_effect_rect[i].left = 64;
					eshoots_effect[i].setTextureRect(eshoots_effect_rect[i]);
					eshoots_effect[i].setTexture(&m_Eshoot_texture);
					break;
				case 3:
					eshoots_effect[i].setPosition({ enemies[i].getPosition().x -32, enemies[i].getPosition().y });
					eshoots_effect_rect[i].left = 96;
					eshoots_effect[i].setTextureRect(eshoots_effect_rect[i]);
					eshoots_effect[i].setTexture(&m_Eshoot_texture);
					break;
				}

				eBullets[eBullets.size() - 1].setPosition(enemies[i].getPosition());
				eBullet_pause_portal_t.push_back(eBullet_pause_portal);
				switch (enemies_dir[i]) {
				case 0:
					eBullets_movement.push_back({ 0,-eBullet_spd }); break;
				case 1:
					eBullets_movement.push_back({ eBullet_spd , 0 }); break;
				case 2:
					eBullets_movement.push_back({ 0,eBullet_spd }); break;
				case 3:
					eBullets_movement.push_back({ -eBullet_spd , 0 }); break;
				}
			}
			else enemies_count_shoot_t[i]++;
		}
		//movement
		for (int i = 0; i < enemies.size(); i++) {
			enemies[i].move(enemies_movement[i]);
			if (is_ef_eshoots[i]) {
				eshoots_effect[i].move(enemies_movement[i]);
			}
			if (enemies_movement[i].x) {
				int tmp = round((enemies[i].getPosition().y - mapOffSet) / 32) * 32 + mapOffSet;
				enemies[i].setPosition(enemies[i].getPosition().x, tmp);
			}
			else if (enemies_movement[i].y) {
				int tmp = round((enemies[i].getPosition().x - mapOffSet) / 32) * 32 + mapOffSet;
				enemies[i].setPosition(tmp, enemies[i].getPosition().y);
			}
			if (enemies_movement[i].x > 0 && enemies_movement[i].y == 0) enemies_dir[i] = 1;
			else if (enemies_movement[i].x < 0 && enemies_movement[i].y == 0) enemies_dir[i] = 3;
			else if (enemies_movement[i].y > 0 && enemies_movement[i].x == 0) enemies_dir[i] = 2;
			else if (enemies_movement[i].y < 0 && enemies_movement[i].x == 0) enemies_dir[i] = 0;
		}
		//---------------------------------------Update-player
		if (player_dead) {
			if (t_ef_spawn_t >= t_ef_spawn) {
				t_ef_spawn_t = 0;
				is_ef_spawn = !is_ef_spawn;
			}
			else t_ef_spawn_t++;
		}
		else is_ef_spawn = false;
		if (immortal) {
			shield.setPosition(player.getPosition());
			if (immortal_t >= immortal_cooldown) {
				immortal_t = immortal = 0;
			}
			else immortal_t++;
		}
		//update depend on stats
		//[0]
		(*player_spd_basic) = (*stats[0]) * 2.f;
		//[1]
		(*player_shoot_cooldown_t) = 115 - (15 * *stats[1]);
		//[2]
		(*player_n_bullets) = *stats[2];
		//[3]
		(*bullet_spd) = 3 + *stats[3];

		if (player_is_debuff) {
			player_spd = water_debuff;
			if (player_debuff_time_t >= player_debuff_time) {
				player_debuff_time_t = 0;
				player_is_debuff = false;
			}
			else player_debuff_time_t++;
		}
		else player_spd = *player_spd_basic;

		if (!player_dead) {
			if (player_n_bullets_t < *player_n_bullets) {
				if (player_n_bullet_cooldown_t >= player_n_bullet_cooldown) {
					player_n_bullet_cooldown_t = 0;
					sm.Fire_Play();
					if (is_ef_shoot == false) {
						is_ef_shoot = true;
						t_ef_shoot_t = 0;
					}
					//m_shoot_effect.setPosition()

					switch (player_dir) {
					case 0:
						m_shoot_effect.setPosition({ player.getPosition().x ,  player.getPosition().y - 32 });
						m_shoot_ef_rect.left = 0;
						m_shoot_effect.setTextureRect(m_shoot_ef_rect);
						m_shoot_effect.setTexture(&m_shoot_texture);
						break;
					case 1:
						m_shoot_effect.setPosition({ player.getPosition().x + 32 ,  player.getPosition().y });
						m_shoot_ef_rect.left = 32;
						m_shoot_effect.setTextureRect(m_shoot_ef_rect);
						m_shoot_effect.setTexture(&m_shoot_texture);
						break;
					case 2:
						m_shoot_effect.setPosition({ player.getPosition().x ,  player.getPosition().y + 32 });
						m_shoot_ef_rect.left = 64;
						m_shoot_effect.setTextureRect(m_shoot_ef_rect);
						m_shoot_effect.setTexture(&m_shoot_texture);
						break;
					case 3:
						m_shoot_effect.setPosition({ player.getPosition().x - 32,  player.getPosition().y });
						m_shoot_ef_rect.left = 96;
						m_shoot_effect.setTextureRect(m_shoot_ef_rect);
						m_shoot_effect.setTexture(&m_shoot_texture);
						break;
					}


					bullets.push_back(m_bullet);
					bullet_pause_portal_t.push_back(bullet_pause_portal);
					bullets[bullets.size() - 1].setPosition(player.getPosition());
					switch (player_dir) {
					case 0:
						bullets_movement.push_back({ 0,-*bullet_spd }); break;
					case 1:
						bullets_movement.push_back({ *bullet_spd , 0 }); break;
					case 2:
						bullets_movement.push_back({ 0,*bullet_spd }); break;
					case 3:
						bullets_movement.push_back({ -*bullet_spd , 0 }); break;
					}
					player_n_bullets_t++;
				}
			}
		}
		else player_n_bullets_t = *player_n_bullets;

		if (player_n_bullet_cooldown_t < player_n_bullet_cooldown) player_n_bullet_cooldown_t++;
		if (!player_dead) {
			player.move(player_movement);
			if (is_ef_shoot) m_shoot_effect.move(player_movement);
			if (player_movement.x) {
				int tmp = round((player.getPosition().y - mapOffSet) / 32) * 32 + mapOffSet;
				player.setPosition(player.getPosition().x, tmp);
			}
			else if (player_movement.y) {
				int tmp = round((player.getPosition().x - mapOffSet) / 32) * 32 + mapOffSet;
				player.setPosition(tmp, player.getPosition().y);
			}
			if (player_movement.x > 0 && player_movement.y == 0) player_dir = 1;
			else if (player_movement.x < 0 && player_movement.y == 0) player_dir = 3;
			else if (player_movement.y > 0 && player_movement.x == 0) player_dir = 2;
			else if (player_movement.y < 0 && player_movement.x == 0) player_dir = 0;
		}
		else {
			if (*lifes) {
				// respawning
				if (player_respawn_count_t >= player_respawn_t) {
					sm.Player_Respawn_Play();
					immortal = true;
					player_respawn_count_t = 0;
					player_dead = false;
					player.setPosition(player_spawn_point);
				}
				else player_respawn_count_t++;
			}
			else {
				game_lose = true;
			}
		}
		if (player_bullet_ready == false) {
			if (player_shoot_cooldown_count_t >= *player_shoot_cooldown_t) {
				player_shoot_cooldown_count_t = 0;
				player_bullet_ready = true;
			}
			else player_shoot_cooldown_count_t++;
		}
		//--------------------------------------Update-home
		if (home_exist) {
			if (home_dead) game_lose = true;
		}

		//--------------------------------------Update-portals
		for (int i = 0; i < portals.size(); i++) {
			if (portals_ready[i] == false) {
				if (portals_cooldown_t[i] >= portal_cooldown) {
					portals_cooldown_t[i] = 0;
					portals_ready[i] = true;
				}
				else portals_cooldown_t[i]++;
			}
		}
		//--------------------------------------INFO-BOARD
	___game_draw:
		draw();

		_window->display();
	}
}

void game::draw() {
	//---------------------------------------------map shape
	_window->draw(mapShape);
	_window->draw(info_board);
	//texts
	info_texts[7].setString("Score:        " + std::to_string(*player_score * 10));
	info_texts[8].setString("Lifes:          " + std::to_string(*lifes));
	for (int i = 0; i < info_texts.size(); i++) {
		_window->draw(info_texts[i]);
	}
	//mini enemies
	for (int i = 0; i < info_n_less; i++) {
		_window->draw(info_mini_enemies[i]);
	}
	for (int i = info_n_less; i < info_mini_enemies.size(); i++) {
		info_mini_rect[i].left = 8;
		info_mini_enemies[i].setTextureRect(info_mini_rect[i]);
		info_mini_enemies[i].setTexture(&mini_enemy_texture);
		_window->draw(info_mini_enemies[i]);
	}
	//stats
	for (int i = 0; i < bars.size(); i++) {
		for (int j = 0; j < *stats[i]; j++) {
			bars[i][j].setFillColor(sf::Color::Green);
			bars[i][j].setOutlineThickness(2);
			bars[i][j].setOutlineColor(sf::Color::Black);
			_window->draw(bars[i][j]);
		}
		for (int j = *stats[i]; j < bars[i].size(); j++) {
			_window->draw(bars[i][j]);
		}
	}
	//infinite
	if (infinite_enemies) {
		_window->draw(infinite_text);
	}
	//------------------------------------------Pause

	if (game_pause) {
		_window->draw(pause_box);
		if (mute_music_all) pause_options[1].setString("MUSIC:   OFF");
		else pause_options[1].setString("MUSIC:   ON");

		if (mute_music_bg) pause_options[2].setString("BG MUSIC:   OFF");
		else pause_options[2].setString("BG MUSIC:   ON");
		for (int i = 0; i < pause_options.size(); i++) {
			if (i == pause_select) {
				pause_options[i].setFillColor(sf::Color::Green);
				pause_options[i].setOutlineThickness(3);
				pause_options[i].setOutlineColor(sf::Color::Black);
			}
			else {
				pause_options[i].setFillColor(sf::Color::Black);
				pause_options[i].setOutlineThickness(0);
			}
			_window->draw(pause_options[i]);
		}

	}

	//------------------------------------------portal
	for (int i = 0; i < portals.size(); i++) {
		if (portals_ready[i] == true) {
			portals_rect[i].left = 0;
		}
		else portals_rect[i].left = 32;
		portals[i].setTextureRect(portals_rect[i]); portals[i].setTexture(&portal_texture);
		_window->draw(portals[i]);
	}
	//------------------------------------------water
	for (int i = 0; i < waters.size(); i++) {
		_window->draw(waters[i]);
	}
	//---------------------------------------------player
	if (!player_dead) {
		switch (player_dir) {
		case 0:
			player_rect.left = 0; break;
		case 1:
			player_rect.left = 32; break;
		case 2:
			player_rect.left = 64; break;
		case 3:
			player_rect.left = 96; break;
		}
		player.setTextureRect(player_rect); player.setTexture(&tank_texture);
		_window->draw(player);
	}
	//------------------------------------------shield
	if (immortal && player_dead == false) _window->draw(shield);

	if (is_ef_spawn) _window->draw(spawn_box);

	//------------------------------------------enemies
	for (int i = 0; i < enemies.size(); i++) {
		switch (enemies_dir[i]) {
		case 0:
			enemies_rect[i].left = 128; break;
		case 1:
			enemies_rect[i].left = 160; break;
		case 2:
			enemies_rect[i].left = 192; break;
		case 3:
			enemies_rect[i].left = 224; break;
		}
		enemies[i].setTextureRect(enemies_rect[i]); enemies[i].setTexture(&tank_texture);
		_window->draw(enemies[i]);
	}

	//------------------------------------------bedrocks
	for (int i = 0; i < bedrocks.size(); i++) {
		_window->draw(bedrocks[i]);
	}
	//------------------------------------------bullets
	for (int i = 0; i < bullets.size(); i++) {
		_window->draw(bullets[i]);
	}
	//------------------------------------------bricks
	for (int i = 0; i < bricks.size(); i++) {
		if (bricks_hp[i] <= m_brick_hp / 2) {
			bricks_rect[i].left = 32;
		}
		bricks[i].setTextureRect(bricks_rect[i]); bricks[i].setTexture(&brick_texture);
	}
	for (int i = 0; i < bricks.size(); i++) {
		_window->draw(bricks[i]);
	}
	//------------------------------------------eBullets
	for (int i = 0; i < eBullets.size(); i++) {
		_window->draw(eBullets[i]);
	}
	//------------------------------------------grasses
	for (int i = 0; i < grasses.size(); i++) {
		_window->draw(grasses[i]);
	}

	//------------------------------------------home
	if (!home_dead && home_exist) _window->draw(home);

	//------------------------------------------pause
	if (game_pause) {
		_window->draw(pause_text);
	}
	//------------------------------------------effects
	if (is_ef_shoot) {
		if (t_ef_shoot_t <= t_ef_shoot) {
			_window->draw(m_shoot_effect);
			t_ef_shoot_t++;
		}
		else {
			is_ef_shoot = false;
			t_ef_shoot_t = 0;
		}
	}

	for (int i = 0; i < bombs_ef.size(); i++) {
		if (t_bombs_t[i] <= 75) {
			t_bombs_t[i]++;
			bombs_ef_rect[i].left = (t_bombs_t[i] * 5 / 75) * 32;
			bombs_ef[i].setTextureRect(bombs_ef_rect[i]);
			bombs_ef[i].setTexture(&m_bomb_texture);
			_window->draw(bombs_ef[i]);
		}
		else {
			bombs_ef.erase(bombs_ef.begin() + i);
			bombs_ef_rect.erase(bombs_ef_rect.begin() + i);
			t_bombs_t.erase(t_bombs_t.begin() + i);
		}

	}

	for (int i = 0; i < enemies.size(); i++) {
		if (is_ef_eshoots[i]) {
			if (t_Eshoot_t[i] <= t_ef_shoot) {
				_window->draw(eshoots_effect[i]);
				t_Eshoot_t[i]++;
			}
			else {
				is_ef_eshoots[i] = false;
				t_Eshoot_t[i] = 0;
			}
		}
	}

	//---------------------------------------------win lose
	if (game_win && !infinite_enemies) {
		_window->draw(game_wl_text);
	}
	if (game_lose) {
		game_wl_text.setString("  GAME OVER  ");
		game_wl_text.setPosition(game_wl_text.getPosition().x + 33, game_wl_text.getPosition().y);
		_window->draw(game_wl_text);
	}
}

void game::collision(SoundManager& sm) {
	//-------------------- player
	for (const auto& br : bedrocks) {
		if (checkCollision(player, br)) {
			float distanceX = player.getPosition().x - br.getPosition().x;
			float distanceY = player.getPosition().y - br.getPosition().y;
			if (abs(distanceX) > abs(distanceY)) {
				player_movement.x = 0;
				//fit block
				if (distanceX > 0) {
					player.setPosition(br.getPosition().x + 32, player.getPosition().y);
				}
				else {
					player.setPosition(br.getPosition().x - 32, player.getPosition().y);
				}
			}
			else {
				player_movement.y = 0;
				//fit block
				if (distanceY > 0) {
					player.setPosition(player.getPosition().x, br.getPosition().y + 32);
				}
				else {
					player.setPosition(player.getPosition().x, br.getPosition().y - 32);
				}
			}
		}
	}
	for (const auto& br : enemies) {
		if (checkCollision(player, br)) {
			float distanceX = player.getPosition().x - br.getPosition().x;
			float distanceY = player.getPosition().y - br.getPosition().y;
			if (abs(distanceX) > abs(distanceY)) {
				player_movement.x = 0;
				//fit block
				if (distanceX > 0) {
					player.setPosition(br.getPosition().x + 32, player.getPosition().y);
				}
				else {
					player.setPosition(br.getPosition().x - 32, player.getPosition().y);
				}
			}
			else {
				player_movement.y = 0;
				//fit block
				if (distanceY > 0) {
					player.setPosition(player.getPosition().x, br.getPosition().y + 32);
				}
				else {
					player.setPosition(player.getPosition().x, br.getPosition().y - 32);
				}
			}
		}
	}
	for (const auto& br : bricks) {
		if (checkCollision(player, br)) {
			float distanceX = player.getPosition().x - br.getPosition().x;
			float distanceY = player.getPosition().y - br.getPosition().y;

			if (abs(distanceX) > abs(distanceY)) {
				player_movement.x = 0;
				//fit block
				if (distanceX > 0) {
					player.setPosition(br.getPosition().x + 32, player.getPosition().y);
				}
				else {
					player.setPosition(br.getPosition().x - 32, player.getPosition().y);
				}
			}
			else {
				player_movement.y = 0;
				//fit block
				if (distanceY > 0) {
					player.setPosition(player.getPosition().x, br.getPosition().y + 32);
				}
				else {
					player.setPosition(player.getPosition().x, br.getPosition().y - 32);
				}
			}
		}
	}
	for (int i = 0; i < portals.size(); i++) {
		if (checkCollision(player, portals[i])) {
			if (portals_ready[i]) {
				sm.Portal_Sound_Play();
				player.setPosition(
					portals[portals_des[i]].getPosition()
				);
				portals_ready[i] = false;
				portals_ready[portals_des[i]] = false;
			}
		}

	}
	for (const auto& wt : waters) {
		if (checkCollision(wt, player)) {
			player_is_debuff = true;
		}
	}
	//-------------------- bullet
	for (int i = 0; i < bullets.size(); i++) {
		for (const auto& br : bedrocks) {
			if (checkCollision(bullets[i], br)) {
				sm.Bred_Bricks_Hit_Play();

				bombs_ef.push_back(m_bomb_effect);
				bombs_ef_rect.push_back(m_bomb_ef_rect);
				t_bombs_t.push_back(0);
				bombs_ef[bombs_ef.size() - 1].setPosition(bullets[i].getPosition());
				bombs_ef[bombs_ef.size() - 1].setScale({ .5f,.5f });
				bombs_ef[bombs_ef.size() - 1].setOrigin(-8, -9);

				bullets.erase(bullets.begin() + i);
				bullets_movement.erase(bullets_movement.begin() + i);
				bullet_pause_portal_t.erase(bullet_pause_portal_t.begin() + i);
				i = 0;
				break;
			}
		}
		if (i >= bullets.size()) break;

		for (int j = 0; j < portals.size(); j++) {
			if (checkCollision(bullets[i], portals[j])) {
				if (portals_ready[j] && bullet_pause_portal_t[i] >= bullet_pause_portal) {
					sm.Portal_Sound_Play();
					bullet_pause_portal_t[i] = 0;
					bullets[i].setPosition(
						portals[portals_des[j]].getPosition()
					);
				}
			}
		}

		for (int j = 0; j < enemies.size(); j++) {
			if (checkCollision(bullets[i], enemies[j])) {
				bullets.erase(bullets.begin() + i);
				bullets_movement.erase(bullets_movement.begin() + i);
				bullet_pause_portal_t.erase(bullet_pause_portal_t.begin() + i);
				i = 0;
				//scoring
				*player_score += 1;
				//animation
				bombs_ef.push_back(m_bomb_effect);
				bombs_ef_rect.push_back(m_bomb_ef_rect);
				t_bombs_t.push_back(0);
				bombs_ef[bombs_ef.size() - 1].setPosition(enemies[j].getPosition());

				//destroy enemy
				sm.Enemy_Tank_Explosion_Play();
				n_enemy_current--;
				info_n_less--;
				enemies.erase(enemies.begin() + j);
				enemies_dir.erase(enemies_dir.begin() + j);
				enemies_rect.erase(enemies_rect.begin() + j);
				enemies_movement.erase(enemies_movement.begin() + j);
				enemies_random_dir_t.erase(enemies_random_dir_t.begin() + j);
				enemies_count_t.erase(enemies_count_t.begin() + j);

				eshoots_effect.erase(eshoots_effect.begin() + j);
				eshoots_effect_rect.erase(eshoots_effect_rect.begin() + j);
				t_Eshoot_t.erase(t_Eshoot_t.begin() + j);
				is_ef_eshoots.erase(is_ef_eshoots.begin() + j);


				break;
			}
		}

		if (i >= bullets.size()) break;

		for (int j = 0; j < bricks.size(); j++) {
			if (checkCollision(bullets[i], bricks[j])) {
				bombs_ef.push_back(m_bomb_effect);
				bombs_ef_rect.push_back(m_bomb_ef_rect);
				t_bombs_t.push_back(0);
				bombs_ef[bombs_ef.size() - 1].setPosition(bullets[i].getPosition());
				bombs_ef[bombs_ef.size() - 1].setScale({ .5f,.5f });
				bombs_ef[bombs_ef.size() - 1].setOrigin(-8, -9);

				bullets.erase(bullets.begin() + i);
				bullets_movement.erase(bullets_movement.begin() + i);
				bullet_pause_portal_t.erase(bullet_pause_portal_t.begin() + i);
				i = -1;
				bricks_hp[j] -= 1;
				if (bricks_hp[j] <= 0) {
					sm.Wall_Destroyed_Play();

					bombs_ef.push_back(m_bomb_effect);
					bombs_ef_rect.push_back(m_bomb_ef_rect);
					t_bombs_t.push_back(0);
					bombs_ef[bombs_ef.size() - 1].setPosition(bricks[j].getPosition());

					bricks.erase(bricks.begin() + j);
					bricks_hp.erase(bricks_hp.begin() + j);
					bricks_rect.erase(bricks_rect.begin() + j);
				}
				else {
					sm.Bred_Bricks_Hit_Play();
				}
				break;
			}
		}

	}
	//-------------------- enemy
	for (int i = 0; i < enemies.size(); i++) {
		for (const auto& br : bedrocks) {
			if (checkCollision(enemies[i], br)) {
				float distanceX = enemies[i].getPosition().x - br.getPosition().x;
				float distanceY = enemies[i].getPosition().y - br.getPosition().y;
				if (abs(distanceX) > abs(distanceY)) {
					enemies_movement[i].x = 0;
					//fit block
					if (distanceX > 0) {
						enemies[i].setPosition(br.getPosition().x + 32, enemies[i].getPosition().y);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].x = enemy_spd; break;
						case 1:
							enemies_movement[i].y = enemy_spd; break;
						case 2:
							enemies_movement[i].y = -enemy_spd; break;
						}
					}
					else {
						enemies[i].setPosition(br.getPosition().x - 32, enemies[i].getPosition().y);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].x = -enemy_spd; break;
						case 1:
							enemies_movement[i].y = enemy_spd; break;
						case 2:
							enemies_movement[i].y = -enemy_spd; break;
						}
					}
				}
				else {
					enemies_movement[i].y = 0;
					//fit block
					if (distanceY > 0) {
						enemies[i].setPosition(enemies[i].getPosition().x, br.getPosition().y + 32);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].y = enemy_spd; break;
						case 1:
							enemies_movement[i].x = enemy_spd; break;
						case 2:
							enemies_movement[i].x = -enemy_spd; break;
						}
					}
					else {
						enemies[i].setPosition(enemies[i].getPosition().x, br.getPosition().y - 32);
						switch (rand() % 3) {
						case 0:
							enemies_movement[i].y = -enemy_spd; break;
						case 1:
							enemies_movement[i].x = enemy_spd; break;
						case 2:
							enemies_movement[i].x = -enemy_spd; break;
						}
					}
				}

			}
		}
		for (const auto& br : bricks) {
			if (checkCollision(enemies[i], br)) {
				float distanceX = enemies[i].getPosition().x - br.getPosition().x;
				float distanceY = enemies[i].getPosition().y - br.getPosition().y;

				if (abs(distanceX) > abs(distanceY)) {
					enemies_movement[i].x = 0;
					//fit block
					if (distanceX > 0) {
						enemies[i].setPosition(br.getPosition().x + 32, enemies[i].getPosition().y);
					}
					else {
						enemies[i].setPosition(br.getPosition().x - 32, enemies[i].getPosition().y);
					}
				}
				else {
					enemies_movement[i].y = 0;
					//fit block
					if (distanceY > 0) {
						enemies[i].setPosition(enemies[i].getPosition().x, br.getPosition().y + 32);
					}
					else {
						enemies[i].setPosition(enemies[i].getPosition().x, br.getPosition().y - 32);
					}
				}
			}
		}
	}

	//-------------------- eBullet
	for (int i = 0; i < eBullets.size(); i++) {

		for (int j = 0; j < portals.size(); j++) {
			if (checkCollision(eBullets[i], portals[j])) {
				if (portals_ready[j] && eBullet_pause_portal_t[i] >= eBullet_pause_portal) {
					eBullet_pause_portal_t[i] = 0;
					sm.Portal_Sound_Play();
					eBullets[i].setPosition(
						portals[portals_des[j]].getPosition()
					);
				}
			}
		}

		if (checkCollision(player, eBullets[i]) ) {
			sm.Player_Being_Hit_Play();
			eBullets.erase(eBullets.begin() + i);
			eBullets_movement.erase(eBullets_movement.begin() + i);

			bombs_ef.push_back(m_bomb_effect);
			bombs_ef_rect.push_back(m_bomb_ef_rect);
			t_bombs_t.push_back(0);
			bombs_ef[bombs_ef.size() - 1].setPosition(player.getPosition());

			if (!immortal) {
				i = -1;
				//player dead
				(*lifes)--;
				player_dead = true;
				player.setPosition({ 0,0 });
			}
			break;
		}

		if (checkCollision(home, eBullets[i])) {

			bombs_ef.push_back(m_bomb_effect);
			bombs_ef_rect.push_back(m_bomb_ef_rect);
			t_bombs_t.push_back(0);
			bombs_ef[bombs_ef.size() - 1].setPosition(home.getPosition());
			bombs_ef[bombs_ef.size() - 1].setScale({ 2.f,2.f });

			eBullets.erase(eBullets.begin() + i);
			eBullets_movement.erase(eBullets_movement.begin() + i);
			i = -1;
			//player dead
			home_dead = true;
			home.setPosition({ 0,0 });
			break;
		}

		for (const auto& br : bedrocks) {
			if (checkCollision(eBullets[i], br)) {

				bombs_ef.push_back(m_bomb_effect);
				bombs_ef_rect.push_back(m_bomb_ef_rect);
				t_bombs_t.push_back(0);
				bombs_ef[bombs_ef.size() - 1].setPosition(eBullets[i].getPosition());
				bombs_ef[bombs_ef.size() - 1].setScale({ .5f,.5f });
				bombs_ef[bombs_ef.size() - 1].setOrigin(-8, -9);

				eBullets.erase(eBullets.begin() + i);
				eBullets_movement.erase(eBullets_movement.begin() + i);
				i = 0;
				break;
			}
		}
		if (i >= eBullets.size()) break;
		for (int j = 0; j < bricks.size(); j++) {
			if (checkCollision(eBullets[i], bricks[j])) {
				bombs_ef.push_back(m_bomb_effect);
				bombs_ef_rect.push_back(m_bomb_ef_rect);
				t_bombs_t.push_back(0);
				bombs_ef[bombs_ef.size() - 1].setPosition(eBullets[i].getPosition());
				bombs_ef[bombs_ef.size() - 1].setScale({ .5f,.5f });
				bombs_ef[bombs_ef.size() - 1].setOrigin(-8, -9);

				eBullets.erase(eBullets.begin() + i);
				eBullets_movement.erase(eBullets_movement.begin() + i);
				i = -1;
				bricks_hp[j]--;
				if (bricks_hp[j] <= 0) {
					sm.Wall_Destroyed_Play();

					bombs_ef.push_back(m_bomb_effect);
					bombs_ef_rect.push_back(m_bomb_ef_rect);
					t_bombs_t.push_back(0);
					bombs_ef[bombs_ef.size() - 1].setPosition(bricks[j].getPosition());

					bricks.erase(bricks.begin() + j);
					bricks_hp.erase(bricks_hp.begin() + j);
					bricks_rect.erase(bricks_rect.begin() + j);
				}
				break;
			}
		}
	}

}

void game::map_read_from_file(const std::string& nameF) {
	mapData.resize(0);
	std::fstream fileOpen(nameF, std::ios::in);
	int tmp = 0;
	std::vector<int> tmp_vt;
	int i = 0;
	const int max = 25;
	while (fileOpen.good()) {
		fileOpen >> tmp;
		tmp_vt.push_back(tmp);
		i++;
		if (i == max) {
			i = 0;
			mapData.push_back(tmp_vt);
			tmp_vt.clear();
		}
	}
	fileOpen.close();
}

sf::Vector2i getIdPortal(int input) {
	input %= 10000;
	int des = input % 100;
	int src = input / 100;
	return { src, des };
}

void game::map_set_game() {
	//------------------------------------------Data-map
	int lv = levelCode;
	std::string src = "level/level_" + std::to_string(lv) + ".txt";
	map_read_from_file(src.c_str());
	for (float i = 0; i < 20; i++) {
		for (float j = 0; j < 25; j++) {
			switch (mapData[i][j]) {
			case 0:
				break;
			case 1://bedrock
				bedrocks.push_back(m_bedrock);
				bedrocks[bedrocks.size() - 1].setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				break;
			case 2://grass
				grasses.push_back(m_grass);
				grasses[grasses.size() - 1].setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				break;
			case 3://brick
			{
				bricks.push_back(m_brick);
				bricks[bricks.size() - 1].setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				bricks_hp.push_back(m_brick_hp);
				bricks_rect.push_back(m_brick_rect);
				break;
			}
			case 4://home
			{
				if (home_exist == false)
					if (i + j >= 41) {
						home_exist == false;
						home_dead = true;
					}
					else {
						home_exist == true;
						home.setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
						home_exist = true;
						sf::Vector2f pos[4]{
							{ j * 32 + mapOffSet + 32, i * 32 + mapOffSet },
							{ j * 32 + mapOffSet, i * 32 + mapOffSet + 32},
							{ j * 32 + mapOffSet, i * 32 + mapOffSet },
							{ j * 32 + mapOffSet + 32, i * 32 + mapOffSet + 32}
						};
						for (int _i = 0; _i < 4; _i++) {
							bedrocks.push_back(m_bedrock);
							bedrocks[bedrocks.size() - 1].setPosition(pos[_i]);
						}

					}
				break;
			}
			case 5: //water
			{
				waters.push_back(m_water);
				waters[waters.size() - 1].setPosition({ j * 32 + mapOffSet , i * 32 + mapOffSet });
				break;
			}
			case 6://spawner
			{
				spawners.push_back({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				//show as grass
				grasses.push_back(m_grass);
				grasses[grasses.size() - 1].setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				break;
			}
			case 7:
				player.setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
				player_spawn_point = { j * 32 + mapOffSet, i * 32 + mapOffSet };
				spawn_box.setPosition(player_spawn_point);
				break;
			default://portal
			{
				if (mapData[i][j] >= 10000 && mapData[i][j] <= 19999)
				{
					sf::Vector2i id = getIdPortal(mapData[i][j]);
					std::cout << id.x << '-' << id.y << '\n';
					portals.push_back(m_portal);
					portals[portals.size() - 1].setPosition({ j * 32 + mapOffSet, i * 32 + mapOffSet });
					portals_rect.push_back(m_portal_rect);
					portals_des.push_back(id.y);
					portals_ready.push_back(true);
					portals_cooldown_t.push_back(0);
					break;
				}
			}
			}


		}
	}
}

bool checkCollision(const sf::RectangleShape& a, const sf::RectangleShape& b) {
	return a.getGlobalBounds().intersects(b.getGlobalBounds());

}

stats_screen::stats_screen() {
	stats = NULL;
	window = NULL;
	currency = NULL;
	select = 0;
}

stats_screen::stats_screen(int* pointer, int* score) {
	stats = pointer;
	currency = score;
	window = new sf::RenderWindow(sf::VideoMode({ 600,300 }), "upgrade", sf::Style::Titlebar);
	bg_texture.loadFromFile("png/stats_bg.png");
	fnt.loadFromFile("font/MobileFont.ttf");
	bg.setSize({ 600,300 });
	bg.setTexture(&bg_texture);
	select = 0;

	float bar_width = 440;
	float bar_coordX = 30;
	float bar_coordY[4]{ 50,100, 150, 200 };
	for (int i = 0; i < 4; i++) {
		std::vector<sf::RectangleShape> bar;
		float dot_width = (bar_width * 4) / (5 * max_stats[i]);
		sf::RectangleShape dot;
		dot.setSize({ dot_width, 5 });
		dot.setFillColor(sf::Color::Black);
		dot.setOutlineThickness(1);
		dot.setOutlineColor(sf::Color::White);
		for (int j = 0; j < max_stats[i]; j++) {
			bar.push_back(dot);
		}
		for (int j = 0; j < max_stats[i]; j++) {
			bar[j].setPosition({ bar_coordX + j * (dot_width + dot_width / 4), bar_coordY[i] });
		}
		bars.push_back(bar);
	}

	std::string strs[4]{ "Speed", "Reload", "Bullets", "Canon" };
	sf::Text m_text("", fnt, 20);
	m_text.setFillColor(sf::Color::White);
	for (int i = 0; i < 4; i++) {
		m_text.setPosition(30, bars[i][0].getPosition().y - 30);
		m_text.setString(strs[i]);
		texts.push_back(m_text);
	}

	for (int i = 0; i < 4; i++) {
		m_text.setPosition(520, bars[i][0].getPosition().y - 7);
		m_text.setString("+");
		m_text.setCharacterSize(40);
		m_text.setOutlineThickness(2);
		m_text.setOrigin({ m_text.getLocalBounds().width / 2, m_text.getLocalBounds().height / 2 });
		buttons.push_back(m_text);
	}
	m_text.setOutlineThickness(0);
	m_text.setCharacterSize(30);
	m_text.setString("Back");
	m_text.setOrigin({ m_text.getLocalBounds().width / 2, 0 });
	m_text.setPosition(520, bars[3][0].getPosition().y + 30);
	buttons.push_back(m_text);
	m_text.setCharacterSize(20);
	m_text.setOrigin(0, 0);
	hint = m_text;
	hint.setString("80 scores = 1 upgrade\n\nYour score:   " + std::to_string(*currency * 10));
	hint.setPosition(30, bars[3][0].getPosition().y + 20);
}

void stats_screen::loop(SoundManager& sm) {
	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::W:
					if (select == 0) select = buttons.size() - 1;
					else select--;
					break;
				case sf::Keyboard::S:
					if (select == buttons.size() - 1) select = 0;
					else select++;
					break;
				case sf::Keyboard::Enter:
					if (select == buttons.size() - 1) {
						window->close();
						sm.Button_Play();
						break;
					}
					if (isMaxed[select]) break;
					else {
						if (*currency >= 8) {
							*currency -= 8;
							sm.Button_Play();
							stats[select]++;
							hint.setString("80 scores = 1 upgrade\n\nYour score:   " + std::to_string(*currency * 10));
						}
					}
					break;
				}
			}
		}

		for (int i = 0; i < 4; i++) {
			if (stats[i] == max_stats[i]) {
				isMaxed[i] = true;
				buttons[i].setString("Maxed");
				buttons[i].setCharacterSize(25);
				buttons[i].setOrigin({ buttons[i].getLocalBounds().width / 2, buttons[i].getLocalBounds().height / 2 });
			}
		}

		window->clear();

		draw();

		window->display();
	}
}

void stats_screen::draw() {
	window->draw(bg);

	for (int i = 0; i < bars.size(); i++) {
		for (int j = 0; j < stats[i]; j++) {
			bars[i][j].setFillColor(sf::Color::Green);
			bars[i][j].setOutlineThickness(2);
			bars[i][j].setOutlineColor(sf::Color::Black);
			window->draw(bars[i][j]);
		}
		for (int j = stats[i]; j < bars[i].size(); j++) {
			window->draw(bars[i][j]);
		}
	}

	window->draw(hint);

	for (int i = 0; i < texts.size(); i++) {
		window->draw(texts[i]);
	}
	for (int i = 0; i < buttons.size(); i++) {
		if (i == select) {
			buttons[i].setFillColor(sf::Color::Green);
		}
		else buttons[i].setFillColor(sf::Color::White);
		window->draw(buttons[i]);
	}
}

void game::saveStatus(const std::string& fileName) {
	std::fstream fileOpen(fileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	//player
	fileOpen.write(reinterpret_cast<const char*>(&player_dead), sizeof(bool));
	sf::Vector2f _playerPos = player.getPosition();
	fileOpen.write(reinterpret_cast<const char*>(&_playerPos.x), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&_playerPos.y), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&player_spawn_point.x), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&player_spawn_point.y), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(lifes), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&player_dir), sizeof(int));
	//enemies
	fileOpen.write(reinterpret_cast<const char*>(&n_enemy_game), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&n_enemy_current), sizeof(int));
	for (int i = 0; i < n_enemy_current; i++) {
		sf::Vector2f _pos = enemies[i].getPosition();
		fileOpen.write(reinterpret_cast<const char*>(&_pos.x), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&_pos.y), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&enemies_dir[i]), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&enemies_random_dir_t[i]), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&enemies_random_shoot_t[i]), sizeof(int));
	}
	//spawners
	int nSpawners = spawners.size();
	fileOpen.write(reinterpret_cast<const char*>(&nSpawners), sizeof(int));
	for (int i = 0; i < nSpawners; i++) {
		fileOpen.write(reinterpret_cast<const char*>(&spawners[i].x), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&spawners[i].y), sizeof(int));
	}
	//Bricks
	int nBricks = bricks.size();
	fileOpen.write(reinterpret_cast<const char*>(&nBricks), sizeof(int));
	for (int i = 0; i < nBricks; i++) {
		sf::Vector2f _pos = bricks[i].getPosition();
		fileOpen.write(reinterpret_cast<const char*>(&_pos.x), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&_pos.y), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&bricks_hp[i]), sizeof(int));
	}
	//Grasses
	int nGrasses = grasses.size();
	fileOpen.write(reinterpret_cast<const char*>(&nGrasses), sizeof(int));
	for (int i = 0; i < nGrasses; i++) {
		sf::Vector2f _pos = grasses[i].getPosition();
		fileOpen.write(reinterpret_cast<const char*>(&_pos.x), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&_pos.y), sizeof(int));
	}
	//bedrocks
	int nBedrocks = bedrocks.size();
	fileOpen.write(reinterpret_cast<const char*>(&nBedrocks), sizeof(int));
	for (int i = 0; i < nBedrocks; i++) {
		sf::Vector2f _pos = bedrocks[i].getPosition();
		fileOpen.write(reinterpret_cast<const char*>(&_pos.x), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&_pos.y), sizeof(int));
	}
	//portal
	int nPortals = portals.size();
	fileOpen.write(reinterpret_cast<const char*>(&nPortals), sizeof(int));
	for (int i = 0; i < nPortals; i++) {
		sf::Vector2f _pos = portals[i].getPosition();
		fileOpen.write(reinterpret_cast<const char*>(&_pos.x), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&_pos.y), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&portals_ready[i]), sizeof(bool));
		fileOpen.write(reinterpret_cast<const char*>(&portals_des[i]), sizeof(int));
	}
	//home
	fileOpen.write(reinterpret_cast<const char*>(&home_exist), sizeof(bool));
	if (home_exist) {
		fileOpen.write(reinterpret_cast<const char*>(&home_dead), sizeof(bool));
		sf::Vector2f _homePos = home.getPosition();
		fileOpen.write(reinterpret_cast<const char*>(&_homePos.x), sizeof(int));
		fileOpen.write(reinterpret_cast<const char*>(&_homePos.y), sizeof(int));
	}
	//info
	fileOpen.write(reinterpret_cast<const char*>(&info_n_enemies), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&info_n_less), sizeof(int));
	fileOpen.close();
}

void game::loadStatus(const std::string& fileName) {
	std::fstream fileOpen(fileName.c_str(), std::ios::in | std::ios::binary);

	// Check if the file is opened successfully
	if (!fileOpen.is_open()) {
		// Handle error: unable to open the file
		return;
	}

	// Player
	fileOpen.read(reinterpret_cast<char*>(&player_dead), sizeof(bool));
	sf::Vector2f _playerPos;
	fileOpen.read(reinterpret_cast<char*>(&_playerPos.x), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(&_playerPos.y), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(&player_spawn_point.x), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(&player_spawn_point.y), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(lifes), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(&player_dir), sizeof(int));
	player.setPosition(_playerPos);

	// Enemies
	fileOpen.read(reinterpret_cast<char*>(&n_enemy_game), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(&n_enemy_current), sizeof(int));

	enemies.resize(0);
	enemies_rect.resize(0);

	/*std::vector<sf::RectangleShape> eshoots_effect;
	std::vector<sf::IntRect> eshoots_effect_rect;
	std::vector<int> t_Eshoot_t;
	std::vector<bool> is_ef_eshoots;*/
	for (int i = 0; i < n_enemy_current; i++) {
		eshoots_effect.push_back(m_Eshoot_effect);
		eshoots_effect_rect.push_back(m_Eshoot_ef_rect);
		t_Eshoot_t.push_back(0);
		is_ef_eshoots.push_back(0);
	}


	for (int i = 0; i < n_enemy_current; i++) {
		enemies.push_back(m_enemy);
		enemies_rect.push_back(m_enemy_rect);
	}
	enemies_dir.resize(n_enemy_current);
	enemies_movement.resize(n_enemy_current);
	for (int i = 0; i < n_enemy_current; i++) {
		enemies_movement[i].x = enemies_movement[i].y = 0;
	}
	enemies_random_dir_t.resize(n_enemy_current);
	enemies_count_t.resize(n_enemy_current);
	for (int i = 0; i < n_enemy_current; i++) {
		enemies_count_t[i] = 0;
	}
	enemies_random_shoot_t.resize(n_enemy_current);
	enemies_count_shoot_t.resize(n_enemy_current);
	for (int i = 0; i < n_enemy_current; i++) {
		enemies_count_shoot_t[i] = 0;
	}
	for (int i = 0; i < n_enemy_current; i++) {
		sf::Vector2f _pos;
		fileOpen.read(reinterpret_cast<char*>(&_pos.x), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&_pos.y), sizeof(int));
		enemies[i].setPosition(_pos);
		fileOpen.read(reinterpret_cast<char*>(&enemies_dir[i]), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&enemies_random_dir_t[i]), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&enemies_random_shoot_t[i]), sizeof(int));
	}
	// Spawners
	int nSpawners;
	fileOpen.read(reinterpret_cast<char*>(&nSpawners), sizeof(int));
	spawners.resize(nSpawners);
	for (int i = 0; i < nSpawners; i++) {
		fileOpen.read(reinterpret_cast<char*>(&spawners[i].x), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&spawners[i].y), sizeof(int));
	}
	spawner_cooldown_t = 0;

	// Bricks
	int nBricks = 0;
	fileOpen.read(reinterpret_cast<char*>(&nBricks), sizeof(int));

	bricks.resize(0);
	bricks_rect.resize(0);
	for (int i = 0; i < nBricks; i++) bricks.push_back(m_brick);
	for (int i = 0; i < nBricks; i++) bricks_rect.push_back(m_brick_rect);
	bricks_hp.resize(nBricks);
	for (int i = 0; i < nBricks; i++) {
		sf::Vector2f _pos;
		fileOpen.read(reinterpret_cast<char*>(&_pos.x), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&_pos.y), sizeof(int));
		bricks[i].setPosition(_pos);

		fileOpen.read(reinterpret_cast<char*>(&bricks_hp[i]), sizeof(int));
	}
	// Grasses
	int nGrasses;
	fileOpen.read(reinterpret_cast<char*>(&nGrasses), sizeof(int));
	grasses.resize(0);
	for (int i = 0; i < nGrasses; i++) grasses.push_back(m_grass);
	for (int i = 0; i < nGrasses; i++) {
		sf::Vector2f _pos;
		fileOpen.read(reinterpret_cast<char*>(&_pos.x), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&_pos.y), sizeof(int));
		grasses[i].setPosition(_pos);
	}
	// Bedrocks
	int nBedrocks;
	fileOpen.read(reinterpret_cast<char*>(&nBedrocks), sizeof(int));
	bedrocks.resize(0);
	for (int i = 0; i < nBedrocks; i++) bedrocks.push_back(m_bedrock);
	for (int i = 0; i < nBedrocks; i++) {
		sf::Vector2f _pos;
		fileOpen.read(reinterpret_cast<char*>(&_pos.x), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&_pos.y), sizeof(int));
		bedrocks[i].setPosition(_pos);
	}
	// Portals
	int nPortals;
	fileOpen.read(reinterpret_cast<char*>(&nPortals), sizeof(int));

	portals.resize(0);
	for (int i = 0; i < nPortals; i++) portals.push_back(m_portal);

	portals_cooldown_t.resize(0);
	for (int i = 0; i < nPortals; i++) portals_cooldown_t.push_back(0);

	portals_rect.resize(0);
	for (int i = 0; i < nPortals; i++) portals_rect.push_back(m_portal_rect);
	portals_des.resize(nPortals);
	portals_ready.resize(nPortals);

	for (int i = 0; i < nPortals; i++) {
		sf::Vector2f _pos;
		fileOpen.read(reinterpret_cast<char*>(&_pos.x), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&_pos.y), sizeof(int));
		portals[i].setPosition(_pos);
		fileOpen.read(reinterpret_cast<char*>(&portals_ready[i]), sizeof(bool));
		fileOpen.read(reinterpret_cast<char*>(&portals_des[i]), sizeof(int));
	}

	// Home
	fileOpen.read(reinterpret_cast<char*>(&home_exist), sizeof(bool));
	if (home_exist) {
		fileOpen.read(reinterpret_cast<char*>(&home_dead), sizeof(bool));
		sf::Vector2f _homePos;
		fileOpen.read(reinterpret_cast<char*>(&_homePos.x), sizeof(int));
		fileOpen.read(reinterpret_cast<char*>(&_homePos.y), sizeof(int));
		home.setPosition(_homePos);
	}

	// Info
	fileOpen.read(reinterpret_cast<char*>(&info_n_enemies), sizeof(int));

	fileOpen.read(reinterpret_cast<char*>(&info_n_less), sizeof(int));

	// Close the file after reading all data
	fileOpen.close();
}

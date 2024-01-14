#include "app.h"


void readHighScores(std::vector<std::pair<std::string, int>>& topScore, const std::string& fileName) {
	std::fstream fileOpen(fileName.c_str(), std::ios::in | std::ios::binary);

	if (!fileOpen.is_open()) {
		std::cerr << "Error: Could not open file for reading high scores." << std::endl;
		return;
	}

	topScore.clear();
	int numScores;
	fileOpen.read(reinterpret_cast<char*>(&numScores), sizeof(int));
	for (int i = 0; i < numScores; ++i) {
		int score;
		fileOpen.read(reinterpret_cast<char*>(&score), sizeof(int));

		std::string name;
		std::getline(fileOpen, name, '\0');

		topScore.push_back(std::make_pair(name, score));
	}

	fileOpen.close();
}

void saveHighScores(const std::vector<std::pair<std::string, int>>& topScore, const std::string& fileName) {
	std::fstream fileOpen(fileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

	if (!fileOpen.is_open()) {
		std::cerr << "Error: Could not open file for saving high scores." << std::endl;
		return;
	}

	int numScores = topScore.size();
	fileOpen.write(reinterpret_cast<const char*>(&numScores), sizeof(int));

	for (const auto& entry : topScore) {
		fileOpen.write(reinterpret_cast<const char*>(&entry.second), sizeof(int));
		fileOpen.write(entry.first.c_str(), entry.first.size() + 1);
	}

	fileOpen.close();
}

void insertHighScore(std::vector<std::pair<std::string, int>>& topScore, const std::string& name, int score) {
	if (score == 0) return;
	topScore.push_back(std::make_pair(name, score));
	std::sort(topScore.begin(), topScore.end(), [](const auto& a, const auto& b) {
		return a.second > b.second; // Sort in descending order of scores
		});
}

void saveDataToFile(const game_data* gD, const player_data* pD, const std::string& nameF, int level) {
	std::fstream fileOpen(nameF.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

	//game data
	fileOpen.write(reinterpret_cast<const char*>(&gD->n_enemy_to_keel), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&gD->n_enemy_max_screen), sizeof(int));
	fileOpen.write(reinterpret_cast<const char*>(&gD->diffCode), sizeof(int));

	//player data
	int playerNameSize = pD->name.size();
	fileOpen.write(reinterpret_cast<const char*>(&playerNameSize), sizeof(int));
	fileOpen.write(pD->name.c_str(), playerNameSize);
	fileOpen.write(reinterpret_cast<const char*>(pD->stats), sizeof(int) * 4);
	fileOpen.write(reinterpret_cast<const char*>(&pD->score), sizeof(int));

	fileOpen.write(reinterpret_cast<const char*>(&level), sizeof(int));

	fileOpen.close();
}

void readDataFromFile(game_data* gD, player_data* pD, const std::string& nameF, int& level) {
	std::fstream fileOpen(nameF.c_str(), std::ios::in | std::ios::binary);
	if (!fileOpen.is_open()) {
		return;
	}
	fileOpen.read(reinterpret_cast<char*>(&gD->n_enemy_to_keel), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(&gD->n_enemy_max_screen), sizeof(int));
	fileOpen.read(reinterpret_cast<char*>(&gD->diffCode), sizeof(int));
	int playerNameSize;
	fileOpen.read(reinterpret_cast<char*>(&playerNameSize), sizeof(int));
	pD->name.resize(playerNameSize);
	fileOpen.read(&pD->name[0], playerNameSize);
	fileOpen.read(reinterpret_cast<char*>(pD->stats), sizeof(int) * 4);
	fileOpen.read(reinterpret_cast<char*>(&pD->score), sizeof(int));

	fileOpen.read(reinterpret_cast<char*>(&level), sizeof(int));
	fileOpen.close();
}



void app::run(SoundManager& sm) {
	srand(time(0));
	std::vector<std::pair<std::string, int>> topScore[3];
	game_data* gData = NULL;
	player_data* pData = NULL;
	int cur_level = 1;
	int saveSlot = 0;
	bool isContinue = false;
	std::string screenCode = "MAIN MENU";
	sf::Vector2i coords;
	mainmenu mainMenu;
	newgame newGame;
	std::vector<game_data> gDs;
	std::vector<player_data> pDs;

	//first run
	mainMenu.window = new sf::RenderWindow(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
	mainMenu.display(sm);
	coords = mainMenu.coords;
	screenCode = mainMenu.screenCode;

	while (true) {
		if (screenCode == "EXIT") {
			exit(0);
		}
		else if (screenCode == "MAIN MENU") {
			mainMenu.window = new sf::RenderWindow(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
			mainMenu.window->setPosition(coords);
			mainMenu.display(sm);
			coords = mainMenu.coords;
			screenCode = mainMenu.screenCode;
		}
		else if (screenCode == "PLAY") {
			sf::Font fnt; fnt.loadFromFile("font/MobileFont.ttf");
			sf::Texture bg_texture; bg_texture.loadFromFile("png/save_bg.png");
			sf::RectangleShape bg({ 1248,702 });
			bg.setTexture(&bg_texture);
			sf::RenderWindow tmp_window(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
			tmp_window.setPosition(coords);
			std::vector<sf::Text> texts;
			sf::Text title("CHOOSE YOUR SAVE SLOT", fnt, 50);
			title.setPosition(1248 / 2 - title.getLocalBounds().width / 2, 30);
			int y = 110;
			int select = 0;

			std::vector<int> lvls;

			//read
			gDs.resize(0); pDs.resize(0);
			for (int i = 0; i <= 4; i++) {
				game_data x; player_data y; int lv = 0;
				std::string path = "save/data/";
				readDataFromFile(&x, &y, path + std::to_string(i) + ".bin", lv);
				//saveDataToFile(&x, &y, path + std::to_string(i) + ".bin", lv);
				x.update();
				gDs.push_back(x); pDs.push_back(y); lvls.push_back(lv);
			}
			//exit(0);
			for (int i = 0; i <= 4; i++) {
				std::string str = "Slot " + std::to_string(i) + ": ";
				if (pDs[i].name == "") str += "Empty";
				else {
					str += pDs[i].name + " - score: "; str += std::to_string(pDs[i].score*10);
					switch (gDs[i].diffCode) {
					case 0:
						str += " - Easy"; break;
					case 1:
						str += " - Medium"; break;
					case 2:
						str += " - Hard"; break;
					}

					str += " - Level: " + std::to_string(lvls[i]);
				}
				sf::Text m_text(str, fnt, 50); m_text.setFillColor(sf::Color::Black);
				m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, y);
				texts.push_back(m_text);
				y += 80;
			}
			sf::Text m_text("BACK", fnt, 50); m_text.setFillColor(sf::Color::Black);
			m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, y);
			texts.push_back(m_text);

			_show_save_game:
			while (tmp_window.isOpen()) {
				sf::Event tmp_event;
				while (tmp_window.pollEvent(tmp_event)) {
					if (tmp_event.type == sf::Event::Closed) {
						exit(0);
					}
					if (tmp_event.type == sf::Event::KeyPressed) {
						if (tmp_event.key.code == sf::Keyboard::Enter) {
							sm.Button_Play();
							if (select != texts.size() - 1) {
								if (pDs[select].name == "") {
									screenCode = "NEW GAME";
									cur_level = 1;
								}
								else {
									sf::RenderWindow _ask(sf::VideoMode(660, 200), "Tank game", sf::Style::Default);
									_ask.setPosition({ tmp_window.getPosition().x + (int)tmp_window.getSize().x / 2 - 330,
										tmp_window.getPosition().y + (int)tmp_window.getSize().y / 2 - 100 });

									sf::Text title("Continue or new game?", fnt, 50);
									title.setFillColor(sf::Color::White);
									title.setPosition(_ask.getSize().x / 2 - title.getLocalBounds().width / 2, _ask.getSize().y / 2 - 60);

									sf::Text option1("Continue", fnt, 50); option1.setFillColor(sf::Color::White);
									sf::Text option2("New game", fnt, 50); option2.setFillColor(sf::Color::White);

									option1.setPosition(30, 130);
									option2.setPosition(375, 130);

									bool ans = 0;
									while (_ask.isOpen()) {
										sf::Event _ask_event;
										while (_ask.pollEvent(_ask_event)) {
											if (_ask_event.type == sf::Event::Closed) {
												_ask.close();
												goto _show_save_game;
												
											}
											if (_ask_event.type == sf::Event::KeyPressed) {
												if (_ask_event.key.code == sf::Keyboard::A ||
													_ask_event.key.code == sf::Keyboard::D)
													ans = !ans;
												if (_ask_event.key.code == sf::Keyboard::Enter) {
													sm.Button_Play();
													if (ans) {
														screenCode = "NEW GAME";
														cur_level = 1;
														goto _play_newGame;
													}
													_ask.close();
												}
											}
										}
										if (!ans) {
											option1.setFillColor(sf::Color::Cyan);
											option2.setFillColor(sf::Color::White);
										}
										else {
											option1.setFillColor(sf::Color::White);
											option2.setFillColor(sf::Color::Cyan);
										}

										_ask.clear(sf::Color::Black);
										_ask.draw(title);
										_ask.draw(option1);
										_ask.draw(option2);
										_ask.display();
									}



									gData = &gDs[select];
									pData = &pDs[select];
									screenCode = "game";
									cur_level = lvls[select];
									isContinue = true;

								}
							}
							else screenCode = "MAIN MENU";
						_play_newGame:
							saveSlot = select;
							coords = tmp_window.getPosition();
							tmp_window.close();
						}
						if (tmp_event.key.code == sf::Keyboard::S) {
							if (select == texts.size() - 1) select = 0;
							else select++;
						}
						if (tmp_event.key.code == sf::Keyboard::W) {
							if (select == 0) select = texts.size() - 1;
							else select--;
						}
					}
				}
				tmp_window.clear();
				tmp_window.draw(bg);
				tmp_window.draw(title);
				for (int i = 0; i < texts.size(); i++) {
					if (i == select) {
						texts[i].setFillColor(sf::Color::Blue);
					}
					else 	texts[i].setFillColor(sf::Color::Black);
					tmp_window.draw(texts[i]);
				}
				tmp_window.display();

			}
		}
		else if (screenCode == "NEW GAME") {
			newGame.window = new sf::RenderWindow(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
			newGame.name = "";
			newGame.texts[1].setString("");
			newGame.window->setPosition(coords);
			newGame.display(sm);
			coords = newGame.coords;
			screenCode = newGame.screenCode;
			//get output
			gData = new game_data(10, 2, newGame.difficulty);
			gData->update();
			pData = new player_data(1, 1, 1, 1, newGame.name, 0);
		}
		else if (screenCode == "HIGH SCORE") {
			topScore[0].resize(0);
			topScore[1].resize(0);
			topScore[2].resize(0);
			readHighScores(topScore[0], "save/highscore/highscore_easy.bin");
			readHighScores(topScore[1], "save/highscore/highscore_medium.bin");
			readHighScores(topScore[2], "save/highscore/highscore_hard.bin");
			int n_cur_size = topScore[0].size();
			for (int i = n_cur_size; i <= 8; i++) {
				topScore[0].push_back(std::make_pair("NONE", 0));
			}
			n_cur_size = topScore[1].size();
			for (int i = n_cur_size; i <= 8; i++) {
				topScore[1].push_back(std::make_pair("NONE", 0));
			}
			n_cur_size = topScore[2].size();
			for (int i = n_cur_size; i <= 8; i++) {
				topScore[2].push_back(std::make_pair("NONE", 0));
			}
			/*saveHighScores(topScore[0], "save/highscore/highscore_easy.bin");
			saveHighScores(topScore[1], "save/highscore/highscore_medium.bin");
			saveHighScores(topScore[2], "save/highscore/highscore_hard.bin");
			exit(0);*/
			sf::Texture bg_texture; bg_texture.loadFromFile("png/save_bg.png");
			sf::RectangleShape bg({ 1248,702 });
			bg.setTexture(&bg_texture);
			sf::RenderWindow tmp_window(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
			tmp_window.setPosition(coords);
			sf::Font fnt; fnt.loadFromFile("font/MobileFont.ttf");
			//objects
			sf::Text title("HIGH SCORE LIST", fnt, 50);
			title.setFillColor(sf::Color::Black);
			title.setOutlineThickness(2);
			title.setOutlineColor(sf::Color::White);
			title.setPosition(1248 / 2 - title.getLocalBounds().width/2, 30);
			std::vector<sf::Text> ranks[3];
			int y = 100;
			for (int i = 0; i < 3; i++) {
				y = 100;
				for (int j = 0; j < topScore[i].size(); j++) {
					std::string str = std::to_string(j + 1) + ":   " + topScore[i][j].first + " --------------- " + std::to_string(topScore[i][j].second);
					sf::Text m_text(str.c_str(), fnt, 35);
					if(i==0) m_text.setFillColor(sf::Color::Green);
					else if(i==1) m_text.setFillColor(sf::Color::Yellow);
					else m_text.setFillColor(sf::Color::Red);
					m_text.setStyle(sf::Text::Style::Bold);
					m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, y);
					m_text.setOutlineThickness(2);
					m_text.setOutlineColor(sf::Color::Black);
					y += 50;
					ranks[i].push_back(m_text);
				}
			}

			sf::Text m_text("Enter to back", fnt, 35);
			m_text.setFillColor(sf::Color::White);
			m_text.setStyle(sf::Text::Style::Bold);
			m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, y + 50);
			m_text.setOutlineThickness(2);
			m_text.setOutlineColor(sf::Color::Black);
			ranks[0].push_back(m_text);
			ranks[1].push_back(m_text);
			ranks[2].push_back(m_text);

			int select = 0;

			while (tmp_window.isOpen()) {
				sf::Event tmp_event;
				while (tmp_window.pollEvent(tmp_event)) {
					if (tmp_event.type == sf::Event::Closed) {
						exit(0);
					}
					if (tmp_event.type == sf::Event::KeyPressed) {
						if (tmp_event.key.code == sf::Keyboard::Enter) {
							sm.Button_Play();
							coords = tmp_window.getPosition();
							tmp_window.close();
							screenCode = "MAIN MENU";
						}
						if (tmp_event.key.code == sf::Keyboard::A || tmp_event.key.code == sf::Keyboard::W) {
							if (select == 0) select = 2; else select--;
						}
						if (tmp_event.key.code == sf::Keyboard::D || tmp_event.key.code == sf::Keyboard::S) {
							if (select == 2) select = 0; else select++;
						}
					}
				}
				tmp_window.clear();
				tmp_window.draw(bg);
				tmp_window.draw(title);
				for (int j = 0; j < ranks[select].size(); j++) {
					tmp_window.draw(ranks[select][j]);
				}
				tmp_window.display();

			}

		}
		else if (screenCode == "INSTRUCTION") {
			sf::Texture bg_texture; bg_texture.loadFromFile("png/instruction.png");
			sf::RectangleShape bg({ 1248,702 });
			bg.setTexture(&bg_texture);
			sf::RenderWindow tmp_window(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
			tmp_window.setPosition(coords);
			while (tmp_window.isOpen()) {
				sf::Event tmp_event;
				while (tmp_window.pollEvent(tmp_event)) {
					if (tmp_event.type == sf::Event::Closed) {
						exit(0);
					}
					if (tmp_event.type == sf::Event::KeyPressed) {
						if (tmp_event.key.code == sf::Keyboard::Enter) {
							sm.Button_Play();
							coords = tmp_window.getPosition();
							tmp_window.close();
							screenCode = "MAIN MENU";
						}
					}
				}
				tmp_window.clear();
				tmp_window.draw(bg);
				tmp_window.display();

			}
		}
		else if (screenCode == "ABOUT US") {
			sf::Texture bg_texture; bg_texture.loadFromFile("png/aboutus.png");
			sf::RectangleShape bg({ 1248,702 });
			bg.setTexture(&bg_texture);
			sf::RenderWindow tmp_window(sf::VideoMode(1248, 702), "Tank game", sf::Style::Default);
			tmp_window.setPosition(coords);
			while (tmp_window.isOpen()) {
				sf::Event tmp_event;
				while (tmp_window.pollEvent(tmp_event)) {
					if (tmp_event.type == sf::Event::Closed) {
						exit(0);
					}
					if (tmp_event.type == sf::Event::KeyPressed) {
						if (tmp_event.key.code == sf::Keyboard::Enter) {
							sm.Button_Play();
							coords = tmp_window.getPosition();
							tmp_window.close();
							screenCode = "MAIN MENU";
						}
					}
				}
				tmp_window.clear();
				tmp_window.draw(bg);
				tmp_window.display();

			}
		}
		else if (screenCode == "game") {
			sm.Start_Up_Play();
			game gameplay(*gData, *pData, cur_level, saveSlot);
			newGame.difficulty = gData->diffCode;
			if (isContinue) {
				std::string path = "save/bin/" + std::to_string(saveSlot) + ".bin";
				gameplay.loadStatus(path);
				isContinue = false;
			}
			gameplay.loop(sm);
			if (gameplay.screenCode == "save") {
				std::string path = "save/data/";
				saveDataToFile(gData, pData, path + std::to_string(saveSlot) + ".bin", cur_level);
				newGame.difficulty = gData->diffCode;
				screenCode = "MAIN MENU";
			}
			if (gameplay.game_win) {
				cur_level += 1;
				int cur_lifes = *gameplay.lifes;
				switch (cur_level) {
				case 2:
					gData = new game_data(15, 5, newGame.difficulty);
					gData->lifes = cur_lifes;
					break;
				case 3:
					gData = new game_data(20, 10, newGame.difficulty);
					gData->lifes = cur_lifes;
					break;
				case 4:
					gData = new game_data(-1, 100, newGame.difficulty);
					gData->lifes = cur_lifes;
					break;
				}
			}
			if (gameplay.game_lose) {
				insertHighScore(topScore[newGame.difficulty], pData->name, pData->score * 10);
				if (topScore[newGame.difficulty].size() > 9) topScore[newGame.difficulty].resize(9);
				switch (newGame.difficulty) {
				case 0:
					saveHighScores(topScore[0], "save/highscore/highscore_easy.bin"); break;
				case 1:
					saveHighScores(topScore[1], "save/highscore/highscore_medium.bin"); break;
				case 2:
					saveHighScores(topScore[2], "save/highscore/highscore_hard.bin"); break;
				}
				//clear saveGame
				game_data x; player_data y;
				saveDataToFile(&x, &y, "save/data/" + std::to_string(saveSlot) + ".bin", 0);
				screenCode = "summary";
			}
		}
		else if (screenCode == "summary") {
			sm.Displayed_Score_Play();
			sf::RenderWindow s_window(sf::VideoMode({ 1248,702 }), "Tank game", sf::Style::Default);
			s_window.setPosition(coords);
			sf::Texture s_texture; s_texture.loadFromFile("png/summary.png");
			sf::Font fnt; fnt.loadFromFile("font/MobileFont.ttf");
			sf::RectangleShape s_bg({ 1248, 702 });
			s_bg.setTexture(&s_texture);
			std::vector<sf::Text> texts;
			sf::Text m_text("SUMMARY", fnt, 45);
			m_text.setPosition(40, 40);
			m_text.setOutlineColor(sf::Color::Black);
			m_text.setOutlineThickness(2);
			m_text.setStyle(sf::Text::Style::Bold);
			texts.push_back(m_text);

			m_text.setString("TANK:   " + pData->name);
			m_text.setPosition(40, 100);
			texts.push_back(m_text);

			m_text.setString("SCORE:   " + std::to_string(pData->score*10));
			m_text.setPosition(40, 160);
			texts.push_back(m_text);

			std::string temp = "MODE:    ";
			switch (newGame.difficulty) {
			case 0: temp += "EASY"; break;
			case 1: temp += "MEDIUM"; break;
			case 2: temp += "HARD"; break;
			}
			m_text.setString(temp.c_str());
			m_text.setPosition(40, 240);
			texts.push_back(m_text);

			m_text.setString("Enter to back");
			m_text.setPosition(40, 360);
			texts.push_back(m_text);

			while (s_window.isOpen()) {
				sf::Event s_event;
				while (s_window.pollEvent(s_event)) {
					if (s_event.type == sf::Event::Closed) {
						s_window.close();
						screenCode = "EXIT";
					}
					if (s_event.type == sf::Event::KeyPressed && s_event.key.code == sf::Keyboard::Enter) {
						screenCode = "MAIN MENU";
						sm.Button_Play();
						coords = s_window.getPosition();
						s_window.close();
					}
				}
				s_window.clear();
				s_window.draw(s_bg);
				for (int i = 0; i < texts.size(); i++) {
					s_window.draw(texts[i]);
				}
				s_window.display();
			}
		}
	}

	return;
}
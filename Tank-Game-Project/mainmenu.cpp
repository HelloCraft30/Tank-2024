#include "mainmenu.h"

mainmenu::mainmenu() {
	bg.setSize({ 1248,702 });
	bg_texture.loadFromFile("png/mainmenu.png");
	bg.setTexture(&bg_texture);
	window = NULL;
	fnt.loadFromFile("font/MobileFont.ttf");

	sf::Text m_text("MAINMENU", fnt, 50);
	m_text.setFillColor(sf::Color::Blue);
	int x = 1248 / 2;
	int y = 702 / 2;
	sf::FloatRect m_bounds = m_text.getLocalBounds();
	y -= m_bounds.height * 5;

	m_text.setString("PLAY");
	m_bounds = m_text.getLocalBounds();
	y += m_bounds.height;
	m_text.setPosition(x - m_bounds.width / 2, y);
	texts.push_back(m_text);

	m_text.setString("HIGH SCORE");
	m_bounds = m_text.getLocalBounds();
	y += m_bounds.height + 25;
	m_text.setPosition(x - m_bounds.width / 2, y);
	texts.push_back(m_text);

	m_text.setString("INSTRUCTION");
	m_bounds = m_text.getLocalBounds();
	y += m_bounds.height + 25;
	m_text.setPosition(x - m_bounds.width / 2, y);
	texts.push_back(m_text);

	m_text.setString("MUSIC:   ON");
	m_bounds = m_text.getLocalBounds();
	y += m_bounds.height + 25;
	m_text.setPosition(x - m_bounds.width / 2, y);
	texts.push_back(m_text);

	m_text.setString("ABOUT US");
	m_bounds = m_text.getLocalBounds();
	y += m_bounds.height + 25;
	m_text.setPosition(x - m_bounds.width / 2, y);
	texts.push_back(m_text);

	m_text.setString("EXIT");
	m_bounds = m_text.getLocalBounds();
	y += m_bounds.height + 25;
	m_text.setPosition(x - m_bounds.width / 2, y);
	texts.push_back(m_text);
}

void mainmenu::display(SoundManager& sm) {
	texts[0].setFillColor(sf::Color::Red);
	int i = 0;
	int n = texts.size();
	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				coords = window->getPosition();
				screenCode = "EXIT";
				window->close();
			}
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::W:
					if (i == 0) i = n - 1;
					else i--;
					break;
				case sf::Keyboard::S:
					if (i == n - 1) i = 0;
					else i++;
					break;
				}
				screenCode = texts[i].getString();
				if (event.key.code == sf::Keyboard::Enter) {
					sm.Button_Play();
					if (screenCode == "MUSIC:   ON") {
						texts[i].setString("MUSIC:   OFF");
						sm.Switch_Sound_Effects();
						break;
					}
					else if (screenCode == "MUSIC:   OFF") {
						texts[i].setString("MUSIC:   ON");
						sm.Switch_Sound_Effects();
						break;
					}
					coords = window->getPosition();
					window->close();
				}
			}

		}

		window->clear(); window->draw(bg);

		for (int j = 0; j < texts.size(); j++) {
			if (j == i) {
				texts[j].setFillColor(sf::Color::Red);
			}
			else {
				texts[j].setFillColor(sf::Color::Blue);
			}
			window->draw(texts[j]);
		}
		window->display();
	}

}

newgame::newgame() {
	bg.setSize({ 1248,702 });
	bg_texture.loadFromFile("png/mainmenu.png");
	bg.setTexture(&bg_texture);
	window = NULL;
	fnt.loadFromFile("font/MobileFont.ttf");

	sf::Text m_text("NEW GAME", fnt, 50);
	m_text.setFillColor(sf::Color::White);
	int x = 1248 / 2;
	int y = 702 / 2;
	sf::FloatRect m_bounds = m_text.getLocalBounds();
	y -= m_bounds.height * 3;

	title = m_text; title.setFillColor(sf::Color::Black);
	title.setPosition(x - title.getLocalBounds().width / 2, y - 2 * title.getLocalBounds().height / 2);

	m_text.setString("Nickname");
	m_bounds = m_text.getLocalBounds();
	y += m_bounds.height;
	m_text.setPosition(x - m_bounds.width / 2, y);
	texts.push_back(m_text);

	m_text.setString("");
	y += m_bounds.height + 20;
	m_text.setPosition(x - m_bounds.width / 2, y);

	texts.push_back(m_text);
	nameCoord.x = 1248 / 2;
	nameCoord.y = y;


	nameBox.setFillColor(sf::Color::Transparent);
	nameBox.setOutlineThickness(2);
	nameBox.setOutlineColor(sf::Color::Black);
}

void newgame::display(SoundManager& sm) {
	texts[0].setFillColor(sf::Color::Red);
	int i = 3;
	int n = texts.size();
	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			std::string tmp = texts[1].getString();
			if (event.type == sf::Event::Closed) {
				coords = window->getPosition();
				screenCode = "EXIT";
				window->close();
			}
			if (name == "") {
				if (texts[1].getString() != "--Limited--Length--") {
					texts[1].setFillColor(sf::Color::White);
				}
				if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::BackSpace) {
						if (tmp.size()) tmp.pop_back();
					}
					if (event.key.code == sf::Keyboard::Enter) {
						if (texts[1].getString() == "--Limited--Length--") break;
						sm.Button_Play();
						name = texts[1].getString();
						texts[1].setFillColor(sf::Color::White);
						texts[1].setOutlineThickness(2);
						texts[1].setOutlineColor(sf::Color::Black);

						sf::Text m_text("Difficulty", fnt, 50);
						m_text.setFillColor(sf::Color::Red);
						m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, nameBox.getPosition().y + 25);
						texts.push_back(m_text);

						m_text.setString("Easy");
						m_text.setFillColor(sf::Color::Green);
						m_text.setOutlineThickness(2);
						m_text.setOutlineColor(sf::Color::Black);
						m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, nameBox.getPosition().y + 25 + 55);
						texts.push_back(m_text);

						m_text.setString("Medium");
						m_text.setFillColor(sf::Color::Yellow);
						m_text.setOutlineThickness(2);
						m_text.setOutlineColor(sf::Color::Black);
						m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, nameBox.getPosition().y + 25 + 110);
						texts.push_back(m_text);

						m_text.setString("Hard");
						m_text.setFillColor(sf::Color::Red);
						m_text.setOutlineThickness(2);
						m_text.setOutlineColor(sf::Color::Black);
						m_text.setPosition(1248 / 2 - m_text.getLocalBounds().width / 2, nameBox.getPosition().y + 25 + 165);
						texts.push_back(m_text);
					}
				}
				if (event.type == sf::Event::TextEntered) {
					if (event.text.unicode <= 126 && event.text.unicode >= 32) {
						tmp += event.text.unicode;
					}
				}
				if (tmp.size() >= 20) {
					tmp = "--Limited--Length--";
					texts[1].setFillColor(sf::Color::Red);
				}
				texts[1].setString(tmp);
			}
			else {
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::W:
						if (i == 3) i = 5;
						else i--;
						break;
					case sf::Keyboard::S:
						if (i == 5) i = 3;
						else i++;
						break;
					case sf::Keyboard::Enter:
						sm.Button_Play();
						difficulty = i - 3;
						coords = window->getPosition();
						screenCode = "game";
						texts[i].setFillColor(sf::Color::White);
						window->draw(texts[i]);
						window->display();
						Sleep(1500);
						window->close();
					}
				}
			}

		}

		window->clear(); window->draw(bg);

		nameCoord.x = 1248 / 2 - texts[1].getLocalBounds().width / 2;
		texts[1].setPosition(nameCoord);

		nameBox.setSize({ texts[1].getLocalBounds().width + 40,54 });
		nameBox.setOrigin(nameBox.getSize().x / 2, nameBox.getSize().y / 2);
		nameBox.setPosition(1248 / 2, nameCoord.y + 34);

		window->draw(texts[0]);
		//window->draw(nameBox);
		window->draw(texts[1]);

		if (name != "") {
			window->draw(texts[2]);
			texts[3].setFillColor(sf::Color::Green);
			texts[4].setFillColor(sf::Color::Yellow);
			texts[5].setFillColor(sf::Color::Red);

			texts[3].setOutlineThickness(2);
			texts[4].setOutlineThickness(2);
			texts[5].setOutlineThickness(2);

			texts[i].setOutlineThickness(4);
			texts[i].setOutlineColor(sf::Color::Black);

			window->draw(texts[3]);
			window->draw(texts[4]);
			window->draw(texts[5]);
		}

		window->draw(title);

		window->display();
	}

}

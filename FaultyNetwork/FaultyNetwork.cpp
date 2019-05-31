// FaultyStorage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "SFML/Network.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "NetworkHelper.h"
#include "EditBox.h"
#include "TextField.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 800), "Faulty Network");
	sf::Font fnt;
	fnt.loadFromFile("arial.ttf");
	
	EditBox textEntry;
	int height = 20;
	textEntry.InitEditBox(0, window.getSize().y - height, window.getSize().x, height, fnt);
	TextField textField;
	textField.InitTextField(0, 0, window.getSize().x, window.getSize().y);

	std::string port;
	std::cout << "Enter a port number" << std::endl;
	std::cin >> port;

	NetworkHelper network(std::stoi(port));

	sf::Clock clock;
	double elapsedTime = 0.0;

	bool canDoAction = true;

	while (window.isOpen())
	{
		sf::Time elapsed = clock.restart();
		elapsedTime = elapsed.asSeconds();
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return && !textEntry.GetText().empty())
			{
				network.SendChatMessage(textEntry.GetText());
				//textField.AddMessage(textEntry.GetText());
				textEntry.ClearText();
			}

			textEntry.ProcessInput(event, window);
		}

		network.ReceiveMessage(textField);
		window.clear();
		textField.Render(window);
		textEntry.Render(window);
		window.display();
	}
	return 0;
}
#pragma once
#include "SFML/Graphics.hpp"

class TextField
{
public:
	TextField();
	void InitTextField(int x, int y, int width, int height);
	void Render(sf::RenderWindow& window);
	void SetHideCharacters(bool hide);
	std::string GetText();
	void SetText(std::string text);
	bool m_bModified;
	std::vector<sf::Text> m_Messages;
	void AddMessage(std::string message);
private:
	bool m_HasFocus;
	bool m_HideCharacters;
	bool m_NewLine;
	sf::Font m_Font;

	sf::IntRect m_Rect;
	sf::RectangleShape m_GuiRect;
	sf::Text m_Text;
	std::string m_EnteredString;
};
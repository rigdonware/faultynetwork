#pragma once
#include "SFML/Graphics.hpp"

class EditBox
{
public:
	EditBox();
	void InitEditBox(int x, int y, int width, int height, sf::Font& font, std::string initialText = "");
	void Render(sf::RenderWindow& window);
	void ProcessInput(sf::Event event, sf::RenderWindow& window);
	void SetHideCharacters(bool hide);
	std::string GetText();
	void SetText(std::string text);
	bool m_bModified;
	void ClearText();

private:
	bool m_HasFocus;
	bool m_HideCharacters;
	bool m_NewLine;

	sf::IntRect m_Rect;
	sf::RectangleShape m_GuiRect;
	sf::Text m_Text;
	std::string m_EnteredString;
};
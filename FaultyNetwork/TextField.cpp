#include "pch.h"
#include "TextField.h"

TextField::TextField()
{
	m_HasFocus = false;
	m_HideCharacters = false;
	m_bModified = false;
	m_NewLine = false;

	m_EnteredString = "";
	//m_Text.setStyle(sf::Text::Bold);
	m_Text.setCharacterSize(18);

	m_Font.loadFromFile("arial.ttf");
}

void TextField::InitTextField(int x, int y, int width, int height)
{
	m_GuiRect.setPosition(x, y);
	m_GuiRect.setSize(sf::Vector2f(width, height));
	m_GuiRect.setFillColor(sf::Color(200, 200, 200));

	m_Rect.left = x;
	m_Rect.top = y;
	m_Rect.width = width;
	m_Rect.height = height;
}

void TextField::SetHideCharacters(bool hide)
{
	m_HideCharacters = hide;
}

void TextField::Render(sf::RenderWindow& window)
{
	window.draw(m_GuiRect);
	for (auto& msg : m_Messages)
		window.draw(msg);
	//window.draw(m_Text);
}

void TextField::AddMessage(std::string message)
{
	m_Messages.push_back(sf::Text(message, m_Font, 18));
	m_Messages.at(m_Messages.size() - 1).setPosition(0, (m_Messages.size() - 1) * 20);
	m_Messages.at(m_Messages.size() - 1).setFillColor(sf::Color::Black);
}
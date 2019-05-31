#include "pch.h"
#include "EditBox.h"

EditBox::EditBox()
{
	m_HasFocus = false;
	m_HideCharacters = false;
	m_bModified = false;
	m_NewLine = false;

	m_EnteredString = "";
	//m_Text.setStyle(sf::Text::Bold);
	m_Text.setCharacterSize(18);
}

void EditBox::InitEditBox(int x, int y, int width, int height, sf::Font& font, std::string initialText)
{
	m_GuiRect.setPosition(x, y);
	m_GuiRect.setSize(sf::Vector2f(width, height));
	m_GuiRect.setFillColor(sf::Color::White);

	m_Rect.left = x;
	m_Rect.top = y;
	m_Rect.width = width;
	m_Rect.height = height;

	int textSpacer = 5;
	m_Text.setFont(font);
	m_Text.setPosition(x + textSpacer, y);
	m_Text.setString(initialText);
	m_Text.setFillColor(sf::Color::Black);
}

void EditBox::SetHideCharacters(bool hide)
{
	m_HideCharacters = hide;
}

void EditBox::Render(sf::RenderWindow& window)
{
	window.draw(m_GuiRect);
	window.draw(m_Text);
}

void EditBox::ProcessInput(sf::Event event, sf::RenderWindow& window)
{
	if (event.type == sf::Event::TextEntered)
	{
		if (event.text.unicode == '\b')
		{
			if (m_EnteredString.size() > 0)
				m_EnteredString.erase(m_EnteredString.size() - 1, 1);
		}
		else
			m_EnteredString += event.text.unicode;

		if (m_Text.getGlobalBounds().width > window.getSize().x - m_Text.getCharacterSize() && !m_NewLine)
		{
			m_EnteredString += "\n";
			m_GuiRect.setSize(sf::Vector2f(m_GuiRect.getSize().x, m_GuiRect.getSize().y + 20));
			m_GuiRect.setPosition(0, window.getSize().y - 40);
			m_Text.setPosition(0, window.getSize().y - 40);
			m_NewLine = true;
		}

		m_Text.setString(m_EnteredString);
	}
}

std::string EditBox::GetText()
{
	return m_EnteredString;
}

void EditBox::SetText(std::string text)
{
	m_Text.setString(text);
}

void EditBox::ClearText()
{
	m_Text.setString("");
	m_EnteredString = "";
}
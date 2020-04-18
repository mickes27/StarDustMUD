#include "TelnetMenu.h"

namespace GwiezdnyPyl {


void TelnetMenu::handle(const std::string data)
{
	const auto option = std::stoi(data);
	switch (option)
	{
	case 0:
		connection->close();
		break;
	case 1:
		//connection->addHandler(new TelnetMenuEnter(*m_connection, m_account.ID()));
		break;
	case 2:
		/*if (m_account.Characters() >= m_account.AllowedCharacters())
		{
			m_connection->Protocol().SendString(
				*m_connection,
				"<#FF0000>Sorry, you are not allowed any more characters.\r\n");
			return;
		}
		m_connection->AddHandler(new TelnetMenuNew(*m_connection, m_account.ID()));*/
		break;
	case 3:
		//m_connection->AddHandler(new TelnetMenuDelete(*m_connection, m_account.ID()));
		break;
	case 4:
		//m_connection->AddHandler(new TelnetMenuHelp(*m_connection, m_account.ID()));
		break;
	default:
		connection->getProtocol().sendString(*connection, "<#FF00AA>Option not recognized");
	}
}

void TelnetMenu::printMenu() const
{
	static auto menuStr = "<#FFFFFF>-------------------------------------------------------------------------------\r\n"
		"<#FFFF00> GwiezdnyPyl v1.0 Main Menu\r\n"
		"<#FFFFFF>-------------------------------------------------------------------------------\r\n"
		" 0 - Quit\r\n"
		" 1 - Enter the Game\r\n"
		" 2 - Create a new Character\r\n"
		" 3 - Delete an existing Character\r\n"
		" 4 - View Help\r\n"
		"-------------------------------------------------------------------------------\r\n"
		"<#7F7F7F> Enter Choice: <#FFFFFF>";

	connection->getProtocol().sendString(*connection, menuStr);
}
}

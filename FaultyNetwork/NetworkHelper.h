#pragma once
#include "SFML/Network.hpp"
#include <thread>
#include "TextField.h"

class VotingStructure;

class NetworkHelper
{
public:
	NetworkHelper(unsigned short sendPort);
	void ReceiveMessage(TextField& textField);
	void SendChatMessage(std::string message);
	void SendReadMessage(std::string fileToRead, char fileNum = NULL);
	void SendReadRetryMessage(std::vector<unsigned char> bits);
	void SendWriteMessage(std::string fileToWrite, char fileNum = NULL);
	void SendWriteRetryMessage(std::vector<unsigned char> bits);
	void ReceiveDataMessage(std::vector<unsigned char> bits);
	void ReceiveAckMessage(std::vector<unsigned char> bits);
	VotingStructure* m_SmallText;
	VotingStructure* m_LargeText;
	std::vector<std::string> m_Messages;
	std::string GetBinaryFromCharacter(char c); // convert a character to a group of eight digits
	char GetCharacterFromBinary(std::string s); // convert a group of 8 digits to a character
	std::vector<char> ConvertMessageToBits(std::string message);
	std::vector<char> CreateParityBits(std::vector<char> bits);
	bool IsPowerOfTwo(unsigned long n);
	bool IsEven(int n);
	int CalculateParityValue(int whichBit, std::vector<char> bits);
private:
	sf::IpAddress m_Ip;
	sf::UdpSocket m_Socket;
	unsigned short sendPort;
};
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
	VotingStructure* m_SmallText;
	VotingStructure* m_LargeText;
	std::vector<std::string> m_Messages;
	std::vector<int> GetBinaryFromCharacter(char c); // convert a character to a group of eight digits
	char GetCharacterFromBinary(std::string s); // convert a group of 8 digits to a character
	std::vector<int> ConvertMessageToBits(std::string message);
	std::vector<int> CreateParityBits(std::vector<int> bits);
	std::vector<int> LoadParityVector(std::vector<int> bits);
	bool IsPowerOfTwo(unsigned long n);
	bool IsEven(int n);
	int CalculateParityValue(int whichBit, std::vector<int> bits);
	std::map<int, char> m_Response;
	void VerifyMessage(std::vector<int>& bits);
	std::vector<int> RemoveParityBits(std::vector<int> bits);
	std::vector<char> m_TotalResponse;
	std::vector<std::string> m_ResponseHistory;
private:
	sf::IpAddress m_Ip;
	sf::UdpSocket m_Socket;
	unsigned short sendPort;
};
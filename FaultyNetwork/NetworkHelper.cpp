#include "pch.h"
#include "NetworkHelper.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <queue>
#include <windows.h>

NetworkHelper::NetworkHelper(unsigned short send)
{
	m_Ip = sf::IpAddress::LocalHost;
	m_Socket.setBlocking(false);
	m_Socket.bind(sf::UdpSocket::AnyPort);

	sendPort = send;
	m_MessageId = 1;

	sf::Packet packet;
	packet << "";
	if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
	{
		std::cout << "Failed to send packet" << std::endl;
		std::cout << "Error: " << m_Socket.Error << std::endl;
	}
}

void NetworkHelper::SendChatMessage(std::string message)
{
	if (message == "LargeTextFile.txt")
	{
		std::stringstream ss;
		std::ifstream fin;
		fin.open(message);
		std::string line;

		char test[4] = { 0 };
		fin.read(test, 3);
		if (strcmp(test, "\xEF\xBB\xBF") != 0)
			fin.seekg(0);

		while (std::getline(fin, line))
		{
			ss << line;
			ss << "\n";
		}

		fin.close();

		message = ss.str();
	}

	std::vector<int> bits = ConvertMessageToBits(message);
	std::queue<int> bitQueue;
	for (int i = 0; i < bits.size(); i++)
		bitQueue.push(bits[i]);

	std::vector<int> newBits;
	sf::Packet packet;
	while (!bitQueue.empty())
	{
		newBits.clear();
		for (int i = 0; i < 4; i++)
		{
			newBits.push_back(bitQueue.front());
			bitQueue.pop();
		}
		std::vector<int> parity = CreateParityBits(newBits);
		unsigned char bitBuffer = parity.at(0);
		for (int i = 1; i < 8; i++)
		{
			if (parity.at(i))
				bitBuffer += (1 << i);
		}

		packet.append(&bitBuffer, 1);
	}

	//if (selector.wait(sf::seconds(3)))
	//{
	//	if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
	//	{
	//		std::cout << "Failed to send packet" << std::endl;
	//		std::cout << "Error: " << m_Socket.Error << std::endl;
	//	}
	//}
	//else
	//{
	//	
	//}
	//for (int i = 0; i < 10; i++)
	//{
		
	//}
	for (int i = 0; i < 100; i++)
	{
		if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
		{
			std::cout << "Failed to send packet" << std::endl;
			std::cout << "Error: " << m_Socket.Error << std::endl;
		}
		Sleep(50);
	}
	//m_MessageId++;
}

void NetworkHelper::ReceiveMessage(TextField& textField)
{
	unsigned char buffer[5000];      // The buffer to store raw response data in
	sf::IpAddress respIP;  // The ip address where the response came from
	size_t respSize;     // The amount of data actually written to buffer
	unsigned short receivePort;

	// Now receive a response.  This is a blocking call, meaning your program
	// will hang until a response is received.
	m_Socket.receive(buffer, 5000, respSize, respIP, receivePort);
	
	if (respSize == 0)
		return;

	std::vector<int> bits;
	std::vector<unsigned char> response;
	for (int i = 0; i < respSize; i++)
	{
		response.push_back(buffer[i]);
	}

	//if (response.size() == 4) //starting 
	//	return;
	std::vector<int> responseGroup;
	for (auto& resp : response)
	{
		unsigned char byte = resp;

		std::vector<int> byteArray = GetBinaryFromCharacter(byte);
		std::vector<int> allBytes;
		for (int i = byteArray.size() - 1; i >= 0; i--)
			allBytes.push_back(byteArray[i]);

		std::vector<int> bitsWithoutParity = RemoveParityBits(allBytes);

		if (allBytes.size() > 0)
			VerifyMessage(allBytes);

		bitsWithoutParity = RemoveParityBits(allBytes);

		for (auto& bit : bitsWithoutParity)
			responseGroup.push_back(bit);

		if (responseGroup.size() == 8)
		{
			std::string group = "";
			for (auto& c : responseGroup)
				group += (std::to_string(c));

			char passedInCharacter = GetCharacterFromBinary(group);
			m_TotalResponse.push_back(passedInCharacter);

			responseGroup.clear();
		}
	}

	std::string finalMessage = "";
	for (auto& c : m_TotalResponse)
		finalMessage.push_back(c);

	for (auto& history : m_ResponseHistory)
	{
		if (history == finalMessage)
			return;
	}

	m_ResponseHistory.push_back(finalMessage);

	textField.AddMessage(finalMessage);
	m_TotalResponse.clear();
}

std::vector<int> NetworkHelper::GetBinaryFromCharacter(char c)
{
	std::bitset<8> temp(c);
	std::string stringTemp = temp.to_string();
	std::vector<int> bits;
	for (int i = 0; i < stringTemp.size(); i++)
		bits.push_back(stringTemp[i] - 48);
	return bits;
}

char NetworkHelper::GetCharacterFromBinary(std::string group)
{
	std::bitset<8> temp(group);
	return temp.to_ulong();
}

std::vector<int> NetworkHelper::ConvertMessageToBits(std::string message)
{
	std::vector<int> bits;
	for (int i = 0; i < message.size(); i++)
	{
		std::vector<int> tempBits = GetBinaryFromCharacter(message[i]);
		for (auto bit : tempBits)
			bits.push_back(bit);
	}
	return bits;
}

std::vector<int> NetworkHelper::RemoveParityBits(std::vector<int> bits)
{
	std::vector<int> parity;

	for (int i = 1; i < bits.size(); i++)
	{
		if (!IsPowerOfTwo(i))
		{
			parity.push_back(bits.at(i));
		}
	}

	return parity;
}

std::vector<int> NetworkHelper::CreateParityBits(std::vector<int> bits)
{
	std::vector<int> parity = LoadParityVector(bits);
	
	std::cout << "Creating parity bits for: ";
	for (int i = 0; i < bits.size(); i++)
		std::cout << bits[i];

	std::cout << std::endl;

	for (int i = 1; i <= parity.size() - 1; i++)
	{
		if (IsPowerOfTwo(i))
		{
			int val = CalculateParityValue(i, parity);
			parity.at(i) = val;
		}
	}

	std::cout << "Resulting parity binary: ";
	for (int i = 0; i < parity.size() ; i++)
		std::cout << parity[i];
	std::cout << std::endl;

	return parity;
}

bool NetworkHelper::IsPowerOfTwo(unsigned long n)
{
	if (n == 1)
		return true;

	return (n != 0) && ((n & (n - 1)) == 0);
}

bool NetworkHelper::IsEven(int n)
{
	return (n % 2 == 0);
}

int NetworkHelper::CalculateParityValue(int whichBit, std::vector<int> bits)
{
	int parity = 0;

	for (int i = whichBit; i < bits.size(); i+=((whichBit) * 2))
	{
		for (int j = 1; j <= whichBit; j++)
		{
			if((i + j - 1) < bits.size() && (i + j - 1) != whichBit)
				parity += (bits.at(i + j - 1));
		}
	}

	if (IsEven(parity))
		return 0;
		
	return 1;
}

std::vector<int> NetworkHelper::LoadParityVector(std::vector<int> bits)
{
	std::vector<int> parity;
	parity.push_back(NULL);
	int count = 0;

	for (int i = 1; count < bits.size(); i++)
	{
		if (!IsPowerOfTwo(i))
		{
			parity.push_back(bits[count]);
			count++;
		}
		else
			parity.push_back(0);
	}

	return parity;
}

void NetworkHelper::VerifyMessage(std::vector<int>& bits)
{
	std::vector<int> badBits;
	bool hasBadBit = false;

	std::vector<int> bitsWithoutParity = RemoveParityBits(bits);

	int count = 0;

	for (int i = 1; i < bits.size(); i++)
	{
		if (IsPowerOfTwo(i))
		{
			int correctVal = CalculateParityValue(i, bits);
			int currentVal = bits.at(i);
			if (correctVal != currentVal)
			{
				badBits.push_back(i);
				hasBadBit = true;
			}
		}
	}

	int whichBit = 0;
	for (auto& bit : badBits)
	{
		whichBit += bit;
	}
	if(hasBadBit)
		bits.at(whichBit) = (bits.at(whichBit) == 1 ? 0 : 1);
}
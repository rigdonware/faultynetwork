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
	std::vector<int> bits = ConvertMessageToBits(message); //{'1','0','1','1','0','0','1'};// ConvertMessageToBits(message);
	std::queue<int> bitQueue;
	for (int i = 0; i < bits.size(); i++)
		bitQueue.push(bits[i]);

	unsigned int messageId = 1;
	while (!bitQueue.empty())
	{
		std::vector<int> newBits;
		for (int i = 0; i < 8; i++)
		{
			newBits.push_back(bitQueue.front());
			bitQueue.pop();
		}
		

		//for (int amount = 0; amount < 100; amount++)
		//{
			std::vector<int> parity = CreateParityBits(newBits);
			std::vector<unsigned char> parityInChar;
			unsigned char blah[8];
			bool data[8];
			std::string blahlbhal;
			sf::Packet packet;
			int value;
			unsigned char bitBuffer = parity.at(0);
			unsigned char bitBuffer2 = parity.at(8);
			int n;

			for (int i = 1; i < 8; i++)
			{
				if (parity.at(i))
					bitBuffer += (1 << i);
			}
			int position = 1;
			for (int i = 9; i < parity.size(); i++)
			{
				if (parity.at(i))
					bitBuffer2 += (1 << position);
				position++;
			}

			//for (int i = 0; i < 8; i++)
			//{
			//	//if(parity.at(i))
			//		bitBuffer |= (i & parity.at(i) & 0x1);
			//	//if(parity.at(i))
			//	//	bitBuffer |= 1 << parity.at(i);
			//}
			//for (int i = 0; i < 8; i++)
			//{
			//	n = parity.at(i) >> i;
			//}
			//for (int j = 0; j < 8; j++)
			//	blah[j] = parity[j];
			//for (int j = 0; j < 8; j++)
			//	blahlbhal += std::to_string(parity[j]);
			//for (int j = 0; j < parity.size(); j++)
			//	parityInChar.push_back(parity[j]);


			//packet << value;
			packet.append(&messageId, 4);
			packet.append(&bitBuffer, 1);
			packet.append(&bitBuffer2, 1);
			//packet << blah;
			//for (int j = 0; j < parity.size(); j++)
			//	packet << parity.at(j);

			if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
			{
				std::cout << "Failed to send packet" << std::endl;
				std::cout << "Error: " << m_Socket.Error << std::endl;
			}
		//}
		messageId++;
	}
}

void NetworkHelper::ReceiveMessage(TextField& textField)
{
	unsigned char buffer[512];      // The buffer to store raw response data in
	sf::IpAddress respIP;  // The ip address where the response came from
	size_t respSize;     // The amount of data actually written to buffer
	unsigned short receivePort;

	// Now receive a response.  This is a blocking call, meaning your program
	// will hang until a response is received.
	m_Socket.receive(buffer, 512, respSize, respIP, receivePort);
	
	if (respSize == 0)
		return;

	std::vector<int> bits;
	std::vector<unsigned char> response;
	for (int i = 0; i < respSize; i++)
	{
		response.push_back(buffer[i]);
	}

	unsigned int messageId = 0;
	messageId += response.at(0);
	messageId += (response.at(1) * 256);
	messageId += (response.at(2) * 512);
	messageId += (response.at(3) * 1024);

	if (response.size() == 4) //starting 
		return;

	unsigned char byte1 = response.at(4);
	unsigned char byte2 = response.at(5);
	
	std::vector<int> byte1Array = GetBinaryFromCharacter(byte1);
	std::vector<int> byte2Array = GetBinaryFromCharacter(byte2);
	std::vector<int> allBytes;
	for (int i = byte1Array.size() - 1; i >= 0; i--)
		allBytes.push_back(byte1Array[i]);
	for (int i = byte2Array.size() - 1; i >= 0; i--)
		allBytes.push_back(byte2Array[i]);
	//for (int i = 0; i < 8; i++)
	//{
	//	numBits.push_back((byte >> i) + 1);
	//}

	//for (int i = 4; i < response.size(); i++)
	//{
	//	int num = response.at(i);
	//	
	//	if (num != 0 || num != 1)
	//	{
	//		response.at(i) ^= response.at(i);
	//	}
	//	num = response.at(i);
	//	bits.push_back(response.at(i));
	//}

	std::vector<int> bitsWithoutParity = RemoveParityBits(allBytes);

	std::string group;
	for (auto& c : bitsWithoutParity)
		group += (std::to_string(c));

	char passedInCharacter = GetCharacterFromBinary(group);

	std::cout << "passed back character before parity: " << passedInCharacter << std::endl;

	if(allBytes.size() > 0)
		VerifyMessage(allBytes);

	bitsWithoutParity = RemoveParityBits(allBytes);

	group = "";
	for (auto& c : bitsWithoutParity)
		group += (std::to_string(c));

	passedInCharacter = GetCharacterFromBinary(group);

	std::cout << "passed back character after parity: " << passedInCharacter << std::endl;

	m_Response[messageId] = 'c';

	//textField.AddMessage(response);
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
		for (int j = 0; j < whichBit; j++)
		{
			if((i + j) < bits.size())
				parity += (bits.at((i-1) + j));
		}
	}

	if (IsEven(parity))
		return 0;
		
	return 1;
}

std::vector<int> NetworkHelper::LoadParityVector(std::vector<int> bits)
{
	std::vector<int> parity;
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

	int count = 0;

	for (int i = 0; i < bits.size(); i++)
	{
		if (IsPowerOfTwo(i + 1))
		{
			int correctVal = CalculateParityValue(i+1, bits);
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
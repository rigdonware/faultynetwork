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
	std::vector<char> bits = ConvertMessageToBits(message);
	std::queue<char> bitQueue;
	for (int i = 0; i < bits.size(); i++)
		bitQueue.push(bits[i]);

	while (!bitQueue.empty())
	{
		std::vector<char> newBits;
		for (int i = 0; i < 8; i++)
		{
			newBits.push_back(bitQueue.front());
			bitQueue.pop();
		}

		//for (int amount = 0; amount < 100; amount++)
		//{
			std::vector<char> parity = CreateParityBits(newBits);

			sf::Packet packet;
			for(int j = 0; j < parity.size(); j++)
				packet << parity[j];

			if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
			{
				std::cout << "Failed to send packet" << std::endl;
				std::cout << "Error: " << m_Socket.Error << std::endl;
			}
		//}
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

	std::cout << "port bound: " << m_Socket.getLocalPort() << " received message from : " << receivePort << std::endl;

	//if (m_Socket.getLocalPort() == receivePort)
	//	return;

	std::vector<unsigned char> bits;
	std::string response;
	for (int i = 0; i < respSize; i++)
	{
		response.push_back(buffer[i]);
	}
	textField.AddMessage(response);
}

void NetworkHelper::ReceiveAckMessage(std::vector<unsigned char> bits)
{
	/*
		Opcode: D 1 byte
		File name: 32 bytes
		Location: 4 bytes
		Data Length: 1 byte
		Data 10 bytes
		Valid 1 byte
	*/
	std::string fileName = "";
	for (int i = 1; i < 33; i++)
	{
		if (bits.at(i) != 0)
		{
			fileName.push_back(bits.at(i));
			int fileNum = bits.at(i) - '0';
		}
	}

	unsigned int location = 0;
	location += bits.at(33);
	location += (bits.at(34) * 256);
	location += (bits.at(35) * 512);
	location += (bits.at(36) * 1024);

	VotingStructure* structure = NULL;

	if (fileName == "SmallTextFile.txt")
		structure = m_SmallText;
	else if (fileName == "LargeTextFile.txt")
		structure = m_LargeText;

	unsigned int length = bits.at(37);

	//if (length == 0)
	//{
	//	structure->doVoting = true;
	//}

	//unsigned char valid = bits.at(bits.size() - 1);
	//if (!valid)
	//	SendWriteRetryMessage();

	//unsigned char previousBit = bits.at(10);
	//unsigned char newBit; 
	//newBit ^= previousBit;
	//std::cout << newBit;

}

void NetworkHelper::ReceiveDataMessage(std::vector<unsigned char> bits)
{
	/*
	Opcode: D 1 byte
	File name: 32 bytes
	Location: 4 bytes
	Data Length: 1 byte
	Data 10 bytes
	Valid 1 byte
	*/

	unsigned char file[32] = "";
	int tolerantFile = -1;
	int count = 0;
	std::string fileTitle = "Results";
	std::string fileName = "";
	for (int i = 1; i < 33; i++)
	{
		file[count] = bits.at(i);
		if (bits.at(i) != '0')
		{
			fileTitle.push_back(file[count]);
			fileName.push_back(file[count]);
			int fileNum = bits.at(i) - '0';
			if (fileNum > 0 && fileNum < 4)
				tolerantFile = fileNum;
		}
		count++;
	}

	VotingStructure* structure = NULL;

	if (fileName == "SmallTextFile.txt")
		structure = m_SmallText;
	else if (fileName == "LargeTextFile.txt")
		structure = m_LargeText;
	//else if (fileName == "SmallBinary.jpg")
	//	structure = &m_FaultTolerantHelper->m_SmallBinaryFile;
	//else if (fileName == "LargeBinary.wav")
	//	structure = &m_FaultTolerantHelper->m_LargeBinaryFile;

	unsigned int location = 0;
	location += bits.at(33);
	location += (bits.at(34) * 256);
	location += (bits.at(35) * 512);
	location += (bits.at(36) * 1024);

	unsigned int length = bits.at(37);
	unsigned char data[10] = "";

	count = 0;
	
	std::ofstream fout(fileTitle, std::ofstream::out | std::ofstream::app | std::ofstream::binary);

	//for (int i = 38; i < 48; i++)
	//{
	//	data[count] = bits.at(i);
	//	fout << data[count];
	//	if(structure)
	//		structure->m_Contents[tolerantFile].push_back(data[count]);
	//	count++;
	//}

	if (length == 10)
	{
		unsigned char code[1]; code[0] = 'R';
		location += 10;
		sf::Packet packet;
		packet.append(&code, 1);
		packet.append(&file, 32);
		packet.append(&location, 4);

		if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
		{
			std::cout << "Failed to send packet" << std::endl;
			std::cout << "Error: " << m_Socket.Error << std::endl;
		}
	}

	if (length == 0)
		std::cout << std::endl << std::endl;

	//unsigned char valid = bits.at(bits.size() - 1);

	//unsigned char previousBit = bits.at(10);
	//unsigned char newBit;
	//newBit ^= previousBit;
	//std::cout << newBit;
}

std::string NetworkHelper::GetBinaryFromCharacter(char c)
{
	std::bitset<8> temp(c);
	return temp.to_string();
}

char NetworkHelper::GetCharacterFromBinary(std::string group)
{
	std::bitset<8> temp(group);
	return temp.to_ulong();
}

std::vector<char> NetworkHelper::ConvertMessageToBits(std::string message)
{
	std::vector<char> bits;
	for (int i = 0; i < message.size(); i++)
	{
		std::string binary = GetBinaryFromCharacter(message[i]);
		for (int j = 0; j < binary.size(); j++)
			bits.push_back(binary[j]);
	}
	return bits;
}

std::vector<char> NetworkHelper::CreateParityBits(std::vector<char> bits)
{
	std::vector<char> parity(20);

	std::cout << "Creating parity bits for: ";
	for (int i = 0; i < bits.size(); i++)
		std::cout << bits[i];

	std::cout << std::endl;

	int count = 0;
	for (int i = 1; i <= parity.size() - 1, count < bits.size(); i++)
	{
		if (!IsPowerOfTwo(i))
		{
			parity.at(i) = bits[count];
			count++;
		}
		else
		{
			int val = CalculateParityValue(i, bits);
			parity.at(i) = '0' + val;
		}
	}

	std::cout << "Resulting parity binary: ";
	for (int i = 0; i < parity.size(); i++)
		std::cout << parity[i];

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

int NetworkHelper::CalculateParityValue(int whichBit, std::vector<char> bits)
{
	int parity = 0;
	for (int i = whichBit; i <= bits.size() - 1; i+=whichBit)
	{
		for (int j = 0; j < whichBit; j++)
		{
			parity += '0' + bits.at(i + j);
		}
	}

	if (IsEven(parity))
		return 1;
		
	return 0;
}
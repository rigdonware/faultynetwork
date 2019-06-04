#include "pch.h"
#include "NetworkHelper.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

NetworkHelper::NetworkHelper(unsigned short receivePort, unsigned short send)
{
	m_Ip = sf::IpAddress::LocalHost;
	m_Socket.setBlocking(false);
	if(m_Socket.bind(receivePort) != sf::Socket::Done)
		std::cout << "Failed to bind to port: " << receivePort << std::endl;

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
	sf::Packet packet;
	packet << message;

	if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
	{
		std::cout << "Failed to send packet" << std::endl;
		std::cout << "Error: " << m_Socket.Error << std::endl;
	}
}

void NetworkHelper::SendWriteRetryMessage(std::vector<unsigned char> bits)
{
	unsigned char code[1]; code[0] = 'W';
	unsigned char file[32] = "";

	std::string fileName = "";
	int count = 0;
	for (int i = 1; i < 33; i++)
	{
		file[count] = bits.at(i);
		count++;
		if (bits.at(i) != 0)
			fileName.push_back(bits.at(i));
	}
	unsigned int length = bits.at(37);

	if (length == 0)
	{
		VotingStructure* structure = NULL;

		if (fileName == "SmallTextFile.txt")
			structure = m_SmallText;
		else if (fileName == "LargeTextFile.txt")
			structure = m_LargeText;

		//structure->doVoting = true;
	}

	unsigned int location = 0;
	location += bits.at(33);
	location += (bits.at(34) * 256);
	location += (bits.at(35) * 512);
	location += (bits.at(36) * 1024);

	unsigned char data[10] = "";

	count = 0;
	for (int i = 38; i < 48; i++)
	{
		data[count] = bits.at(i);
		//std::cout << data[count];
		count++;
	}

	sf::Packet packet;
	packet.append(&code, 1);
	packet.append(&file, 32);
	packet.append(&location, 4);
	packet.append(&length, 1);
	packet.append(&data, 10);
	if (m_Socket.send(packet, m_Ip, sendPort) != sf::Socket::Done)
	{
		std::cout << "Failed to send packet" << std::endl;
		std::cout << "Error: " << m_Socket.Error << std::endl;
	}
}

void NetworkHelper::SendReadRetryMessage(std::vector<unsigned char> bits)
{
	unsigned int length = bits.at(37);

	//if (length == 0)
	//	return;

	unsigned char code[1]; code[0] = 'R';
	unsigned char file[32] = "";

	int count = 0;
	for (int i = 1; i < 33; i++)
	{
		file[count] = bits.at(i);
		count++;
	}
		
	unsigned int location = 0;
	location += bits.at(33);
	location += (bits.at(34) * 256);
	location += (bits.at(35) * 512);
	location += (bits.at(36) * 1024);

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

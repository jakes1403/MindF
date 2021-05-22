// Taken from ThrusterEngine
// 2019 - Jacob Allen

#include "TFile.h"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

namespace Thruster
{
	TFile LoadTFileFromLocalPath(string path)
	{
		TFile file;

		ifstream fileStream;
		fileStream.open(path, ios::binary | ios::in | ios::ate);

		if (fileStream.is_open())
		{
			file.Size = fileStream.tellg();
			file.Data = (unsigned char*)malloc(file.Size);
			fileStream.seekg(0, ios::beg);
			fileStream.read((char*)file.Data, file.Size);
			fileStream.close();
		}
		else
		{
			printf("Failure Loading File: %s\n", path.c_str());
			exit(-1);
		}

		return file;
	}


	void WriteTFileToLocalPath(TFile& file, string path)
	{
		ofstream ofileStream;
		ofileStream.open(path, ios::binary | ios::out);

		if (ofileStream.is_open()) {
			ofileStream.write((char*)file.Data, file.Size);
			ofileStream.close();
		}
		else
		{
			printf("Failure Writing File: %s\n", path.c_str());
			exit(-1);
		}
	}

	string TFileToString(TFile file)
	{
		string asString = "";
		for (int i = 0; i < file.Size; i++)
		{
			asString += file.Data[i];
		}
		return asString;
	}

	TFile StringToTFile(string data)
	{
		TFile file;
		file.Size = data.length();
		file.Data = (unsigned char*)malloc(file.Size);

		for (int i = 0; i < data.length(); i++)
		{
			file.Data[i] = data[i];
		}

		return file;
	}
	
}
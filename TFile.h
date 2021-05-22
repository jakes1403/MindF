// TFile.h
// Taken from ThrusterEngine
// 2019 - Jacob Allen

#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace Thruster
{
	class TFile {
	public:
		unsigned char* Data;
		size_t Size;
		inline void CreateBlank(size_t size)
		{
			Data = (unsigned char*)malloc(size);
			Size = size;
		}
		inline void CreateFromPointer(void* data, size_t size)
		{
			Data = (unsigned char*)data;
			Size = size;
		}
		inline unsigned char& operator[](size_t i)
		{
			return Data[i];
		}
		inline void Destroy()
		{
			free(Data);
		}
	};

	TFile LoadTFileFromLocalPath(std::string path);

	void WriteTFileToLocalPath(TFile& file, std::string path);

	std::string TFileToString(TFile file);

	TFile StringToTFile(std::string data);

}
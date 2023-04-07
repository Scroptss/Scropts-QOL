#pragma once
#include "includes.h"
//#include <algorithm>

namespace utils {

	template<typename... Parameters>
	FORCEINLINE std::string write_log(std::string format, Parameters... params)
	{
		char szBuffer[4096] = { NULL };
		sprintf_s(szBuffer, format.c_str(), params...);

		std::ofstream myfile;
		myfile.open(".\\QOL\\logs.txt""", std::ios_base::app);
		//myfile << "Exception: " << "\n";
		myfile << szBuffer << "\n";
		//myfile << "\n";
		myfile.close();
		return szBuffer;
	}

	FORCEINLINE std::string begin_exception() {
		std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
		time_t t = std::chrono::system_clock::to_time_t(p);
		char str[26];
		ctime_s(str, sizeof str, &t);
		std::ofstream myfile;
		myfile.open(".\\QOL\\logs.txt""", std::ios_base::app);
		myfile << "Exception: " << "\n";
		myfile << str;
		myfile.close();
		return "begun"s;
	}

	FORCEINLINE std::string end_exception() {
		std::ofstream myfile;
		myfile.open(".\\QOL\\logs.txt""", std::ios_base::app);
		myfile << "End Exception\n\n";
		myfile.close();
		return "ended"s;
	}

	static std::string StripMaterial(std::string text)
	{
		size_t iPosition;

		while ((iPosition = text.find("^")) != std::string::npos)
			text.erase(iPosition, 2);

		return text;
	}

	static std::string ToLower(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	static std::string FindAndReplaceString(std::string text, std::string find, std::string replace)
	{
		size_t iPosition;

		while ((iPosition = ToLower(text).find(ToLower(find))) != std::string::npos)
			text.replace(iPosition, find.length(), replace);

		return text;
	}

	static std::string StripClan(std::string text)
	{
		size_t iPosition = text.find("[");
		size_t iPosition2 = text.find("]");

		if (iPosition == 0 && iPosition2 <= 5) {
			text.erase(iPosition, iPosition2 + 1);
		}

		return text;
	}

}
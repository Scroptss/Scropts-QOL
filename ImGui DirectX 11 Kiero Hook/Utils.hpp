#pragma once
#include "includes.h"
#include <algorithm>

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
		return "begun";
	}

	FORCEINLINE std::string end_exception() {
		std::ofstream myfile;
		myfile.open(".\\QOL\\logs.txt""", std::ios_base::app);
		myfile << "End Exception\n\n";
		myfile.close();
		return "ended";
	}

	FORCEINLINE bool is_in_array(std::string cmp1, std::vector<std::string> cmp2)
	{
		for (auto& cmp : cmp2)
		{
			if (!strcmp(cmp1.data(), cmp.data()))
				return true;
		}
		return false;
	}

	FORCEINLINE bool is_in_number_array(int cmp1, std::vector<int> cmp2)
	{
		for (auto cmp : cmp2)
		{
			if (cmp1 == cmp)
				return true;
		}
		return false;
	}

	FORCEINLINE std::string to_lower(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), ::tolower);
		return text;
	}

	FORCEINLINE bool is_equal(const std::string& lhs, const std::string& rhs, const std::size_t count, const bool case_sensitive)
	{
		auto left = lhs;
		auto right = rhs;

		if (count != std::string::npos)
		{
			if (lhs.size() > count)
				left.erase(count);

			if (rhs.size() > count)
				right.erase(count);
		}

		if (case_sensitive)
		{
			return left == right;
		}

		else
		{
			return to_lower(left) == to_lower(right);
		}
	}

	FORCEINLINE bool is_address_within_range(std::uintptr_t address, std::uintptr_t min, std::uintptr_t max)
	{
		return (address >= min && address <= max);
	}

	FORCEINLINE std::vector<std::string> splitString(const char* input) {
		std::vector<std::string> result;
		if (input == nullptr) {
			return result;
		}

		std::istringstream stream(input);
		std::string word;
		while (stream >> word) {
			result.push_back(word);
		}

		return result;
	}

	FORCEINLINE std::vector<std::string> splitString(std::string input) {
		std::vector<std::string> result;
		if (input.empty()) {
			return result;
		}

		std::istringstream stream(input);
		std::string word;
		while (stream >> word) {
			result.push_back(word);
		}

		return result;
	}

	FORCEINLINE std::string decodeEncodedChars(const std::string& input) {
		std::stringstream output;

		for (size_t i = 0; i < input.size(); ++i) {
			if (input[i] == '%' && i + 4 <= input.size() && input[i + 1] == 'x') {
				std::string hexStr = input.substr(i + 2, 2);

				int value;
				std::stringstream ss;
				ss << std::hex << hexStr;
				ss >> value;

				output << static_cast<char>(value);

				i += 3;
			}
			else {
				output << input[i];
			}
		}

		return output.str();
	}

	FORCEINLINE bool containsIllegalChar(std::string text)
	{
		if (text.empty())
		{
			return true;
		}

		for (auto& c : text)
		{
			if (!::isalnum(c) && c != '$' && c != '/' && c != '_')
			{
				return true;
			}
		}

		return false;
	}

	FORCEINLINE bool isInvalidMaterial(std::string name)
	{
		materialName material;

		material.prefix = name.at(0);

		if (material.prefix == '^')
		{
			if (name.size() > 1)
			{
				material.type = name.at(1);

				if (material.type == 'H' || material.type == 'I')
				{
					if (name.size() > 5)
					{
						material.length = name.at(4);
						material.name = name.substr(5, material.length);

						if (material.name.size() < material.length)
						{
							return true;
						}

						else if (containsIllegalChar(material.name))
						{
							return true;
						}
					}

					else
					{
						return true;
					}
				}
			}
		}

		return false;
	}
	
	FORCEINLINE bool isInvalidButton(std::string name)
	{
		materialName material;

		material.prefix = name.at(0);

		if (material.prefix == '^')
		{
			if (name.size() > 1)
			{
				material.type = name.at(1);

				if (material.type == 'B')
				{
					if (name.size() > 2)
					{
						material.name = name.substr(2);
						material.length = material.name.find('^');

						if (material.length >= 64)
						{
							return true;
						}

						else if (material.length == std::string::npos)
						{
							return true;
						}
					}

					else
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	FORCEINLINE std::size_t findInvalidMaterials(std::string text)
	{
		std::size_t pos = 0u;

		while ((pos = text.find('^', pos)) != std::string::npos)
		{
			if (isInvalidMaterial(text.substr(pos)))
			{
				return pos;
			}

			if (isInvalidButton(text.substr(pos)))
			{
				return pos;
			}

			pos++;
		}

		return std::string::npos;
	}

}
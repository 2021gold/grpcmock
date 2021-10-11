// Utility.cpp : Defines the exported functions for the DLL.
//



#include "pch.h"
#include "framework.h"
#include "Utility.h"

#include <iostream>
#include <iomanip>
#include "atlstr.h"

std::string getDataIniFile(const LPCWSTR appName, const LPCWSTR keyName)
{
	// read ini file
	wchar_t address_wc[MAX_LENGTH];
	char address_c[MAX_LENGTH];
	std::string result;

	GetPrivateProfileString(
		appName,
		keyName,
		ERROR_MESSAGE,
		address_wc,
		MAX_LENGTH,
		PATH_TO_INI

	);
	size_t output_length;
	wcstombs_s(&output_length, address_c, MAX_LENGTH, address_wc, MAX_LENGTH);
	std::cout << "target : " << address_c << std::endl;

	for (int i = 0; i < sizeof(address_c); i++)
	{
		if (address_c[i] == NULL)
		{
			break;
		}

		result.push_back(address_c[i]);
	}
	return result;
}

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the UTILITY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// UTILITY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include <string>
#include <wchar.h>

#ifdef UTILITY_EXPORTS
#define UTILITY_API __declspec(dllexport)
#else
#define UTILITY_API __declspec(dllimport)
#endif



#define MAX_LENGTH 100
#define PATH_TO_INI L"..\\Data\\config.ini"
#define ERROR_MESSAGE L"fail while retrieving file"

UTILITY_API std::string getDataIniFile(const LPCWSTR appName, const LPCWSTR keyName);

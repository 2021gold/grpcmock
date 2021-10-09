// CommondataClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <string>
#include <tchar.h>
#include <wchar.h>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "../CommondataProto/ProtoOutput/cpp/test.grpc.pb.h"

#define MAX_BUFFER 255
#define MAX_LENGTH 100
#define PATH_TO_INI L"..\\Data\\config.ini"
#define ERROR_MESSAGE L"fail while retrieving file"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using commondata::CommondataService;
using ::google::protobuf::Empty;

enum SYSTEMINFO
{
	NONE = 0,
	VERSION ,
	MODEL ,
	REGION ,
	MAX
};

class CommonClient
{
public:
	CommonClient(std::shared_ptr<Channel> channel) : stub_(CommondataService::NewStub(channel)) {}
	std::string GetSystemInfo(SYSTEMINFO eSystem)
	{	
		ClientContext context;
		const ::google::protobuf::Empty request;
		Status status = stub_->GetSystemInfo(&context, request, &systeminfo);

		if (status.ok())
		{
			std::string l_result;
			switch (eSystem)
			{

			case VERSION:
				l_result = systeminfo.version();
				break;
			case MODEL:
				l_result = systeminfo.model();
				break;
			case REGION:
				l_result = systeminfo.region();
				break;

			default:
				l_result = "eSystem invalid";
				break;
			}

			return l_result;
		}
		std::cout << status.error_code() << ": " << status.error_message() << std::endl;

		return "RPC failed";

	}
	bool SetSystemInfo(const std::string model, const std::string version, const std::string region)
	{
		ClientContext context;
		systeminfo.set_model(model);
		systeminfo.set_version(version);
		systeminfo.set_region(region);

		Status status = stub_->SetSystemInfo(&context, systeminfo, &request);
		
		if (status.ok())
		{
			std::cout << "system set success." << std::endl;
			return true;
		}
		else
		{
			std::cout << "system set failed." << std::endl;
			return false;
		}
	}

private:
	std::unique_ptr<CommondataService::Stub> stub_;
	commondata::SystemInfo systeminfo;
	::google::protobuf::Empty request;

};

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
	wcstombs(address_c, address_wc, MAX_LENGTH);
	std::cout << "ini : " << address_c << std::endl;

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

void DisplayMenu(CommonClient &client_system)
{
	{
		using namespace std;
		char menu_option;
		string a;
		string l_model, l_version, l_region;
		// Nhap tu ban phim
		do
		{
			system("cls");
			cout << "\n\n\n\tMenu";
			cout << "\n\n\t1. SystemInfo Display";
			cout << "\n\n\t2. Set SystemInfo";
			cout << "\n\n\t3. Exit";
			// check menu option phai tu 1 den 8
			do
			{
				cout << "\n\n\tPlease choose menu (so tu 1 den 3) ";
				cin >> menu_option;
			} while (menu_option < '1' || menu_option > '3');

			system("cls");
			switch (menu_option)
			{
			case '1':
				//SystemInfo Display
				cout << "Display SystemInfo\n" << endl;
				std::cout << "MODEL   : " << client_system.GetSystemInfo(SYSTEMINFO::MODEL) << std::endl;
				std::cout << "VERSION : " << client_system.GetSystemInfo(SYSTEMINFO::VERSION) << std::endl;
				std::cout << "REGION  : " << client_system.GetSystemInfo(SYSTEMINFO::REGION) << std::endl;
				cout << "\n\nPress Enter to back..." << endl;
				break;
			case '2':
				cout << "Set SystemInfo\n";
				cout << "\nEnter Model : ";
				cin >> l_model;
				cout << "\nEnter Version : ";
				cin >> l_version;
				cout << "\nEnter Region : ";
				cin >> l_region;
				client_system.SetSystemInfo(l_model, l_version, l_region);
				cout << "\n\nPress Enter to back..." << endl;
				break;
			
			case '3':
				menu_option = '3';
				cout << "\n\n\tThank You." << endl;
				break;
			default:cout << "\a";
			}
			cin.ignore();
			cin.get();
		} while (menu_option != '3');
	}
}

int main()
{
	std::cout << "Hello client!\n";
	
	/**** Read address port ****/
	std::string target_str = getDataIniFile(L"config", L"address");
	std::string port_str = getDataIniFile(L"config", L"port");
	//target_str = "localhost:50051";
	target_str = target_str.append(":").append(port_str);
	std::cout << "Client connected target : " << target_str << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "\nStart" << std::endl;
	for (int i = 0; i < 30; i++)
	{
		Sleep(100);
		std::cout << ".";
	}
	

	/**** Create Client *******/
	CommonClient client_system(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
	
	/**** Display Menu *******/
	

	DisplayMenu(client_system);
	
	
	//
	


}

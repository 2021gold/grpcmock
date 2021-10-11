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

#include "../Utility/Utility.h"



using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using commondata::CommondataService;
using ::google::protobuf::Empty;


#define MENU_EXIT				'0'
#define MENU_SYSTEMINFO			'1'
#define MENU_DISPLAYSETTING		'2'
#define MENU_SET_DISPLAYSETTING '3'

enum SYSTEMINFO
{
	NONE = 0,
	VERSION ,
	MODEL ,
	REGION ,
	MAX
};

enum DISPLAYSETTING
{
	THEME ,
	FONTSIZE,
	LANGUAGE
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


	std::string GetDisplaySetting(DISPLAYSETTING eSetting)
	{
		ClientContext context;
		const ::google::protobuf::Empty request;
		Status status = stub_->GetDisplaySetting(&context, request, &displaysetting);

		if (status.ok())
		{
			std::string l_result;
			switch (eSetting)
			{

			case THEME:
				l_result = displaysetting.theme();
				break;
			case FONTSIZE:
				l_result = displaysetting.fontsize();
				break;
			case LANGUAGE:
				l_result = displaysetting.language();
				break;

			default:
				l_result = "eSetting invalid";
				break;
			}

			return l_result;
		}
		std::cout << status.error_code() << ": " << status.error_message() << std::endl;

		return "RPC failed";
	}

	bool SetDisplaySetting(const std::string theme, const std::string fontsize, const std::string language)
	{
		ClientContext context;
		displaysetting.set_theme(theme);
		displaysetting.set_fontsize(fontsize);
		displaysetting.set_language(language);

		Status status = stub_->SetDisplaySetting(&context, displaysetting, &request);
		if (status.ok())
		{
			std::cout << "Display setting set success." << std::endl;
			return true;
		}
		else
		{
			std::cout << "Display setting set failed." << std::endl;
			return false;
		}
	}


private:
	std::unique_ptr<CommondataService::Stub> stub_;
	commondata::SystemInfo systeminfo;
	commondata::DisplaySetting displaysetting;
	::google::protobuf::Empty request;

};


void DisplayMenu(CommonClient &client_system)
{
	{
		using namespace std;
		char menu_option;
		string a;
		string l_theme, l_fontsize, l_language;
		// Nhap tu ban phim
		do
		{
			system("cls");
			cout << "\n\n\n\tMenu";
			cout << "\n\n\t1. SystemInfo";
			cout << "\n\n\t2. Display Setting";
			cout << "\n\n\t3. Set Display Setting";
			cout << "\n\n\t0. Exit";
			// check menu option phai tu 1 den 8
			do
			{
				cout << "\n\n\tPlease choose menu (so tu 0 den 3) ";
				cin >> menu_option;
			} while (menu_option < '0' || menu_option > '3');

			system("cls");
			switch (menu_option)
			{
			case MENU_SYSTEMINFO:
				//SystemInfo Display
				cout << "SystemInfo\n" << endl;
				std::cout << "MODEL   : " << client_system.GetSystemInfo(SYSTEMINFO::MODEL) << std::endl;
				std::cout << "VERSION : " << client_system.GetSystemInfo(SYSTEMINFO::VERSION) << std::endl;
				std::cout << "REGION  : " << client_system.GetSystemInfo(SYSTEMINFO::REGION) << std::endl;
				cout << "\n\nPress Enter to back..." << endl;
				break;

			case MENU_DISPLAYSETTING:
				cout << "Display Setting\n" << endl;
				std::cout << "THEME    : " << client_system.GetDisplaySetting(DISPLAYSETTING::THEME) << std::endl;
				std::cout << "FONTSIZE : " << client_system.GetDisplaySetting(DISPLAYSETTING::FONTSIZE) << std::endl;
				std::cout << "LANGUAGE : " << client_system.GetDisplaySetting(DISPLAYSETTING::LANGUAGE) << std::endl;
				cout << "\n\nPress Enter to back..." << endl;
				break;

			case MENU_SET_DISPLAYSETTING:
				cout << "Set Display Setting\n";
				cout << "\nEnter Theme : ";
				cin >> l_theme;
				cout << "\nEnter Fontsize : ";
				cin >> l_fontsize;
				cout << "\nEnter Language : ";
				cin >> l_language;
				client_system.SetDisplaySetting(l_theme, l_fontsize, l_language);
				cout << "\n\nPress Enter to back..." << endl;
				break;
			case '0':
				menu_option = '0';
				cout << "\n\n\tThank You." << endl;
				break;
			default:cout << "\a";
			}
			cin.ignore();
			cin.get();
		} while (menu_option != '0');
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

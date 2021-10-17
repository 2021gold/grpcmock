// CommondataClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <string>
#include <tchar.h>
#include <wchar.h>
#include <iomanip>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "../CommondataProto/ProtoOutput/cpp/data.grpc.pb.h"

#include "../Utility/Utility.h"



using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using commondata::CommondataService;
using ::google::protobuf::Empty;
using grpc::ClientReader;
using commondata::UserProfile;
using grpc::ClientWriter;

#define MENU_EXIT				'0'
#define MENU_SYSTEMINFO			'1'
#define MENU_DISPLAYSETTING		'2'
#define MENU_SET_DISPLAYSETTING '3'

enum SYSTEMINFO
{
	NONE = 0,
	VERSION,
	MODEL,
	REGION,
	SERIAL,
	TYPE,
	SCREEN,
	MAX
};

enum DISPLAYSETTING
{
	THEME,
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
			case SERIAL:
				l_result = std::to_string(systeminfo.serial());
				break;
			case TYPE:
				l_result = systeminfo.type();
				break;
			case SCREEN:
				l_result = std::to_string(systeminfo.screen());
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
				l_result = std::to_string(displaysetting.fontsize());
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

	bool SetDisplaySetting(const std::string theme, const int fontsize, const std::string language)
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

	void SetUserProfile(const std::string user, const int id, const int fontsize, const std::string language, const std::string theme)
	{
		ClientContext context;
		
	}
	void GetUserProfile(int id)
	{
		ClientContext context;
		const ::google::protobuf::Empty request;

		std::unique_ptr<ClientReader<UserProfile> > reader(
			stub_->GetUserProfile(&context, request));
		while (reader->Read(&userprofile)) {
			if (id == userprofile.id())
			{
				std::cout << std::setw(20) << "ID:\t" << userprofile.id() << std::endl;
				std::cout << std::setw(20) << "USERNAME:\t" << userprofile.username() << std::endl;
				std::cout << std::setw(20) << "FONTSIZE:\t" << userprofile.mutable_disp()->fontsize() << std::endl;
				std::cout << std::setw(20) << "LANGUAGE:\t" << userprofile.mutable_disp()->language() << std::endl;
				std::cout << std::setw(20) << "THEME:\t" << userprofile.mutable_disp()->theme() << std::endl;
				std::cout << std::setw(20) << "VOLUME:\t" << userprofile.mutable_disp()->volume() << std::endl;
				std::cout << std::setw(20) << "BRIGHTNESS:\t" << userprofile.mutable_disp()->brightness() << std::endl;
				std::cout << std::setw(20) << "KEYBOARD:\t" << userprofile.mutable_disp()->keyboard() << std::endl;
				std::cout << std::setw(20) << "CAMERA:\t" << userprofile.mutable_disp()->camera() << std::endl;
				std::cout << std::setw(20) << "NOTIFICATION:\t" << userprofile.mutable_disp()->notification() << std::endl;

			}
		}
		Status status = reader->Finish();
		if (status.ok()) {
			std::cout << "list userprofile rpc succeeded." << std::endl;
		}
		else {
			std::cout << "list userprofile rpc failed." << std::endl;
		}
	}


private:
	std::unique_ptr<CommondataService::Stub> stub_;
	commondata::SystemInfo systeminfo;
	commondata::DisplaySetting displaysetting;
	::google::protobuf::Empty request;
	UserProfile userprofile;
};


void DisplayMenu(CommonClient &client_system)
{
	{
		using namespace std;
		char menu_option;
		string a;
		string l_theme, l_language, l_user;
		int l_fontsize, l_id;
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
				cout << "\n-----------------------------------------------------------------------------\n" << endl;
				cout << "\t\t\t\tSystem Infomation\n" << endl;
				cout << "-----------------------------------------------------------------------------\n" << endl;

				std::cout << "\t\tMODEL   :\t " << client_system.GetSystemInfo(SYSTEMINFO::MODEL) << std::endl;
				cout << "-----------------------------------------------------------------------------\n" << endl;

				std::cout << "\t\tVERSION :\t " << client_system.GetSystemInfo(SYSTEMINFO::VERSION) << std::endl;
				cout << "-----------------------------------------------------------------------------\n" << endl;

				std::cout << "\t\tREGION  :\t " << client_system.GetSystemInfo(SYSTEMINFO::REGION) << std::endl;
				cout << "-----------------------------------------------------------------------------\n" << endl;

				std::cout << "\t\tSERIAL  :\t " << client_system.GetSystemInfo(SYSTEMINFO::SERIAL) << std::endl;
				cout << "-----------------------------------------------------------------------------\n" << endl;

				std::cout << "\t\tTYPE	:\t " << client_system.GetSystemInfo(SYSTEMINFO::TYPE) << std::endl;
				cout << "-----------------------------------------------------------------------------\n" << endl;

				std::cout << "\t\tSCREEN  :\t " << client_system.GetSystemInfo(SYSTEMINFO::SCREEN) << std::endl;
				cout << "\-----------------------------------------------------------------------------\n" << endl;

				cout << "\n\nPress Enter to back..." << endl;
				break;

			case MENU_DISPLAYSETTING:
				cout << "Display Setting\n" << endl;
				//std::cout << "THEME    : " << client_system.GetDisplaySetting(DISPLAYSETTING::THEME) << std::endl;
				//std::cout << "FONTSIZE : " << client_system.GetDisplaySetting(DISPLAYSETTING::FONTSIZE) << std::endl;
				//std::cout << "LANGUAGE : " << client_system.GetDisplaySetting(DISPLAYSETTING::LANGUAGE) << std::endl;
				std::cout << "Enter id user :";
				int iduser;
				std:cin >> iduser;
				 client_system.GetUserProfile(iduser);
				cout << "\n\nPress Enter to back..." << endl;
				break;

			case MENU_SET_DISPLAYSETTING:
				cout << "Set Display Setting\n";
				cout << "\nEnter User : ";
				cin >> l_user;
				cout << "\nEnter Id : ";
				cin >> l_id;
				cout << "\nEnter Language : ";
				cin >> l_language;
				cout << "\nEnter Theme : ";
				cin >> l_theme;
				cout << "\nEnter Fontsize : ";
				cin >> l_fontsize;
				/*cout << "\nEnter Volume : ";
				cin >> l_language;
				cout << "\nEnter Brightness : ";
				cin >> l_theme;
				cout << "\nEnter Keyboard : ";
				cin >> l_fontsize;
				cout << "\nEnter Camera : ";
				cin >> l_language;
				cout << "\nEnter Notification : ";
				cin >> l_language;*/
				client_system.SetUserProfile(l_user, l_id, l_fontsize, l_language, l_theme);
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

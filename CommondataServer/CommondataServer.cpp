// CommondataServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <string>
#include <tchar.h>
#include <wchar.h>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <thread>
#include <sstream>
#include <grpcpp/grpcpp.h>
#include "../../json/single_include/nlohmann/json.hpp"
#include <grpcpp/health_check_service_interface.h>
#include "../CommondataProto/ProtoOutput/cpp/data.grpc.pb.h"

#include "../Utility/Utility.h"
#include <stdint.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using commondata::CommondataService;
using commondata::UserProfile;
using commondata::DisplaySetting;

using nlohmann::json;
using namespace std;

#define PATH_TO_SYSTEMINFO		L"..\\Data\\systeminfo.json"
#define PATH_TO_DISPLAYSETTING	L"..\\Data\\setting.json"
#define PATH_TO_DISPLAYSETTING2	L"..\\Data\\setting2.json"

int idx = 0;
std::mutex mtx;

nlohmann::json readJsonFile(const wchar_t *path)
{
	// read a JSON file
	std::ifstream i(path);
	nlohmann::json js = json::parse(i);

	return js;
}
std::string readJsonFile(const string &key, const wchar_t *path)
{
	// read a JSON file
	std::ifstream i(path);
	nlohmann::json js = json::parse(i);
	cout << std::string(js[key]) << endl;

	return std::string(js[key]);
}
void writeJsonFile(nlohmann::json &j, const wchar_t *path)
{
	// write prettified JSON to another file
	mtx.lock();
	std::cout << "writeJsonFile " << ++idx << std::endl;
	std::ofstream o(path);
	o << std::setw(4) << j << std::endl;
	mtx.unlock();
}
struct s_setting
{
	int id;
	string username;
	struct {
		int fontsize;
		string language;
		string theme;
		int volume;
		int brightness;
		string keyboard;
		string camera;
		string notification;
	}displaysetting;
};

class CommonDataServiceImpl final : public CommondataService::Service {

public:
	CommonDataServiceImpl() {

		// system infomation
		ReadSystemInfo();

		//
		m_theme = readJsonFile("theme", PATH_TO_DISPLAYSETTING);
		m_fontsize = readJsonFile(PATH_TO_DISPLAYSETTING)["fontsize"];
		m_language = readJsonFile("language", PATH_TO_DISPLAYSETTING);
		// userprofile setting
		ReadDataUserSetting();
	}

	void ReadSystemInfo()
	{
		m_serial = readJsonFile(PATH_TO_SYSTEMINFO)["serial"];
		m_screen = readJsonFile(PATH_TO_SYSTEMINFO)["screen"];
		m_model = readJsonFile("model", PATH_TO_SYSTEMINFO);
		m_version = readJsonFile("version", PATH_TO_SYSTEMINFO);
		m_region = readJsonFile("region", PATH_TO_SYSTEMINFO);
		m_type = readJsonFile("type", PATH_TO_SYSTEMINFO);
	}

	void ReadDataUserSetting()
	{
		std::ifstream db_file(PATH_TO_DISPLAYSETTING2);
		if (!db_file.is_open()) {
			std::cout << "Failed to open " << PATH_TO_DISPLAYSETTING2 << std::endl;
		}
		std::stringstream db;
		db << db_file.rdbuf();
		string path = db.str();
		path.erase(remove_if(path.begin(), path.end(), isspace), path.end());
		cout << path << endl;

		nlohmann::json js = json::parse(path);
		for (json obj : js)
		{
			struct_setting.id = static_cast<int>(obj["id"]);
			userprofile.set_id(struct_setting.id);

			struct_setting.username = static_cast<string>(obj["username"]);
			userprofile.set_username(struct_setting.username);
			struct_setting.displaysetting.fontsize = static_cast<int>(obj["settingdisplay"]["fontsize"]);
			userprofile.mutable_disp()->set_fontsize(struct_setting.displaysetting.fontsize);

			struct_setting.displaysetting.language = static_cast<string>(obj["settingdisplay"]["language"]);
			userprofile.mutable_disp()->set_language(struct_setting.displaysetting.language);

			struct_setting.displaysetting.theme = static_cast<string>(obj["settingdisplay"]["theme"]);
			userprofile.mutable_disp()->set_theme(struct_setting.displaysetting.theme);

			struct_setting.displaysetting.volume = static_cast<int>(obj["settingdisplay"]["volume"]);
			userprofile.mutable_disp()->set_volume(struct_setting.displaysetting.volume);

			struct_setting.displaysetting.brightness = static_cast<int>(obj["settingdisplay"]["brightness"]);
			userprofile.mutable_disp()->set_brightness(struct_setting.displaysetting.brightness);

			struct_setting.displaysetting.keyboard = static_cast<string>(obj["settingdisplay"]["keyboard"]);
			userprofile.mutable_disp()->set_keyboard(struct_setting.displaysetting.keyboard);

			struct_setting.displaysetting.camera = static_cast<string>(obj["settingdisplay"]["camera"]);
			userprofile.mutable_disp()->set_camera(struct_setting.displaysetting.camera);

			struct_setting.displaysetting.notification = static_cast<string>(obj["settingdisplay"]["notification"]);
			userprofile.mutable_disp()->set_notification(struct_setting.displaysetting.notification);

			setting_vector.push_back(struct_setting);
			userprofile_list.push_back(userprofile);
		}

	}
	
	void DisplayAllDataUser()
	{
		for (int i = 0; i < setting_vector.size(); i++)
		{
			cout << "id : " << setting_vector[i].id << endl;
			cout << "username : " << setting_vector[i].username << endl;
			cout << "fontsize : " << setting_vector[i].displaysetting.fontsize << endl;
			cout << "language : " << setting_vector[i].displaysetting.language << endl;
			cout << "theme : " << setting_vector[i].displaysetting.theme << endl;
			cout << "volume : " << setting_vector[i].displaysetting.volume << endl;
			cout << "brightness : " << setting_vector[i].displaysetting.brightness << endl;
			cout << "keyboard : " << setting_vector[i].displaysetting.keyboard << endl;
			cout << "camera : " << setting_vector[i].displaysetting.camera << endl;
			cout << "notification : " << setting_vector[i].displaysetting.notification << endl;
		}
	}

private:
	::grpc::Status GetSystemInfo(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::commondata::SystemInfo* response) override
	{

		response->set_model(m_model);
		response->set_region(m_region);
		response->set_version(m_version);
		response->set_serial(m_serial);
		response->set_type(m_type);
		response->set_screen(m_screen);

		return Status::OK;
	}
	::grpc::Status GetDisplaySetting(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::commondata::DisplaySetting* response) override
	{
		response->set_theme(m_theme);
		response->set_fontsize(m_fontsize);
		response->set_language(m_language);
		return Status::OK;
	}
	::grpc::Status SetDisplaySetting(::grpc::ServerContext* context, const ::commondata::DisplaySetting* request, ::google::protobuf::Empty* response) override
	{
		m_theme = request->theme();
		m_fontsize = request->fontsize();
		m_language = request->language();

		std::ifstream i(PATH_TO_DISPLAYSETTING);
		nlohmann::json j = nlohmann::json::parse(i);
		j["theme"] = m_theme;
		j["fontsize"] = m_fontsize;
		j["language"] = m_language;

		writeJsonFile(j, PATH_TO_DISPLAYSETTING);
		return Status::OK;
	}
	::grpc::Status GetUserProfile(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter< ::commondata::UserProfile>* writer) override
	{
		for (const UserProfile &f : userprofile_list)
		{
			writer->Write(f);
		}
		return Status::OK;
	}


private:

	string m_model;
	string m_version;
	string m_region;
	int m_serial;
	string m_type;
	int m_screen;

	std::string m_theme;
	int m_fontsize;
	std::string m_language;
	s_setting struct_setting;
	vector<s_setting> setting_vector;
	vector<UserProfile> userprofile_list;
	UserProfile userprofile;

};

void RunServer(const std::string &server_address) {

	CommonDataServiceImpl service;
	service.DisplayAllDataUser();
	grpc::EnableDefaultHealthCheckService(true);
	//grpc::reflection::InitProtoReflectionServerBuilderPlugin();
	ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}


int main()
{
	std::cout << "Hello  Server!\n";

	/**** Read address port ****/

	std::string server_address = getDataIniFile(L"config", L"address");
	std::string port_str = getDataIniFile(L"config", L"port");
	//target_str = "localhost:50051";
	server_address = server_address.append(":").append(port_str);
	std::cout << "serveraddress : " << server_address << std::endl;

	/**** Run server ****/

	RunServer(server_address);
}
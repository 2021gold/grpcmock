// CommondataServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <string>
#include <tchar.h>
#include <wchar.h>
#include <fstream>
#include <iomanip>


#include <grpcpp/grpcpp.h>
#include "../../json/single_include/nlohmann/json.hpp"
#include <grpcpp/health_check_service_interface.h>
#include "../CommondataProto/ProtoOutput/cpp/test.grpc.pb.h"

#include "../Utility/Utility.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using commondata::CommondataService;
using nlohmann::json;


#define PATH_TO_JSON L"..\\Data\\data.json"


std::string readJsonFile(const std::string &key)
{
	// read a JSON file
	std::ifstream i(PATH_TO_JSON);
	nlohmann::json js = json::parse(i);
	return std::string(js[key]);
}

void writeJsonFile(nlohmann::json &j)
{
	// write prettified JSON to another file
	std::ofstream o(L"..\\Data\\data.json");
	o << std::setw(4) << j << std::endl;
}

class CommonDataServiceImpl final : public CommondataService::Service {
	
public :
	CommonDataServiceImpl() {

		m_model = readJsonFile("model");
		m_version = readJsonFile("version");
		m_region = readJsonFile("region");
	
	}

private :
	::grpc::Status GetSystemInfo(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::commondata::SystemInfo* response) override
	{
		
		response->set_model(m_model);
		response->set_version(m_version);
		response->set_region(m_region);

		return Status::OK;
	}
	::grpc::Status SetSystemInfo(::grpc::ServerContext* context, const ::commondata::SystemInfo* request, ::google::protobuf::Empty* response) override
	{
		m_model = request->model();
		m_version = request->version();
		m_region = request->region();
		std::ifstream i(PATH_TO_JSON);
		nlohmann::json js = json::parse(i);
		js["model"] = m_model;
		js["version"] = m_version;
		js["region"] = m_region;

		writeJsonFile(js);
		return Status::OK;

	}

	::grpc::Status GetDisplaySetting(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::commondata::DisplaySetting* response) override
	{
		return Status::OK;
	}
	::grpc::Status TestDisplaySetting(::grpc::ServerContext* context, const ::commondata::DisplaySetting* request, ::google::protobuf::Empty* response) override
	{
		return Status::OK;
	}
	::grpc::Status GetSystemInfoDisplay(::grpc::ServerContext* context, const ::commondata::DisplaySetting* request, ::commondata::SystemInfo* response) override
	{

		response->set_model(m_model);
		response->set_version(m_version);
		response->set_region(m_region);

		return Status::OK;
	}

private :

	std::string m_model;
	std::string m_version;
	std::string m_region;
};

void RunServer(const std::string &server_address) {
	
	CommonDataServiceImpl service;

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
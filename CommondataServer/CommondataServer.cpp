// CommondataServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <string>
#include <tchar.h>
#include <wchar.h>

#include <grpcpp/grpcpp.h>

#include <grpcpp/health_check_service_interface.h>

#include "../CommondataProto/ProtoOutput/cpp/test.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

#define MAX_BUFFER 255

std::string getDataIniFile(LPCWSTR appName, LPCWSTR keyName)
{
	// read ini file
	wchar_t address_wc[100];
	char address_c[100];
	std::string result;

	GetPrivateProfileString(
		appName,
		keyName,
		L"fail while retrieving file",
		address_wc,
		100,
		L"..\\Data\\config.ini"

	);
	wcstombs(address_c, address_wc, MAX_BUFFER);
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



class GreeterServiceImpl final : public Greeter::Service {
	Status SayHello(ServerContext* context, const HelloRequest* request,
		HelloReply* reply) override {
		std::string prefix("Hello ");
		reply->set_message(prefix + request->name());
		return Status::OK;
	}
};

void RunServer(const std::string &server_address) {
	
	GreeterServiceImpl service;

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
	std::cout << "Hello World Server!\n";
	std::string address_str = getDataIniFile(L"config", L"address");
	std::string port_str = getDataIniFile(L"config", L"port");
	//std::string server_address("0.0.0.0:50051");

	std::string server_address;
	//target_str = "localhost:50051";
	server_address = address_str.append(":").append(port_str);
	std::cout << "serveraddress : " << server_address << std::endl;

	RunServer(server_address);
}
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


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

#define MAX_BUFFER 255


class GreeterClient {
public:
	GreeterClient(std::shared_ptr<Channel> channel)
		: stub_(Greeter::NewStub(channel)) {}

	// Assembles the client's payload, sends it and presents the response back
	// from the server.
	std::string SayHello(const std::string& user) {
		// Data we are sending to the server.
		HelloRequest request;
		request.set_name(user);

		// Container for the data we expect from the server.
		HelloReply reply;

		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		ClientContext context;

		// The actual RPC.
		Status status = stub_->SayHello(&context, request, &reply);

		// Act upon its status.
		if (status.ok()) {
			return reply.message();
		}
		else {
			std::cout << status.error_code() << ": " << status.error_message()
				<< std::endl;
			return "RPC failed";
		}
	}

private:
	std::unique_ptr<Greeter::Stub> stub_;
};

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

int main()
{
	std::cout << "Hello World client!\n";
	
	//
	std::string address_str = getDataIniFile(L"config", L"address");
	std::string port_str = getDataIniFile(L"config", L"port");

	std::string target_str;
	//target_str = "localhost:50051";
	target_str = address_str.append(":").append(port_str);
	std::cout << "target : " << target_str << std::endl;
	GreeterClient greeter(
		grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
	std::string user("world");
	std::string reply = greeter.SayHello(user);
	std::cout << "Greeter received: " << reply << std::endl;
	//
	std::string myName;
	std::cout << "Enter your name : ";
	std::cin >> myName;
	std::string reply1 = greeter.SayHello(myName);
	std::cout << "Greeter received: " << reply1 << std::endl;


}

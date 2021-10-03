// CommondataClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "../CommondataProto/ProtoOutput/cpp/test.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;

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


int main()
{
	std::cout << "Hello World client!\n";
	std::string target_str;
	target_str = "localhost:50051";

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

set OurDir=%1
set ProjectDir=%CD%\
set GRPCLibDir="%ProjectDir%..\..\grpc_lib\bin\"
set ProtoDir="%ProjectDir%\Definition"
set ProtoCppDir="%ProjectDir%\ProtoOutput\cpp"
set ProtoCsharpDir="%ProjectDir%\ProtoOutput\csharp"
set GrpcCppPlugin="%ProjectDir%..\..\grpc_lib\bin\grpc_cpp_plugin.exe"
set GrpcCsharpPlugin="%ProjectDir%..\..\grpc_lib\bin\grpc_csharp_plugin.exe"

 

if not exist %OurDir% mkdir %OurDir%

 

xcopy /R /C /Y %GRPCLibDir%*.dll %OurDir%

 

%GRPCLibDir%protoc.exe --proto_path=%ProtoDir% --cpp_out=%ProtoCppDir% --csharp_out=%ProtoCsharpDir% test.proto

%GRPCLibDir%protoc.exe --proto_path=%ProtoDir% --grpc_out=%ProtoCppDir% --plugin=protoc-gen-grpc=%GrpcCppPlugin% test.proto 

%GRPCLibDir%protoc.exe --proto_path=%ProtoDir% --grpc_out=%ProtoCsharpDir% --plugin=protoc-gen-grpc=%GrpcCsharpPlugin% test.proto

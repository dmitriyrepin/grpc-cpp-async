# Introduction
The code is an oversimplified example of how to 
1. Create C++ gRPC async server with multiple endpoints
2. How to asynchronously call another gRPC service from the server endpoint code

# Build the example
First, built the gRPC library and the example in a container:
```Bash
docker build -f grpc-ubuntu2004.Dockerfile --no-cache -t drepin/grpc-cpp:grpc-ubuntu2004 .
docker build -f grpc-cpp-async-server.Dockerfile --no-cache -t drepin/grpc-cpp:grpc-cpp-async-server .
```

Start an interactive bash session in the container
```Bash
docker run -it --rm drepin/grpc-cpp:grpc-cpp-async-server
```

Run the pinger server and clients:
```Bash
# In bash shell 1 run pinger service:
/opt/grpc-cpp-async/bin/Release/pinger_server.exe &
# In bash shell 2 connect to it with a block client:
/opt/grpc-cpp-async/bin/Release/pinger_client.exe
```
```Bash
# In bash shell 1 run pinger service:
/opt/grpc-cpp-async/bin/Release/pinger_server.exe &
# In bash shell 2 connect to it with an async client:
/opt/grpc-cpp-async/bin/Release/pinger_client_async.exe
```

Run the talker server (that calls pinger) and client tests:
```Bash
# In bash shell 1 run pinger service:
/opt/grpc-cpp-async/bin/Release/pinger_server.exe &
# In bash shell 2 run talker service:
/opt/grpc-cpp-async/bin/Release/talker_async_server.exe &
# In bash shell 3 connect to it with an async client:
/opt/grpc-cpp-async/bin/Release/talker_client.exe
```
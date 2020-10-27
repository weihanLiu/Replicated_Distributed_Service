#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "ServerSocket.h"
#include "ServerThread.h"

int main(int argc, char *argv[]) {
	int port;
	int engineer_cnt = 0;
	int peer_count;
	int id;
	std::vector<ServerNode> peers;

	ServerSocket socket;
	RobotFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;

//	if (argc < 3) {
//		std::cout << "not enough arguments" << std::endl;
//		std::cout << argv[0] << "[port #] [# experts]" << std::endl;
//		return 0;
//	}


//	port = atoi("32000");
//	id = 0;
//	peer_count = 1;
//    peers.reserve(peer_count);
//    for(int i = 0; i < peer_count; i++) {
//        peers.push_back({1,"127.0.0.1", 12345});
//    }


    port = atoi(argv[1]);
    id = atoi(argv[2]);
    peer_count = atoi(argv[3]);
    peers.reserve(peer_count);
    for(int i = 0; i < peer_count; i++) {
        peers.push_back({atoi(argv[4 + i * 3]),argv[5 + i * 3], atoi(argv[6 + i * 3])});
    }
    factory.Init(id, peers);

    std::thread admin_thread(&RobotFactory::AdminThread, &factory, engineer_cnt++);
    thread_vector.push_back(std::move(admin_thread));

	if (!socket.Init(port)) {
		std::cout << "Socket initialization failed" << std::endl;
		return 0;
	}
    std::cout << "Main Socket created." << std::endl;

	while ((new_socket = socket.Accept())) {
		std::thread engineer_thread(&RobotFactory::EngineerThread, &factory, 
				std::move(new_socket), engineer_cnt++);
		thread_vector.push_back(std::move(engineer_thread));
	}
	return 0;
}

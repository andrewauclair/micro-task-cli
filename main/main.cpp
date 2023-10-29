#include "packets.hpp"

#include <iostream>
#include <cstdlib>

#include <sockpp/tcp_acceptor.h>

/*
* micro-task-cli.exe 127.0.0.1 5000
*/
int main(int argc, char** argv)
{
	if (argc < 3) return -1;

	sockpp::initialize();

	const std::string ip_address = argv[1];

	const int port = std::atoi(argv[2]);

	std::cout << ip_address << " " << port << '\n';

	auto acceptor = sockpp::tcp_acceptor(sockpp::inet_address(ip_address, port));

	auto connection = acceptor.accept();

	auto socket = std::make_unique<sockpp::tcp_socket>(std::move(connection));
	
	std::cout << "connected\n";

	mt::read_next_packet(*socket);
}

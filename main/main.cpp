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

	struct task {
		std::int32_t id;
		std::string name;
		std::chrono::system_clock::time_point add_time;
		std::optional<std::chrono::system_clock::time_point> start_time;
	};

	std::unordered_map<std::int32_t, task> tasks;
	std::int32_t next_id = 1;

	try {
		while (auto opt_json = mt::read_next_packet(*socket))
		{
			auto json = opt_json.value();

			std::int32_t command;
			json.at("command").get_to(command);

			if (command == 1) {
				// version request
				auto response = nlohmann::json();
				response["command"] = 1;
				response["version"] = "2023-10-30";

				mt::send_packet(*socket, response);
			}
			else if (json["command"].get<int>() == 2) {
				// add task
				std::string name = json["name"].get<std::string>();

				task new_task;
				new_task.id = next_id++;
				new_task.name = name;
				new_task.add_time = std::chrono::system_clock::now();// std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());

				tasks.emplace(new_task.id, new_task);
			}
			else if (json["command"].get<int>() == 3) {
				// start task
				const std::int32_t id = json["id"].get<int>();

				if (tasks.contains(id))
				{
					task task = tasks.at(id);

					task.start_time = std::chrono::system_clock::now();// std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
				}
			}
			else if (json["command"].get<int>() == 4) {
				// get task (temporary command)
				std::int32_t id = json["id"].get<int>();

				if (tasks.contains(id))
				{
					task task = tasks.at(id);

					nlohmann::json response;

					response["command"] = 4;
					response["name"] = task.name;
					response["add-time"] = std::format("{:%FT%TZ}", task.add_time);

					if (task.start_time.has_value())
					{
						response["start-time"] = std::format("{:%FT%TZ}", task.start_time.value());
					}

					mt::send_packet(*socket, response);
				}
			}
			std::cout << json << '\n';
		}
	}
	catch (const nlohmann::json::type_error& e) {
		std::cout << e.what();
	}
}

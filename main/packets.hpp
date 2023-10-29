#ifndef MICRO_TASK_PACKETS_HPP
#define MICRO_TASK_PACKETS_HPP

#include <memory>

#include <string>
#include <vector>
#include <array>

#include <sockpp/tcp_socket.h>

// packet structure
// ----------------------------------------------
// | length (2) | command (1) | sub-command (1) |
// ----------------------------------------------
// 
// version request, response
// add task
// start task
// stop task
// finish task

// add list

// add group
enum class commands
{
	VERSION_REQUEST = 1,
	VERSION_RESPONSE = 2,

	ADD = 3,
	START = 4,
	STOP = 5,
	FINISH = 6,
};

enum class add_sub_commands
{
	TASK = 1,
	LIST = 2,
	GROUP = 3,
	PROJECT = 4,
	FEATURE = 5,
	MILESTONE = 6,
};

class command_packet
{
public:
	command_packet(commands command) 
		: m_command(command)
	{
	}

	commands command() const { return m_command; }

private:
	commands m_command;
};

class version_request : public command_packet
{
public:
	version_request() : command_packet(commands::VERSION_REQUEST) {}
};

class version_response : public command_packet
{
public:
	version_response(std::string version) 
		: command_packet(commands::VERSION_RESPONSE),
		version(std::move(version))
	{
	}

private:
	std::string version;
};

struct add_packet : public command_packet
{
	add_sub_commands sub_command;
};

struct add_task_packet : public add_packet
{

};

namespace mt
{
	inline std::uint16_t read_u16(const std::vector<std::byte>& input, std::size_t index)
	{
		std::array<std::byte, 2> bytes;
		std::memcpy(bytes.data(), input.data() + index, 2);
		std::uint16_t t = {};
		std::memcpy(&t, bytes.data(), 2);
		return t;
	}

	inline std::unique_ptr<command_packet> read_next_packet(sockpp::tcp_socket& socket)
	{
		std::vector<std::byte> received;

		std::array<std::byte, 512> buffer;

		while (true)
		{
			const auto read = socket.read(buffer.data(), buffer.size());

			std::copy_n(buffer.begin(), read, std::back_inserter(received));
			break;
		}

		const std::uint16_t packet_length = read_u16(received, 0);
		const std::uint16_t command = read_u16(received, 2);

		std::cout << "packet length: " << packet_length << '\n';
		std::cout << "packet command: " << command << '\n';

		return std::make_unique<version_request>();
	}

	inline void send_version(sockpp::tcp_socket& socket, std::string_view version)
	{
		unsigned char values[255]{};
		values[0] = 0;
		values[1] = 2;
		values[2] = 0;
		values[3] = 2;
		values[4] = static_cast<unsigned char>(version.size()) + 1;

		for (int i = 5; i < 5 + version.size(); i++)
		{
			values[i] = version[i - 5];
		}
		values[5 + version.size()] = 0;

		socket.write_n(&values, version.size() + 5);
	}
}

#endif

#pragma once

#include "network.hpp"

#define A4_PACKET_MESSAGE				0
#define A4_PACKET_IMAGE					1
#define A4_PACKET_CHANNEL				2
#define A4_PACKET_USER_INFO				3
#define A4_PACKET_SERVER_STATE			4

#define A4_API							__declspec(dllexport)

#define A4_USER_STATUS_ONLINE			0
#define A4_USER_STATUS_BUSY				1
#define A4_USER_STATUS_AWAY				2
#define A4_USER_STATUS_OFFLINE			3
#define A4_USER_STATUS_LOGIN_SUCCESS	100
#define A4_USER_STATUS_LOGIN_FAIL		101

typedef ne::abstract_packet a4_packet;

// There is good reason to have these macros, in my opinion.
// I should definitely improve this in the networking API though, so a user won't
//  have to make it easier on themselves by doing stuff like this.

#define A4_BEGIN_PACKET(X)	uint16 type() const { return A4_PACKET_##X; }\
							struct A4_API fields {\
								void read(ne::memory_buffer* buffer);\
								void write(ne::memory_buffer* buffer)

#define A4_END_PACKET		} data;\
							void write() { data.write(&buffer); }\
							void read() { data.read(&buffer); }

class A4_API message_packet : public a4_packet {
public:
	
	A4_BEGIN_PACKET(MESSAGE);

	uint32 timestamp;
	std::string sent_by;
	std::string body;
	std::string channel;

	A4_END_PACKET

};

class A4_API image_packet : public a4_packet {
public:

	A4_BEGIN_PACKET(IMAGE);

	uint32 timestamp;
	std::string sent_by;
	std::string channel;
	ne::memory_buffer image;

	A4_END_PACKET

};

class A4_API user_info_packet : public a4_packet {
public:

	A4_BEGIN_PACKET(USER_INFO);

	std::string name;
	uint8 status;

	A4_END_PACKET

};

class A4_API channel_packet : public a4_packet {
public:

	A4_BEGIN_PACKET(CHANNEL);

	std::string name;
	std::vector<uint8> order;
	std::vector<message_packet::fields> messages;
	std::vector<image_packet::fields> images;

	A4_END_PACKET

};

class A4_API server_state_packet : public a4_packet {
public:

	A4_BEGIN_PACKET(SERVER_STATE);
		
	std::vector<channel_packet::fields> channels;
	std::vector<user_info_packet::fields> users;

	A4_END_PACKET

};

template<typename T>
class a4_userlist {
public:

	std::vector<T> users;

	template<typename U>
	void update_user_info(const U& data) {

		for (auto& i : users) {

			// Check if we have the user already in the userlist.
			if (i.name == data.name) {

				// The user is registered locally, so we'll just update the status and return.
				i.status = data.status;
				return;

			}

		}

		// The user was not found, so we should add it.
		T user;
		user.name = data.name;
		user.status = data.status;
		users.push_back(user);

	}

	bool contains(const std::string& name) {
		for (auto& i : users) {
			if (i.name == name) {
				return true;
			}
		}
		return false;
	}

};

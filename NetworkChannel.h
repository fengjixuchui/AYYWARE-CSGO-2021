#include<cstddef>

class NetworkChannel {
public:
	std::byte pad[44];
	int chokedPackets;
};
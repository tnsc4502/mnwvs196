//
// by Buya (the developer's pseudonym)
//

#pragma once

namespace crypto
{
	void decrypt(unsigned char *buffer, unsigned char *iv, unsigned short size);
	void encrypt(unsigned char *buffer, unsigned char *iv, unsigned short size);
	void create_packet_header(unsigned char *buffer, unsigned char *iv, unsigned short size);
	unsigned short get_packet_length(unsigned char *buffer);
}

//
// by Buya (the developer's pseudonym)
//

#include "WvsCrypto.hpp"

#include <cstring>

#include "aes.h"

#include "..\Common\CryptoConstants.hpp"
#include "..\Common\ServerConstants.hpp"

namespace WvsCrypto
{

	unsigned char rotate_right(unsigned char val, unsigned short shifts)
	{
		shifts &= 7;
		return static_cast<unsigned char>((val >> shifts) | (val << (8 - shifts)));
	}

	unsigned char rotate_left(unsigned char val, unsigned short shifts)
	{
		shifts &= 7;
		return static_cast<unsigned char>((val << shifts) | (val >> (8 - shifts)));
	}

	void shuffle_iv(unsigned char *iv)
	{
		unsigned char new_iv[4] = { 0xF2, 0x53, 0x50, 0xC6 };
		unsigned char input;
		unsigned char value_input;
		unsigned int full_iv;
		unsigned int shift;
		int loop_counter = 0;

		for (; loop_counter < 4; loop_counter++)
		{
			input = iv[loop_counter];
			value_input = CryptoConstants::kIvTable[input];

			new_iv[0] += (CryptoConstants::kIvTable[new_iv[1]] - input);
			new_iv[1] -= (new_iv[2] ^ value_input);
			new_iv[2] ^= (CryptoConstants::kIvTable[new_iv[3]] + input);
			new_iv[3] -= (new_iv[0] - value_input);

			full_iv = (new_iv[3] << 24) | (new_iv[2] << 16) | (new_iv[1] << 8) | new_iv[0];
			shift = (full_iv >> 0x1D) | (full_iv << 0x03);

			new_iv[0] = static_cast<unsigned char>(shift & 0xFFu);
			new_iv[1] = static_cast<unsigned char>((shift >> 8) & 0xFFu);
			new_iv[2] = static_cast<unsigned char>((shift >> 16) & 0xFFu);
			new_iv[3] = static_cast<unsigned char>((shift >> 24) & 0xFFu);
		}

		// set iv
		memcpy(iv, new_iv, 4);
		memcpy(iv + 4, new_iv, 4);
		memcpy(iv + 8, new_iv, 4);
		memcpy(iv + 12, new_iv, 4);
	}

	void multiplyBytes(unsigned char* out, unsigned char*in, int m, int n)
	{
		for (int i = 0; i < m * n; ++i)
		{
			out[i] = in[i % 4];
		}
	}

	void aes_crypt(unsigned char *buffer, unsigned char *iv, unsigned short size)
	{
		unsigned char temp_iv[16];
		unsigned short pos = 0;
		unsigned short t_pos = 1456;
		unsigned short bytes_amount;

		aes_encrypt_ctx cx[1];
		aes_init();

		while (size > pos)
		{
			multiplyBytes(temp_iv, iv, 4, 4);

			aes_encrypt_key256(CryptoConstants::kAesKeys, cx);

			if (size > (pos + t_pos))
			{
				bytes_amount = t_pos;
			}
			else
			{
				bytes_amount = size - pos;
			}

			aes_ofb_crypt(buffer + pos, buffer + pos, bytes_amount, temp_iv, cx);

			pos += t_pos;
			t_pos = 1460;
		}
	}

	void decrypt(unsigned char *buffer, unsigned char *iv, unsigned short size)
	{
		aes_crypt(buffer, iv, size);
		shuffle_iv(iv);

		/*unsigned char a;
		unsigned char b;
		unsigned char c;
		unsigned short temp_size;
		int loop_counter = 0;

		for (; loop_counter < 3; ++loop_counter)
		{
			a = 0;
			b = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[temp_size - 1];
				c = rotate_left(c, 3);
				c = c ^ 0x13;
				a = c;
				c = c ^ b;
				c = static_cast<unsigned char>(c - temp_size);
				c = rotate_right(c, 4);
				b = a;
				buffer[temp_size - 1] = c;
			}
			a = 0;
			b = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[size - temp_size];
				c = c - 0x48;
				c = c ^ 0xFF;
				c = rotate_left(c, temp_size);
				a = c;
				c = c ^ b;
				c = static_cast<unsigned char>(c - temp_size);
				c = rotate_right(c, 3);
				b = a;
				buffer[size - temp_size] = c;
			}
		}*/
	}

	void encrypt(unsigned char *buffer, unsigned char *iv, unsigned short size)
	{
		/*unsigned char a;
		unsigned char c;
		unsigned short temp_size;
		int loop_counter = 0;

		for (; loop_counter < 3; ++loop_counter)
		{
			a = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[size - temp_size];
				c = rotate_left(c, 3);
				c = static_cast<unsigned char>(c + temp_size);
				c = c ^ a;
				a = c;
				c = rotate_right(a, temp_size);
				c = c ^ 0xFF;
				c = c + 0x48;
				buffer[size - temp_size] = c;
			}
			a = 0;
			for (temp_size = size; temp_size > 0; --temp_size)
			{
				c = buffer[temp_size - 1];
				c = rotate_left(c, 4);
				c = static_cast<unsigned char>(c + temp_size);
				c = c ^ a;
				a = c;
				c = c ^ 0x13;
				c = rotate_right(c, 3);
				buffer[temp_size - 1] = c;
			}
		}*/

		aes_crypt(buffer, iv, size);
		shuffle_iv(iv);
	}

	void create_packet_header(unsigned char *buffer, unsigned char *iv, unsigned short size)
	{

		unsigned short version = (((iv[3] << 8) | iv[2]) ^ -(ServerConstants::kGameVersion + 1));
		size = version ^ size;

		buffer[0] = version & 0xFF;
		buffer[1] = (version >> 8) & 0xFF;

		buffer[2] = size & 0xFF;
		buffer[3] = (size >> 8) & 0xFF;
	}

	unsigned short get_packet_length(unsigned char *buffer)
	{
		return ((*(unsigned short *)(buffer)) ^ (*(unsigned short *)(buffer + 2)));
	}
}

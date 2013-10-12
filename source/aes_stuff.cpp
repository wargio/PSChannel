#include "aes_stuff.h"
#include <cstring>
#include <stdio.h>
#include "core_util.h"

#include <fstream>

#define AES_TYPE			256

using namespace std;

static char hex_C[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
static u8 bin0[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
static u8 bin1[] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0};
void string_to_hex(char* str, u8 *in, u32 len){
	for(int i=0, j=0, k=0;i<len;i++, j=0, k=0){
		while(hex_C[j]!=str[i*2] && j<16)
			j++;
		while(hex_C[k]!=str[i*2+1] && j<16)
			k++;
		if(str[i*2]==0)
			break;
		in[i] = bin1[j] | bin0[k];
	}
}

u32 get_len(const char*in){
	return get_size(in);
}

void decrypt_xml(const char* fin, const char* fout){
	
	AES_KEY k;
	u32 i;
	u8 tmp[16];

	u8 key[16] = "YOURKEYaaaaaaaa";
	u8 iv [16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	memset(&k, 0, sizeof k);
	AES_set_decrypt_key(key, AES_TYPE, &k);

	u32 len = get_len(fin);
	uint8_t in [16];
	uint8_t out[17];

	fstream fr;
	fstream fw;

	fr.open(fin, ios::in | ios::binary);
	fw.open(fout, ios::out | ios::binary);

	len/=16;

	for(int j=0;j<len && !fr.eof();j++){
		char str[34];
		fr.read(str,32);
		str[33] = '\0';
		memset(in, 0, sizeof in);
		string_to_hex(str,in, strlen(str)/2);
		memcpy(tmp, in, 16);
		AES_decrypt(in, out, &k);

		for (i = 0; i < 16; i++)
			out[i] ^= iv[i];

		memcpy(iv, tmp, 16);
		out[16] = '\0';

		fw.write((char*)out,16);
		if(out[15] == 0x00) break;

	}
	fw.close();
	fr.close();
}





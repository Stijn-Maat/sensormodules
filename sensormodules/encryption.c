/*
 * encryption.c
 *
 * Created: 6-10-2023 11:49:23
 *  Author: HP
 */ 
#include "encryption.h"

/*
	encrypt_decrypt
	This function will encrypt and decrypt data by xor-ing the data package with a key
	
	uint8_t *pData: is the data buffer which must be encrypted.
	after encryption is complete pData will hold the encrypted data
	
	uint8_t *pKey: this parameter will hold the key. key can be multiple bytes long
	Important!: the same key must be used to decrypt the data
	
	size_t dataSize and keySize: give the amount of elements for the data packet and key packet
*/

void encrypt_decrypt(uint8_t *pData, uint8_t *pKey, size_t dataSize, size_t keySize){
	// encrypt data
	for (uint8_t i=0;i<dataSize;i++){
		*(pData+i) = *(pData+i) ^ (*(pKey+(i%keySize)));
	}
}
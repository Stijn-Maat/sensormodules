/*
 * encryption.c
 *
 * Created: 6-10-2023 11:49:23
 *  Author: HP
 */ 
#include "encryption.h"

/*	encrypt_data
	This function will encrypt data by xor-ing the entire key per byte
	and does this for the entire data package
	
	uint8_t *pData: is the data buffer which must be encrypted.
	after encryption is complete pData will hold the encrypted data
	
	uint8_t *pKey: this parameter will hold the key. key can be multiple bytes long
	Important!: the same key must be used to decrypt the data
	
	size_t dataSize and keySize: give the amount of elements for the data packet and key packet
*/

void encrypt_data(uint8_t *pData, uint8_t *pKey, size_t dataSize, size_t keySize){
	uint8_t temp;
	
	// encrypt data
	for (uint8_t i=0;i<dataSize;i++){
		temp=*(pData+i);
		for (uint8_t j=0;j<keySize;j++){
			temp=temp^(*(pKey+j));
		}
		*(pData+i)=temp;
	}
}

/*	decrypt_data
	This function will decrypt the data by reverse xor-ing the entire key per byte
	and does this for the entire data package
	
	uint8_t *pData: is the data buffer which must be decrypted.
	after encryption is complete pData will hold the decrypted data
	
	uint8_t *pKey: this parameter will hold the key. key can be multiple bytes long
	Important!: the key must be the same key used to encrypt the data
	
	size_t dataSize and keySize: give the amount of elements for the data packet and key packet
*/
void decrypt_data(uint8_t *pData, uint8_t *pKey, size_t dataSize, size_t keySize){
	uint8_t temp;
	
	//decrypt data
	for (uint8_t i=0;i<dataSize;i++){
		temp=*(pData+dataSize-1-i);
		for (uint8_t j=0;j<keySize;j++){
			temp=temp^(*(pKey+keySize-1-j));
		}
		*(pData+dataSize-1-i)=temp;
	}
}
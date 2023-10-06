/*
 * encryption.h
 *
 * Created: 6-10-2023 11:50:43
 *  Author: HP
 */ 


#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_

#include <stdio.h>
#include <stdlib.h>

void encrypt_data(uint8_t *pData, uint8_t *pKey, size_t dataSize, size_t keySize);
void decrypt_data(uint8_t *pData, uint8_t *pKey, size_t dataSize, size_t keySize);

#endif /* ENCRYPTION_H_ */
/*
 * types.h
 *
 *  Created on: Apr 15, 2011
 */

#ifndef TYPES_H_
#define TYPES_H_

typedef unsigned int DWORD;
typedef unsigned short int WORD;
typedef unsigned char BYTE;

typedef DWORD FOURCC; //Four-character code
typedef FOURCC CKID;	//Four-character-code chunk identifier
typedef DWORD CKSIZE; //32-bit unsigned size value

#define RIFF_CKID 0x52494646
#define DATA_CKID 0x61746164
#define	RIFF_FORMAT 0x57415645
#define FMT_CKID 0x666D7420
#define	FMT_CKSIZE	16
#define	FMT_FORMAT_CAT	1
#define	CHANNEL_MONO	1
#define	CHANNEL_STEREO	2

#endif /* TYPES_H_ */

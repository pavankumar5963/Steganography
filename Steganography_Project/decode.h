#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h"

/*
 * Structure to store information required for
 * decoding stego image to new file
 * Info about output and intermediate data is 
 * also stored
 */

/* Get file extention size by decoding 32 bytes */
#define DECODE_FILE_EXTN_SIZE 32

/* Get file size by decoding 32 bytes*/
#define DECODE_FILE_SIZE 32

typedef struct  _DecodeInfo
{
	/* Encoded stego image info */
	char *stego_image_fname;
	FILE *fptr_stego_image;
	uint image_data_size;
	char image_data[MAX_IMAGE_BUF_SIZE];

	/* Decoded output file info */
	char *output_file_name;
	FILE *fptr_output_file;
	char extn_output_file[MAX_FILE_SUFFIX];
	char decode_data[MAX_SECRET_BUF_SIZE];

	/* Passcode */
	char *passcode;
	uint passcode_size;

}	DecodeInfo;


/* Decoding function prototypes */

/* Read and validation Decode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Get File pointer for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Performs Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get the magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode file extention size */
Status decode_output_file_extn_size(DecodeInfo *decInfo);

/* Decode file extenstion */
Status decode_output_file_extn(uint extn_size, DecodeInfo *decInfo);

/* Decode file size */
Status decode_file_size(DecodeInfo *decInfo);

/* Decoding file data */
Status decode_data_to_file(DecodeInfo *decInfo);

/* Decode a array of LSB into byte */
Status decode_lsb_to_byte(char *decode_data, char *image_data);

#endif

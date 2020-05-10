#include<stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>

/* Function Definiation */

/* Get File names and validate
 * Input: Stego Image file name and Output file name
 * Output: Stores the above files names
 * Return Value: e-success or e_failure, on file errors
 */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
	if (argc > 2)
	{
		if (strcmp(strstr(argv[2],"."), ".bmp") == 0)
        {
			decInfo->stego_image_fname = argv[2];
        }
        else
        {
            fprintf(stderr,"Error: Stego image %s format should be .bmp\n", argv[2]);
            return e_failure;
        }
	}
	else
	{
		fprintf(stderr,"Error: Arguments are missing\n");
        printf("./Test_encode: Encoding: ./Test_encode -e <.bmp file> <.txt file> [Output file]\n");
        printf("./Test_encode: Deconding: ./Test_encode -d <.bmp file> [output file]\n");
		return e_failure;
	}

	if (argc > 3)
	{
		strncpy(decInfo->extn_output_file, strstr(argv[3], "."), 4);
        if ((strncmp(decInfo->extn_output_file, ".txt", 4) == 0) || (strncmp(decInfo->extn_output_file, ".c", 2) == 0) || (strncmp(decInfo->extn_output_file, ".sh", 3) == 0))
        {
				decInfo->output_file_name = argv[3];
        }
        else
        {
      		fprintf(stderr,"Error: Output file %s format should be .txt or .c or .sh\n", argv[3]);
            return e_failure;
        }
	}
	else
	{
		decInfo->output_file_name = NULL;
	}
	
	/* Passcode validation*/
		if (argc > 4)
		{
			if (strcmp(argv[4], "-p") == 0)
			{
				if (argc > 5)
				{
					decInfo->passcode = argv[5];
					decInfo->passcode_size = strlen(decInfo->passcode);
				}
				else
				{
					fprintf(stderr,"Error: No passcode found\n");
					return e_failure;
				}
			}
			else
			{
				fprintf(stderr,"Error: Invalid option\n");
				return e_failure;
			}
		}
		else
		{
			decInfo->passcode = NULL;
			decInfo->passcode_size = 1;
		}
	return e_success;
}

/* Get File pointers for i/p and o/p files
 * Inputs: Stego Image file, Output file
 * Ouput: File pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_decode_files(DecodeInfo *decInfo)
{
	static int open = 0;
	if (open == 0)
	{
		open++;
		// Open Stego Image file
		decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

		// Error handling
		if (decInfo->fptr_stego_image == NULL)
		{
			perror("fopen");
			fprintf(stderr, "Error: Unable to open file %s\n", decInfo->stego_image_fname);
			return e_failure;
		}
		else
		{
			printf("INFO: Opened %s\n", decInfo->stego_image_fname);
		}
	}
	else 
	{
		// Open Output file
		decInfo->fptr_output_file = fopen(decInfo->output_file_name, "w");
		// Error handling
		if (decInfo->fptr_output_file == NULL)
		{
			perror("fopen");
			fprintf(stderr, "Error: Unable to open file %s\n", decInfo->output_file_name);
			return e_failure;
		}
		else
		{
			printf("INFO: Opended %s\n", decInfo->output_file_name);
		}
	}
	return e_success;
}

/* Decoding stego image to another file
 * Input: File info of stego image and output file
 * Output: Decoded message copied in output file
 * Return: e_success or e_failure, on file error
 */
Status do_decoding(DecodeInfo *decInfo)
{
	printf("INFO: ## Decoding Procedure Started ##\n");
	printf("INFO: Opening required files\n");
	
	// Open the required files
	if (open_decode_files(decInfo) == e_success)
	{
		printf("INFO: Done\n");
		uint raster_data;
		fseek(decInfo->fptr_stego_image, 10L, SEEK_SET);
		fread(&raster_data, sizeof(int), 1, decInfo->fptr_stego_image);
		fseek(decInfo->fptr_stego_image, raster_data, SEEK_SET);
		printf("INFO: Decoding Magic String Signature\n");
		
		// Decoding magic string signature
		if (decode_magic_string(decInfo->passcode, decInfo) == e_success)
		{
			printf("INFO: Done\n");
			
			// Decoding Output File Extenstion Size
			printf("INFO: Decoding Output File Extension\n");
			if (decode_output_file_extn_size(decInfo) == e_success)
			{
				printf("INFO: Done\n");

				// Check for output file provided or not
				if (decInfo->output_file_name == NULL)
				{
					
					if (strncmp(decInfo->extn_output_file, ".txt", 4) == 0)
					{
						decInfo->output_file_name = "decoded.txt";
						printf("INFO: Output file not mentioned. Creating %s as default\n", "decoded.txt");
					}
					else if (strncmp(decInfo->extn_output_file, ".sh", 3) == 0)
					{
						decInfo->output_file_name = "decoded.sh";
						printf("INFO: Output file not mentioned. Creating %s as default\n", "decoded.sh");
					}
					else
					{
						decInfo->output_file_name = "decoded.c";
						printf("INFO: Output file not mentioned. Creating %s as default\n", "decode.c");
					}
					if (open_decode_files(decInfo) == e_success)
					{
						printf("INFO: Opened %s\n", decInfo->output_file_name);
					}
					else
					{
						fprintf(stderr,"Error: %s function failed\n", "open_decode_files");
						return e_failure;
					}
				}
				else
				{
					printf("INFO: Output file is mentioned\n");
					if (open_decode_files(decInfo) == e_success)
					{
						printf("INFO: Opened %s\n", decInfo->output_file_name);
					}
					else
					{
						fprintf(stderr,"Error: %s function failed\n", "open_decode_files");
						return e_failure;
					}
				}
				printf("INFO: Done. Opened all required files\n");

				// Decoding File Size
				printf("INFO: Decoding File Size\n");
				if (decode_file_size(decInfo) == e_success)
				{
					printf("INFO: Done\n");

					// Decoding File Data
					printf("INFO: Decoding File Data\n");
					if (decode_data_to_file(decInfo) == e_failure)
					{
						fprintf(stderr,"Error: %s function failed\n", "decode_data_to_file()");
						return e_failure;
					}
				}
				else
				{
					fprintf(stderr,"Error: %s function failed\n", "decode_file_size()");
					return e_failure;
				}
			}
			else
			{
				fprintf(stderr, "Error: %s function failed\n", "decode_output_file_extn_size()");
				return e_failure;
			}
		}
		else
		{
			fprintf(stderr,"Error: %s function failed\n", "decode_magic_string()");
			return e_failure;
		}
	}
	else
	{
		fprintf(stderr,"Error: %s function failed\n", "open_decode_files()");
		return e_failure;
	}
	printf("INFO: Done\n");
	return e_success;
}

/* Decodes the magic string from stego image
 * Input: 	Magic string and File info of input and output
 * Output:	Proceed decoing if magic string if found or else stop decoding
 * Return:	e_success or e_failure
 */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
	uint i;
	if (magic_string != NULL)
	{
		for (i = 0; i < decInfo->passcode_size; i++)
		{
			fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);
			if (ferror(decInfo->fptr_stego_image))
    		{
        		fprintf(stderr,"Error: While reading the data from stego image file\n");
         		clearerr(decInfo->fptr_stego_image);
         		return e_failure;
    		}
			if (decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
			{
				if (decInfo->decode_data[0] == magic_string[i])
				{
					continue;
				}
				else
				{
					fprintf(stderr,"Error: Incorrect passcode\n");
					return e_failure;
				}
			}
			else
			{
				fprintf(stderr, "Error: %s function failed\n","decode_lsb_to_byte()");
				return e_failure;
			}
		}
	}
	fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
        fprintf(stderr,"Error: While reading the data from stego image file\n");
        clearerr(decInfo->fptr_stego_image);
        return e_failure;
    }
	if (decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
	{
		if (decInfo->decode_data[0] == '*')
		{
				return e_success;
		}
		else
		{
				fprintf(stderr,"Error: Incorrect passcode\n");
				return e_failure;
		}
	}
	else
	{
		fprintf(stderr, "Error: %s function failed\n","decode_lsb_to_byte()");
		return e_failure;
	}
}

/* Decode file extenstion size from stego image
 * Input: File info of stego image and output file
 * Output: Decode the extenstion size from stego image and store in image_data_size
 * Return: e_success or e_failure
 */
Status decode_output_file_extn_size(DecodeInfo *decInfo)
{
	char file_size[DECODE_FILE_EXTN_SIZE];
	fread(file_size, sizeof(char), DECODE_FILE_EXTN_SIZE, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
         fprintf(stderr,"Error: While reading the data from stego image file\n");
         clearerr(decInfo->fptr_stego_image);
         return e_failure;
    }

	decInfo->image_data_size = 0;
	for (uint i = 0; i < DECODE_FILE_EXTN_SIZE; i++)
	{
		decInfo->image_data_size <<= 1;
		decInfo->image_data_size |= (uint) (file_size[i] & 0x01);
	}
	if (decode_output_file_extn(decInfo->image_data_size, decInfo) == e_failure)
	{
		fprintf(stderr,"Error: %s function failed\n","decode_output_file_extn()");
		return e_failure;
	}
	return e_success;
}

/* Decode file extenstion from stego image 
 * Input: Extenstion Size and File info of stego image
 * Output: Decodes the file extenstion and store in extn_output_file
 * Return: e_success or e_failure
 */
Status decode_output_file_extn(uint extn_size, DecodeInfo *decInfo)
{
	for (uint i = 0; i < extn_size; i++)
	{
		fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
		if (ferror(decInfo->fptr_stego_image))
    	{
        	fprintf(stderr,"Error: While reading the data from stego image file\n");
         	clearerr(decInfo->fptr_stego_image);
         	return e_failure;
    	}
		if (decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
		{
			decInfo->extn_output_file[i] = decInfo->decode_data[0];
		}
	}
	return e_success;
}

/* Decode file size from stego image
 * Input: FILE info of stego image and output file
 * Output: Decodes the file image and store in image_data_size
 * Return: e_success or e_failure
 */
Status decode_file_size(DecodeInfo *decInfo)
{
	char file_size[DECODE_FILE_SIZE];
	fread(file_size, sizeof(char), DECODE_FILE_SIZE, decInfo->fptr_stego_image);
	if (ferror(decInfo->fptr_stego_image))
    {
         fprintf(stderr,"Error: While reading the data from stego image file\n");
         clearerr(decInfo->fptr_stego_image);
         return e_failure;
    }
	decInfo->image_data_size = 0;
	for (uint i = 0; i < DECODE_FILE_SIZE; i++)
	{
		decInfo->image_data_size <<= 1;
		decInfo->image_data_size |= (uint) (file_size[i] & 0x01);
	}
	return e_success;
}

/* Decode file data from stego image
 * Input: FILE info of stego image and output decode file
 * Output: Write decode data in the output file
 * Return: e_success or e_failure
 */
Status decode_data_to_file(DecodeInfo *decInfo)
{
	for (uint i = 0; i < decInfo->image_data_size; i++)
	{
		fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
		if (ferror(decInfo->fptr_stego_image))
    	{
        	 fprintf(stderr,"Error: While reading the data from stego image file\n");
         	clearerr(decInfo->fptr_stego_image);
         	return e_failure;
    	}
		if(decode_lsb_to_byte(decInfo->decode_data, decInfo->image_data) == e_success)
		{
			fwrite(decInfo->decode_data, sizeof(char), 1, decInfo->fptr_output_file);
			if (ferror(decInfo->fptr_stego_image))
    		{
        		fprintf(stderr,"Error: While reading the data from stego image file\n");
         		clearerr(decInfo->fptr_stego_image);
         		return e_failure;
    		}

		}
		else
		{
			fprintf(stderr, "Error: %s function failed\n","decode_lsb_to_byte()");
			return e_failure;
		}
	}
	return e_success;
}

/* Get lsb bit from stego image byte 
 * Input: Image_data array and decode_data character
 * Output: Decode the image_data and stores the 1 byte data in decode_data
 * Return: e_success or e_failure
 */
Status decode_lsb_to_byte(char *decode_data, char *image_data)
{
	decode_data[0] = 0;
	for (uint i = 0; i < MAX_IMAGE_BUF_SIZE; i++)
	{
		decode_data[0] <<= 1;
		decode_data[0] |=  (image_data[i] & 0x01);
	}
	return e_success;
}

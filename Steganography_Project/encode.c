
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height, bits_per_pixel;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
	
	// Read bits per pixel
	fseek(fptr_image, 2L, SEEK_CUR);
	fread(&bits_per_pixel, sizeof(short), 1, fptr_image);
	rewind(fptr_image);
    
	// Return image capacity
    return width * height * bits_per_pixel;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_encode_files(EncodeInfo *encInfo)
{
	static int open_count = 0;
	if (open_count == 0)
	{
		open_count++;
    	// Src Image file
    	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_src_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    		return e_failure;
    	}
		else
		{
			printf("INFO: Opened %s\n", encInfo->src_image_fname);
		}
	
   		 // Secret file
    	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_secret == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    		return e_failure;
    	}
		else
		{
			printf("INFO: Opened %s\n", encInfo->secret_fname);
			printf("INFO: Done\n");
		}
	}
	else
	{
    	// Stego Image file
    	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    	// Do Error handling
    	if (encInfo->fptr_stego_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    		return e_failure;
    	}
		else
		{
			printf("INFO: opened %s\n", encInfo->stego_image_fname);
		}
	}

    // No failure return e_success
    return e_success;
}

/* Check for operation to be performed
 * Input: Command line arguments
 * Output: Operation to be performed
 * Return: integer constant coresponding to operation
 */
OperationType check_operation_type(char *argv[])
{
	
	if ((strcmp(argv[1],"-e")) == 0)
	{
		return e_encode;
	}
	else if ((strcmp(argv[1], "-d")) == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}

/* Read and validate command line argument
 * Input: Command line Argument count and Arguments wtih File name info
 * Output: File names stored in encoded Info
 * Return: e_success or e_failure
 */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
		if (argc > 2)
		{
			if (strcmp(strstr(argv[2],"."), ".bmp") == 0)
			{
				encInfo->src_image_fname = argv[2];
			}
			else
			{
				fprintf(stderr,"Error: Source image %s format should be .bmp\n", argv[2]);
				return e_failure;
			}
		}
		else
		{
				fprintf(stderr,"Error: Arguments are missing\n");
				printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
				printf("./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
				return e_failure;
		}
		
		if (argc > 3)
		{
			// Copy text file extenstion in extn_secret_file
			strncpy(encInfo->extn_secret_file, strstr(argv[3], "."), 4);
			if ((strncmp(encInfo->extn_secret_file, ".txt", 4) == 0) || (strncmp(encInfo->extn_secret_file, ".c", 2) == 0) || (strncmp(encInfo->extn_secret_file, ".sh", 3) == 0))
			{
				encInfo->secret_fname = argv[3];
			}
			else
			{
				fprintf(stderr,"Error: Secrete file %s format should be .txt or .c or .sh\n", argv[3]);
				return e_failure;
			}
		}
		else
		{
				fprintf(stderr,"Error: Arguments are missing\n");
				printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
				printf("./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
				return e_failure;
		}
		if (argc > 4)
		{
			if (strcmp(strstr(argv[4],"."), ".bmp") == 0)
			{
				encInfo->stego_image_fname = argv[4];
			}
			else
			{
				fprintf(stderr,"Error: Output image %s format should be .bmp\n", argv[2]);
				return e_failure;
			}
		}
		else
		{
			encInfo->stego_image_fname = NULL;
		}

		/* Passcode validation*/
		if (argc > 5)
		{
			if (strcmp(argv[5], "-p") == 0)
			{
				if (argc > 6)
				{
					encInfo->passcode = argv[6];
					encInfo->passcode_size = strlen(encInfo->passcode);
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
			encInfo->passcode = NULL;
			encInfo->passcode_size = 1;
		}

	return e_success;
}

/* Encoding the secret file data to stego image
 * Input: FILE info of image, secret file and stego image
 * Output: Encodes the data in secret to stego image
 * Return: e_success or e_failure;
 */
Status do_encoding(EncodeInfo *encInfo)
{
	printf("INFO: Opening required files\n");
	// Open required files
    if (open_encode_files(encInfo) == e_success)
    {
    	printf("INFO: ## Encoding Procedure Started ##\n");	
    	printf("INFO: Checking for %s size\n", encInfo->secret_fname);
    	encInfo->size_secret_file = (long) get_file_size(encInfo->fptr_secret);
    	if (encInfo->size_secret_file)
   		{
    		printf("INFO: Done. Not Empty\n");
        	printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
      		encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
			if (check_capacity(encInfo) == e_success)
			{
				printf("INFO: Done. Found OK\n");
			
				// check for output .bmp
				if (encInfo->stego_image_fname  == NULL)
				{
					printf("INFO: Output File not mentioned. ");
					encInfo->stego_image_fname = "steged_img.bmp";
					printf("Creating steged_img.bmp as default\n");
					if (open_encode_files(encInfo) == e_failure)
					{
						fprintf(stderr,"Error: %s function failed\n", "open_encode_files");
						return e_failure;		
					}
				}
				else
				{
					printf("INFO: Output File mentioned. ");
					if (open_encode_files(encInfo) == e_failure)
					{
						fprintf(stderr,"Error: %s function failed\n", "open_encode_files");
						return e_failure;
					}
					printf("INFO: Opened %s\n",encInfo->stego_image_fname);
				}

				//Copying Image Header
				printf("INFO: Copying Image Header\n");
				if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
				{
					printf("INFO: Done\n");

					// Encode magic string in destination image
					printf("INFO: Encoding Magic String Signature\n");
					if (encode_magic_string(encInfo->passcode, encInfo) == e_success)
					{
						printf("INFO: Done\n");

						// Encoding secret.txt file extenstion size
						printf("INFO: Encoding %s File Extenstion Size\n", encInfo->secret_fname);
						if (encode_secret_file_extn_size(encInfo) == e_success)
						{
							printf("INFO: Done\n");
						
							// Encode secret file extenstion
							printf("INFO: Encoding %s File Extenstion\n", encInfo->secret_fname);
							if (encode_secret_file_extn(encInfo) == e_success)
							{
								printf("INFO: Done\n");
							
								// Encode secret file size
								printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
								if (encode_secret_file_size(encInfo) == e_success)
								{
									printf("INFO: Done\n");
								
									// Encode secret file data
									printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
									if (encode_secret_file_data(encInfo) == e_success)
									{
										printf("INFO: Done\n");
									
										// Copy remaining image bytes from src to dest
										printf("INFO: Copying Left Over Data\n");
										if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
										{
											printf("INFO: Done\n");
										}
										else
										{
											fprintf(stderr, "Error: %s function failed\n", "copy_remaining_img_data()");
											return e_failure;
										}
									}
									else
									{
										fprintf(stderr, "Error: %s function failed\n", "encode_secret_file_data()");
										return e_failure;
									}	
								}
							}
							else
							{
								fprintf(stderr, "Error: %s function failed\n", "encode_secret_file_extn()");
								return e_failure;
							}
						}
						else
						{
							fprintf(stderr,"Error: %s function failed\n","encode_secret_file_extn()");
							return e_failure;
						}
					}
					else
					{
						fprintf(stderr,"Error: %s function failed\n","encode_magic_string()");
						return e_failure;
					}

				}
				else
				{
					fprintf(stderr,"Error: %s function failed\n","copy_bmp_header()");
					return e_failure;
				}
			
			}
			else
			{
				fprintf(stderr,"Error: %s function failed\n","check_capacity()");
				return e_failure;
			}
     	}
     	else
     	{   
			printf("INFO: Secret file is empty\n");
        	return e_failure;
     	}
	}
 	else
   	{
    	fprintf(stderr, "Error: %s function failed\n","open_encode_files()");
       	return 1;
    }

	 return e_success;
}

/* Get the file size 
   * Input:  File pointer
   * Return: Size of file in bytes
   */
uint get_file_size(FILE *fptr)
{
	uint size;
	fseek(fptr, 0L, SEEK_END);
	size = (uint) ftell(fptr);
	rewind(fptr);
	return size;
}

/* To check the capacity of source image to handle secret data
 * Input: File info source image, stego image and secret file
 * Output: Get Source image capacity and store in image_capacity
 * Return: e_success or e_failure
 */
Status check_capacity(EncodeInfo *encInfo)
{
	uint bytes_for_secret_file_size = 4;
	if (encInfo->image_capacity >= (MAX_IMAGE_BUF_SIZE * (encInfo->passcode_size + MAX_FILE_SUFFIX + (uint)sizeof(encInfo->extn_secret_file) + bytes_for_secret_file_size + encInfo->size_secret_file)))
	{
		return e_success;
	}
	return e_failure;
}

/* Copy the header bytes data to stego image
 * Input: FILE pointers source and stego image
 * Output: Copies header data of source image to stego image
 * Return: e_success or e_failure
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	uint raster_data;
	fseek(fptr_src_image, 10L, SEEK_SET);
	fread(&raster_data, sizeof(int), 1, fptr_src_image);
	rewind(fptr_src_image);
	char header_data[raster_data];
	fread(header_data, sizeof(char), (size_t)raster_data, fptr_src_image);
	if (ferror(fptr_src_image))
	{
		fprintf(stderr,"Error: While reading the data from source image file\n");
		clearerr(fptr_src_image);
		return e_failure;
	}
	fwrite(header_data, sizeof(char), (size_t)raster_data, fptr_dest_image);
	if (ferror(fptr_dest_image))
	{
		fprintf(stderr,"Error: While writing the data from destination image file\n");
		clearerr(fptr_dest_image);
		return e_failure;
	}
	return e_success;
}

/* Encode Magic string in stego image
 * Input: magic string and FILEs info
 * Output: Encode magic character '*' in stego image first 8 bytes from image data
 * Return: e_success or e_failure
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	if (magic_string != NULL)
	{
		for (uint i = 0; i < encInfo->passcode_size; i++)
		{
			// copy * to secret_data
			if ((strncpy(encInfo->secret_data, (magic_string + i), 1)) == NULL)
			{
				return e_failure;
			}
			// Divide secrete_data to 8 bits i.e MSB at 0th index and LSB at 7th index	
			fread(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);
			if (ferror(encInfo->fptr_src_image))
			{
				fprintf(stderr,"Error: While reading the data from source nimage file\n");
				clearerr(encInfo->fptr_src_image);
				return e_failure;
			}
			if (encode_byte_to_lsb(encInfo->secret_data[0], encInfo->image_data) == e_failure)
			{
				fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
				return e_failure;
			}
			fwrite(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);
			if (ferror(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"Error: While writing the data to destination image file\n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}
		}
	}
	
	// Encode "*" after passcode or Encode "*" 
	if ((strncpy(encInfo->secret_data, MAGIC_STRING, 1)) == NULL)
	{
			return e_failure;
	}

	// Divide secrete_data to 8 bits i.e MSB at 0th index and LSB at 7th index	
	fread(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);
	if (ferror(encInfo->fptr_src_image))
	{
			fprintf(stderr,"Error: While reading the data from source image file\n");
			clearerr(encInfo->fptr_src_image);
			return e_failure;
	}
	if (encode_byte_to_lsb(encInfo->secret_data[0], encInfo->image_data) == e_failure)
	{
			fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
			return e_failure;
	}
	fwrite(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);
	if (ferror(encInfo->fptr_stego_image))
	{
		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
}

/* Encode the secret file extenstion size to stego image
 * Input: FILEs info
 * Output: Encode file extenstion size to stego image 
 * Return: e_success or e_failure
 */
Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
	uint mask = 0x80000000;
	uint len;
	for (len = 0; (len < MAX_FILE_SUFFIX) && (encInfo->extn_secret_file[len] != '\0'); len++);
	uint size_bits = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int);
	char data[size_bits];
	// Divide secrete_data to 8 bits i.e MSB at 0th index and LSB at 7th index
	for (int i = 0; i < size_bits; i++)
	{
		if ((uint) len & mask)
		{
			data[i] = 1;
		}
		else
		{
			data[i] = 0;
		}
		mask >>= 1;
		data[i] = (char) ((char) (fgetc(encInfo->fptr_src_image) & (~(0x01))) | (data[i]));
	}
	fwrite(data, sizeof(char), (size_t) size_bits, encInfo->fptr_stego_image);
	if (ferror(encInfo->fptr_stego_image))
	{
		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
}

/* Encode file extenstion to stego image
 * Input: Files info
 * Output: Copy source image data to stego image with encoded file extenstion
 * Return: e_success or e_failure
 */
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
	uint len;
	for (len = 0; (len < MAX_FILE_SUFFIX) && (encInfo->extn_secret_file[len] != '\0'); len++);
	// Divide secrete_data to 8 bits i.e MSB at 0th index and LSB at ((len * 8) - 1)th index

	for (uint i = 0; i < len; i++)
	{
		fread(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);
		if (ferror(encInfo->fptr_src_image))
		{
			fprintf(stderr,"Error: While reading the data from source nimage file\n");
			clearerr(encInfo->fptr_src_image);
			return e_failure;
		}
		if (encode_byte_to_lsb(encInfo->extn_secret_file[i], encInfo->image_data) == e_failure)
		{
			fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
			return e_failure;
		}
		fwrite(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);
		if (ferror(encInfo->fptr_stego_image))
		{
			fprintf(stderr,"Error: While writing the data to destination image file\n");
			clearerr(encInfo->fptr_stego_image);
			return e_failure;
		}
	}
	return e_success;
}

/* Encode secret file size data to stego image
 * Input: Files info
 * Output: Encode secret file size data to stego image
 * Return: e_success or e_failure
 */
Status encode_secret_file_size(EncodeInfo *encInfo)
{
	uint mask = 0x80000000;
	uint len = (uint) encInfo->size_secret_file;
	uint size_bits = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int);
	char data[size_bits];
	// Divide secrete_data to 8 bits i.e MSB at 0th index and LSB at 31th index
	for (int i = 0; i < size_bits; i++)
	{
		if ((uint) len & mask)
		{
			data[i] = 1;
		}
		else
		{
			data[i] = 0;
		}
		mask >>= 1;
		data[i] = (char) ((char) (fgetc(encInfo->fptr_src_image) & ~(0x01))| (data[i]));
	}
	fwrite(data, sizeof(char), (size_t) size_bits, encInfo->fptr_stego_image);
	if (ferror(encInfo->fptr_stego_image))
	{
 		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
}

/* Encode secret file data to stego image file
 * Input: Files info
 * Output: Encodes secret data to stego image
 * Return: e_success or e_failure
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char data[(int) encInfo->size_secret_file];
	fread(data, sizeof(char), (uint) encInfo->size_secret_file, encInfo->fptr_secret);
	if (ferror(encInfo->fptr_secret))
	{
		fprintf(stderr,"Error: While reading the data to secret file\n");
		clearerr(encInfo->fptr_secret);
		return e_failure;
	}

	// Encoding the text in secret file to dest image
	if (encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
	{
		fprintf(stderr, "Error: %s function failed\n", "encode_data_to_image()");
		return e_failure;
	}
	return e_success;
}

/* Encode secret data to image data
 * Input: Secret data, secret data size, File pointer of source and stego image files
 * Output: Gets source image data and encodes it with secret data by calling another function
 * Return: e_success or e_failure
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char image_buffer[MAX_IMAGE_BUF_SIZE];
	for(uint i = 0; i < size; i++)
	{
		fread(image_buffer, sizeof(char), MAX_IMAGE_BUF_SIZE, fptr_src_image);
		if (encode_byte_to_lsb(data[i], image_buffer)  == e_success)
		{

			fwrite(image_buffer, sizeof(char), MAX_IMAGE_BUF_SIZE, fptr_stego_image);
		}
		else
		{
			fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
			return e_failure;
		}
	}
	return e_success;
}

/* Encodes secret byte data to image 8 byte data
 * Input: Secret 1 byte data and Image 8 byte data
 * Output: Encode data to image_buffer
 * Return: e_success or e_failure 
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	unsigned char mask = 0x80;
	for(uint i = 0; i < MAX_IMAGE_BUF_SIZE; i++)
	{
		if ( data & mask)
		{
			image_buffer[i] = (image_buffer[i] | (unsigned char) (0x01));
		}
		else
		{
			image_buffer[i] = (image_buffer[i] & (unsigned char) (~(0x01)));
		}
		mask >>= 1;
	}
	return e_success;
}

/* Copy remaining source image data to stego image
 * Input: File pointer of source and stego image
 * Output: Remaining image data copied from source image 
 * Return: e_success or e_failure
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	int ch;
	while ((ch = getc(fptr_src)) != EOF)  // EOF is macro with value -1   
    {
		if (ferror(fptr_src))
		{
			fprintf(stderr, "Error: unable to read from %s", "Source image file");
			clearerr(fptr_src);
			return e_failure;
		}
      	putc(ch, fptr_dest);
		if (ferror(fptr_dest))
		{
			fprintf(stderr, "Error: unable to write to  %s", "Destination image file");
			clearerr(fptr_dest);
			return e_failure;
		}
    }
	return e_success;
}

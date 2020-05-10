/*
Title: 	Programme to encode secret file data to source image file by creating new image file with option '-e' and decode data from new image file to new text file
Date: 	01-11-2019
Author: B. Pavan Kumar
Input:	For option 'e'
		a) Source image file name
		b) Secret text file name
		c) Output image file name
		For option 'd'
		a) Stego image file name
		b) Output text file name
Output:	Data in secret file copied to Output text file
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"


int main(int argc, char **argv)
{
		// Input argument validation
		if (argc > 1 && argc < 6)
		{		
				// Option validation

				// Encoding
				if ((check_operation_type(argv)) == e_encode)
				{
						EncodeInfo encInfo;
						
						// Read the file name
						if (read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
						{
							// Encoding secret data 
							if (do_encoding(&encInfo) == e_failure)
							{
								fprintf(stderr,"Error: %s function failed\n","do_encoding()");
								return 1;
							}
							fclose(encInfo.fptr_src_image);
							fclose(encInfo.fptr_secret);
							fclose(encInfo.fptr_stego_image);
							printf("INFO: ## Encoding Done Successfully ##\n");
						}
						else
						{
								fprintf(stderr, "Error: %s function failed\n", "read_and_validation_encode_args()");
								return 1;
						}
				}
				// Decoding
				else if (check_operation_type(argv) == e_decode)
				{
					DecodeInfo decInfo;

					// To read and store file names
					if (read_and_validate_decode_args(argc, argv, &decInfo) == e_success)
					{
						// Decode the stego image 
						if (do_decoding(&decInfo) == e_success)
						{
							printf("INFO: ## Decoding Done Successfully ##\n");
							fclose(decInfo.fptr_stego_image);
							fclose(decInfo.fptr_output_file);
						}
						else
						{
							fprintf(stderr,"Error: %s function failed\n","do_decoding()");
							return e_failure;
						}
					}
					else
					{
						fprintf(stderr,"Error: %s function failed\n", "read_and_validate_decode_args()");
						return e_failure;
					}
				}
				else
				{
					fprintf(stderr,"Error: Invalid option\n");
					printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
					return 1;
				}
		}
		else
		{
			printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
			return 1;
		}
		return 0;
}

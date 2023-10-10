// Author: Steven Bertolucci
// Course: CS 374/344 Operating System I
// Due Date: October 15, 2023
// File: base64enc.c
// Description: 
// -----------------------------------------------------------------------------------------------------
//       In this assignment, you will write a simple base64 encoding utility to familiarize 
//       yourself with the basics of C programming. You will be using, exclusively, a small 
//       subset of the C standard library to accomplish this task.
//
//       In this program, you will be working with arbitrary input data as raw bytes of a fixed 
//       width (8-bits), using bitwise arithmetic operations, as specified in the base64 
//       specification. You will want to read in data from the input file (or stdin) using a buffer 
//       (array) of uint8_t, which is a special unsigned 8-bit type provided by the <stdint.h> header.
//
//       You will need to perform bitwise operations on these raw bytes to calculate indices into the 
//       base64 alphabet, and then look up the corresponding characters in the alphabet to produce 
//       text for output.
// -----------------------------------------------------------------------------------------------------

#include <stdio.h>  // Standard input and output
#include <errno.h>  // Access to errno and Exxx macros
#include <stdint.h> // Extra fixed-width data types
#include <string.h> // String utilities
#include <err.h>    // Convenience functions for error reporting (non-standard)

static char const b64_alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "+/";

int main(int argc, char *argv[])
{
    FILE *newFile;                                                        /* Pointer to newFile */
    int num_requested = 3;
    int number_of_chars = 0;                                              /* Initialized to keep track of count later */

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
        errx(1, "Too many arguments");
    } else if (argc == 2 && strcmp(argv[1], "-") != 0) {
        newFile = fopen(argv[1], "r");                                    /* open FILE */
        if (!newFile) {
            err(1, "There is an error opening this file %s", argv[1]);    /* Display error message */
        }
    } else {
        newFile = stdin;                                                  /* use stdin instead */
    }
    for (;;) {
        uint8_t input_bytes[3] = {0};
        size_t n_read = fread(input_bytes, 1, num_requested, newFile);    /* Hold count for the number of characters in the file/stdin */

        if (n_read != 0) {
            /* Have data */
            int alph_ind[4];
            alph_ind[0] = input_bytes[0] >> 2;                                   /* Shift the first byte to the right by 2 */
            alph_ind[1] = (input_bytes[0] << 4 | input_bytes[1] >> 4) & 0x3Fu;   /* Shift the first byte to the left by 4 and the second byte to the right by 4 */
            alph_ind[2] = (input_bytes[1] << 2 | input_bytes[2] >> 6) & 0x3Fu;   /* Shift the second byte to the left by 2 and the third byte to the right by 6 */
            alph_ind[3] = input_bytes[2] & 0x3Fu;

            char output[5];
            output[0] = b64_alphabet[alph_ind[0]];

            // Loop through the array and encode the values usinng base64
            for (int i = 0; i < 4; i++) {
                if (i > num_requested) {
                    output[i] = '=';                                      /* Any unfilled bytes will be inserted with '=' */
                } else {
                    output[i] = b64_alphabet[alph_ind[i]];                /* Encode the data */
                }
            }
            
            output[4] = '\0';                                             /* After the fourth byte, insert null terminator */

            size_t n_write = fwrite(output, 1, 4, stdout);                /* Save the charcter for output */
            number_of_chars += n_write;                                   /* Increment count for each charcter written to output */

            // Check to see if there has been 76 characters written
            if (number_of_chars >= 76) {
              putchar('\n');                                              /* Break the line by inserting new line */         
              number_of_chars = 0;                                        /* Reset the count */
            }

            if (ferror(stdout)) {
              err(1, "Error with this file. Sorry");                      /* Write error */
            }
        }
        if (n_read < 3) {
            /* Got less than expected */
          putchar('\n');
            if (feof(newFile)) {
              break;                                                      /* End of file */
            }

            if (ferror(newFile)) {
                err(1, "Error with this file. Sorry");                    /* Read error */
            }
        }
    }
    if (newFile != stdin) {
        fclose(newFile);                                                  /* close opened files; */
    }

    /* any other cleanup tasks? */
    return 0;                                                             /* Program exit with code 0 */
}

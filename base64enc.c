// Author: Steven Bertolucci
// Course: CS 374/344
// Assignment: BASE64
// File: base64enc.c
// Due Date: October 15, 2023
// Description: 
// ------------------------------------------------------------------------------------------------             
//              This file focuses on Base64, which encode FILE, or standard input, 
//              and output to standard output.
//
//              In this program, you will be working 
//              with arbitrary input data as raw bytes of a fixed width (8-bits), using 
//              bitwise arithmetic operations, as specified in the base64 specification. 
//              You will want to read in data from the input file (or stdin) using a buffer 
//              (array) of uint8_t, which is a special unsigned 8-bit type provided by the 
//              <stdint.h> header.
//
//              You will need to perform bitwise operations on these raw bytes to calculate 
//              indices into the base64 alphabet, and then look up the corresponding characters 
//              in the alphabet to produce text for output.
// ------------------------------------------------------------------------------------------------              

#include <stdio.h>  // Standard input and output
#include <errno.h>  // Access to errno and Exxx macros
#include <stdint.h> // Extra fixed-width data types
#include <string.h> // String utilities
#include <err.h>    // Convenience functions for error reporting (non-standard)
#include <unistd.h> // For system call flags
#include <fcntl.h>  // For system call flags

// base64 alphabet so that we can translate our input bytes into output text
// {'A', 'B', 'C', ... }
static char const b64_alphabet[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789"
  "+/";

// Start of main aka the entry point of the program
int main(int argc, char *argv[])
{
    // base64 text file for writing
    FILE *newFile;
    int num_requested = 3;
    uint8_t input_bytes[3] = {0};

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
        errx(1, "Too many arguments");
    } else if (argc == 2 && strcmp(argv[1], "-")) {
        newFile = fopen(argv[1], "r");                                                    /* open FILE */
    } else {
        int newFile = fread(input_bytes, 1, num_requested, stdin);                        /* use stdin instead */
    }

    // The comments for the for loop, below, was taken from this Ed Discussion: edstem.org/us/courses/48016/discussion/3510524
    // because I couldn't understand what exactly this loop does. A student, Ali Alameedi, provided the comments
    // in response to another student that asked what the for loop does.
    
    for (;;) {                                                                     // Continue loop until we process all bytes

        uint8_t input_bytes[3] = {0};                                              // How many input bytes do we have?
        size_t n_read = fread(input_bytes, 1, num_requested, newFile);             // What is the number of bytes we are reading RIGHT NOW to process

        if (n_read != 0) {                                                         // As long as we have bytes to process
            /* Have data */
            int alph_ind[4];                                                       // Alpha index is calculating the index for our alphabet at the beginning of the program
            alph_ind[0] = input_bytes[0] >> 2;                                     // Shift the first byte by 2 bits (ie: 11111111 --> 00111111)
            alph_ind[1] = (input_bytes[0] << 4 | input_bytes[1] >> 4) & 0x3Fu;     // Shift the first byte by the right by 4 bytes and the second by 4 bytes to the left
                                                                                   // and add this equalizer to help us get an index to the alphabet array
            alph_ind[2] = (input_bytes[1] << 2 | input_bytes[2] >> 6) & 0x3Fu;     // Shift the second byte to the left by 2 and the third byte to the right by 6 bytes
            alph_ind[3] = input_bytes[2] & 0x3Fu;
            
            // Continue processing for remaining bytes
            char output[5];                                                         
            output[0] = b64_alphabet[alph_ind[0]];                                 // Get the letter that was encoded
            
            // For loop to store the base64 encoding
            for (int i = 0; i < 4; i++) {
              if (i > n_read) {
                output[i] = '=';
              } else {
                output[i] = b64_alphabet[alph_ind[i]];
              }
            }

            // Null Terminator
            output[4] = '\0';
         
            size_t n_write = fwrite(output, 1, 4, newFile);                       // Write the result to the file

            if (ferror(newFile)) err(1, "Error with this file. Sorry");           /* Write error */
        }

        if (n_read < num_requested) {
        /* Got less than expected */
        if (feof(newFile)) break;                                                 /* End of file */

        if (ferror(newFile)) err(1, "Error with this file. Sorry");               /* Read error */
        };
    }

    if (newFile != stdin) fclose(newFile);                                        /* close opened files; */

    /* any other cleanup tasks? */
    return 0;
}

#include <getopt.h> //Library used to parse command-line options of the Unix/POSIX style. POSIX is a family of standards for maintaining compatibility between
//operating systems.
#include <stdio.h> //Standard input output.
#include <stdlib.h> //Standard library.

#include "helpers.h" //For includying the functions declared in helpers.h.

int main(int argc, char *argv[]) //To accept command-line arguments.
{

    // Define allowable filters
    char *filters = "bgrs"; //Creates a variable called filters that is a pointer to a value of type char (the filter).  Tells the program that the allowable
    //command-line arguments to run the program are: b, g, r, s (blur, grayscale, reflection, sepia).

    // Get filter flag and check validity
    char filter = getopt(argc, argv, filters);
    if (filter == '?') //Check if the filter is a valid option -b, -g, -r, -s
    {
        fprintf(stderr, "Invalid filter.\n"); //the function fprintf sends formatted output to a stream.  In C Programming language, there are different file
        //descriptors which are also known as standard output. There are 3 standards I/O devices that are stdin for standard input, stdout for standard output,
        //stderr for error message output.
        return 1;
    }

    // Ensure only one filter
    if (getopt(argc, argv, filters) != -1)
    {
        fprintf(stderr, "Only one filter allowed.\n"); //the function fprintf sends formatted output to a stream.  In C Programming language, there are different
        //file descriptors which are also known as standard output. There are 3 standards I/O devices that are stdin for standard input, stdout for standard
        //output, stderr for error message output.
        return 2;
    }

    // Ensure proper usage
    if (argc != optind + 2) //Ensures that the filter option is entered after the name of the app: Ex: ./filter -g
    {
        fprintf(stderr, "Usage: filter [flag] infile outfile\n"); //the function fprintf sends formatted output to a stream.  In C Programming language, there
        //are different file descriptors which are also known as standard output. There are 3 standards I/O devices that are stdin for standard input, stdout for
        //standard output, stderr for error message output.
        return 3;
    }

    // Remember filenames
    char *infile = argv[optind];
    char *outfile = argv[optind + 1];

    // Open input file
    FILE *inptr = fopen(infile, "r");
    //Creates a variable called inptr that is a pointer to a value of type FILE.  fopen will open the input file (infile) and it will return a pointer to
    //infile in such way that then we can read information from.
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile); // If there is not file to read the program will return an error and a message of error.
        return 4;
    }

    // Open output file
    FILE *outptr = fopen(outfile, "w");
    //Creates a variable called outptr that is a pointer to a value of type FILE.  fopen will open the file (outfile)
    //and it will return a pointer to outfile in such way that then we can write information to.
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile  //If there is not file to write to, the program will return an error and a message of error.
        return 5;
    }

    // Read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 6;
    }

    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // Allocate memory for image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        fprintf(stderr, "Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);

        // Skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }

    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflection
        case 'r':
            reflect(height, width, image);
            break;

        // Sepia
        case 's':
            sepia(height, width, image);
            break;
    }

    // Write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // Free memory for image
    free(image);

    // Close infile
    fclose(inptr);

    // Close outfile
    fclose(outptr);

    return 0;
}

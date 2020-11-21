// Modifies the volume of an audio file

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Number of bytes in .wav header
const int HEADER_SIZE = 44; //The header is always 44 bytes long.

int main(int argc, char *argv[]) //The program accepts three command-line arguments.
{
    // Check command-line arguments
    if (argc != 4) //If one of the arguments is missed, the program will return an error and a message of error.
    {
        printf("Usage: ./volume input.wav output.wav factor\n");
        return 1;
    }

    // Open files and determine scaling factor
    FILE *input = fopen(argv[1], "r");
    //Creates a variable called input that is a pointer to a value of type FILE.  fopen will open the file (input.wav) located
    //in argv[1] and it will return a pointer to input.wav in such way that then we can read information from.
    if (input == NULL)
    {
        printf("Could not open file.\n"); //The only file that we have to test the program is called input.wav.  If there is not file to read (for example if we
        //execute the program with a name of a .wav file that doesn't exist, Eg. ./volume input1.wav output.wav 2.0, the program
        //will return an error and a message of error.
        return 1;
    }

    FILE *output = fopen(argv[2], "w");
    //Creates a variable called output that is a pointer to a value of type FILE.  fopen will open the file (output.wav)
    //located in argv[2] and it will return a pointer to output.wav in such way that then we can write information to.
    if (output == NULL)
    {
        printf("Could not open file.\n"); //If there is not file to write to, the program will return an error and a message of error.
        return 1;
    }

    float factor = atof(argv[3]);
    //Creates a variable called factor that converts a string into a floating point numerical representation.  Remember that the
    //command-line arguments are being stored in an array of characters and the syntaxis of the atof function is like the following:
    // double atof(const char *str)

    // TODO: Copy header from input file to output file
    uint8_t header[HEADER_SIZE]; //This is an array of bytes created to store the data from the wav file header that you will read from the input file. Using the unit8_t
    //type to represent a byte, you can create an array of n bytes.  Here it's not necessary to use pointers, because the name of an array is a
    //pointer itself.
    if (fread(header, sizeof(header), 1, input))
    {
        //This conditional means that if there is information to read from input, we need to save it in output, and
        //we will save it at the first position of the array of elements with type uint8_t which can store 44 bytes
        //This will copy the header on the output.
        fwrite(header, sizeof(header), 1, output);
    }


    // TODO: Read samples from input file and write updated data to output file
    int16_t buffer; //Each sample of audio is an int16_t value. In this variable (buffer) we are going to store the audio samples (one sample at a time) that we are
    //going to read from input.wav
    //In the next loop we are going the read the samples from the input file and we are going to store the samples in the location in memory of int16_t buffer
    //(2 bytes).
    //While there is something to read we are going to read it, process it and copy the modification in output (one at a time).
    while (fread(&buffer, sizeof(buffer), 1, input))
    {
        //read from input and store in the space where &buffer is pointing too.  That means that now we have
        //something stored in the variable buffer.
        double changedvolume = buffer * factor;
        buffer = (int16_t)changedvolume;
        //When we multiplied buffer*factor, we are multiplying a variable with the type int16_t (buffer) by a variable with the
        //the type float (factor).  This line converts the result of that multiplication into an int16_t type variable.  Now in the buffer we have the changed value
        //stored and we only need to copy in the output.

        fwrite(&buffer, sizeof(buffer), 1, output);
    }


    // Close files
    fclose(input); //Pointer function used to close the file.
    fclose(output); //Pointer function used to close the file.
}

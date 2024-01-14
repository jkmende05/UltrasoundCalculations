#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

float **createDataMatrix(int numElement, int numSample)
{
    // Create a double pointer of float type to store data from a .txt file
    float ** RFData = new float*[numElement];

    for (int i = 0; i < numElement; i++){
        RFData[i] = new float [numSample];
    }

    // Return data matrix
    return RFData;     
}

int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    // Open the text file fileName, read the data and store into RFData
    // You can use the getline() command to extract the data lines from the txt files

    // Open the .txt file
    ifstream filename (fileName);

    // Check if program successfully opened the file
    if (!filename.is_open()){
        // Return error message if file cannot be opened 
        return -1;
    }

    char temp[100];

    // Store data from .txt file into previously created 2D Data Matrix using getline command
    for (int i = 0; i < numElement; i++){
        for (int j = 0; j < numSample; j++){
            filename.getline(temp, 99);
            RFData[i][j] = atof(temp);
        }
    }

    return 0;
}

// Create an array containing the depth location (in z-direction) for each pixel on the scanline
float *genScanlineLocation(int &numPixel)
{
    // Initialize variable for scanline depth and get the user to input a value
    float scanlineDepth;

    cout << "Enter the scanline depth (in m) ";
    cin >> scanlineDepth;

    // Get user inputed value for the number of pixels
    cout << "Enter the number of pixels for the scanline: ";
    cin >> numPixel;

    // Declare a float type pointer and allocate memory
    float *scanlineLocation = new float [numPixel];

    // Calculate the distance between each scanline and store in array
    float increments = scanlineDepth / (numPixel - 1);

    for (int i = 0; i < numPixel; i++){
        // Store the location in scanlineLocation
        scanlineLocation[i] = (float)(i) * increments;
    }

    // Return array
    return scanlineLocation;

}
 
// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
    // Creates an array with a pointer to store transducer element location
   float *eleLocation = new float [numElement];

    // Calculate transducer element locations and store in eleLocation
   int N = numElement;
   for (int n = 0; n <= N - 1; n++){
        eleLocation[n] = ((float)(n)-(float)(N-1)/2) * PITCH;
   }

   return eleLocation;
}

// Allocate memory to store the beamformed scanline
float *createScanline(int numPixel)
{
    // Use pointer to allocate memory for calculations done in the beamform function
    float *scanline = new float [numPixel];

    return scanline;
} 

// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{
    // Declare pointer for floats and allocate memory for time forward calculations
    float *tForward = new float [numPixel];

    // Declare double pointer of float data type and allocate memory for tBackward and tTotal calculations 
    float **tBackward = new float *[numPixel];
    float **tTotal = new float *[numPixel];
    
    // Declare int double pointer and allocate memory for sample calculations
    int **sample = new int *[numPixel];

    // Declare pointer for floats to store pReal and pImag values
    float *pReal = new float [numPixel];
    float *pImag = new float [numPixel];

    // Iterate for each pointer and allocate memory to store calculations
    for (int i = 0; i < numPixel; i++){
        tBackward[i] = new float [numElement];
        tTotal[i] = new float [numElement];
        sample[i] = new int [numElement];

        // "Resets" values in pReal and pImag array for every time the program is run, works similarily to delete[] keyword
        pReal[i] = 0;
        pImag[i] = 0;
    }

    // Calculate magnitude and store in scanline array
    for (int i = 0; i < numPixel; i++){
        // Calculate time forward 
        tForward[i] = scanlinePosition[i] / SoS;

        for (int k = 0; k < numElement; k++){
            // Calculate and store total time by first calculating time backwards
            tBackward[i][k] = sqrt(pow(scanlinePosition[i], 2) + pow(elementPosition[k],2)) / SoS;
            tTotal[i][k] = tForward[i] + tBackward[i][k];

            // Calculate which sample from the kth transducer element matches with the ith scanline location
            sample[i][k] = floor (tTotal[i][k] * FS);

            // Calculate both the real and imaginary part of the pulse signal
            pReal[i] += realRFData[k][sample[i][k]];
            pImag[i] += imagRFData[k][sample[i][k]];

            // Calculate magnitudes and store
            float magnitude = sqrt(pow(pReal[i],2) + pow(pImag[i],2));

            scanline[i] = magnitude;
        }
    }  
}

// Write the scanline to a csv file
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{
    // Creates output file
    ofstream filename (fileName);

    // Check to ensure that creation of output file was successful
    if (!filename.is_open()){
        return -1;
    }

    // Transfers values in scanlinePosition and scanline arrays to output file
    for (int i = 0; i < numPixel; i++){
        filename << scanlinePosition[i] << "," << scanline[i] << endl;
    }   

    return 0;  
}

// Destroy all the allocated memory
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
   for (int i = 0; i < numElement; i++) {
    // Deletes the memory allocated for each float pointer for these double pointers
        delete[] realRFData[i];
        delete[] imagRFData[i];
    }

    // Delete memory allocated for each float pointer
    delete[] realRFData;
    delete[] imagRFData;

    delete[] scanline;
    delete[] scanlinePosition;
    delete[] elementPosition; 
} 

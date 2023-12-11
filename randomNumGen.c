
#include <time.h>
#include <stdlib.h>
#include "randomNumGen.h"

// Function to generate a random integer in the specified range [min, max]
int getRandomNumber(int min, int max) {
    // Seed the random number generator with the current time
    // srand(time(NULL));

    // Generate a random number in the range [min, max]
    int randomNumber = rand() % (max - min + 1) + min;

    return randomNumber;
}
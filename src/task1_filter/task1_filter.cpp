// Task 1 - Load a 512x512 image lena.raw
//        - Apply specified per-pixel transformation to each pixel
//        - Save as result.raw
#include <fstream>
#include <iostream>

using namespace std;

// Size of the framebuffer
const unsigned int SIZE = 512;

// A simple RGB struct will represent a pixel in the framebuffer
struct Pixel {
  // TODO: Define correct data type for r, g, b channel
  unsigned char red, green, blue;
};

int main()
{
  // Initialize a framebuffer
  auto framebuffer = new Pixel[SIZE][SIZE];

  // TODO: Open file lena.raw (this is 512x512 RAW GRB format)
  FILE *input = fopen("lena.raw", "rb");


  // TODO: Read data to framebuffer and close the file
  /*for(unsigned int i = 0; i < SIZE; i++) {
      for(unsigned int j = 0; j < SIZE; j++) {
          fread(&framebuffer[i][j], sizeof(Pixel), 1, input);
      }
  }*/

  fread(&framebuffer[0][0], sizeof(Pixel), SIZE*SIZE, input);

  fclose(input);

  // Traverse the framebuffer
  for (unsigned int y = 0; y < SIZE; y++) {
    for (unsigned int x = 0; x < SIZE; x++) {
      // TODO: Apply pixel operation
      // add noise
      float noise = (double)rand() / (double)((unsigned)RAND_MAX + 0.7) + 0.5;
      framebuffer[x][y].red *= noise;
      framebuffer[x][y].green *= noise;
      framebuffer[x][y].blue *= noise;


       /*
      // black and white
      float color = (framebuffer[x][y].red + framebuffer[x][y].green + framebuffer[x][y].blue) / 3;
      framebuffer[x][y].red = (unsigned char)color;
      framebuffer[x][y].green = (unsigned char)color;
      framebuffer[x][y].blue = (unsigned char)color;
       */
    }
  }

  // TODO: Open file result.raw
  std::cout << "Generating result.raw file ..." << std::endl;
  FILE *output = fopen("result.raw", "wb");

  // TODO: Write the framebuffer to the file and close it
  for(unsigned int i = 0; i < SIZE; i++) {
      for(unsigned int j = 0; j < SIZE; j++) {
          fwrite(&framebuffer[i][j], sizeof(Pixel), 1, output);
      }
  }
  fclose(input);

  std::cout << "Done." << std::endl;
  delete[] framebuffer;
  return EXIT_SUCCESS;
}

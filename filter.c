  #include <stdio.h>
  #include <stdlib.h>
  #include <stddef.h>
  #define STB_IMAGE_IMPLEMENTATION
  #include "stb_image/stb_image.h"
  #define STB_IMAGE_WRITE_IMPLEMENTATION
  #include "stb_image/stb_image_write.h"
int histt(void) {
    int width, height, channels;
    unsigned char *img = stbi_load("sky.jpg", &width, &height, &channels, 0);
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
     printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
     // stbi_write_png("sky.png", width, height, channels, img, width * channels);
     //stbi_write_jpg("sky2.jpg", width, height, channels, img, 100);

     // Convert the input image to gray
     size_t img_size = width * height * channels;
      int gray_channels = channels == 4 ? 2 : 1;
     size_t gray_img_size = width * height * gray_channels;
 
     unsigned char *gray_img = malloc(gray_img_size);
     if(gray_img == NULL) {
         printf("Unable to allocate memory for the gray image.\n");
         exit(1);
     }
     for(unsigned char *p = img, *pg = gray_img; p != img + img_size; p += channels, pg += gray_channels) {
         *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
         if(channels == 4) {
             *(pg + 1) = *(p + 3);
         }
     }
     stbi_write_jpg("sky_gray.jpg", width, height, gray_channels, gray_img, 100);
     //histogram
     unsigned char* pixels = stbi_load("sky_gray.jpg", &width, &height, &channels, 1);
    int histogram[256] = { 0 };
    int hist;
    for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        histogram[pixels[x + y * height]]++;

    }
  }
  return histogram;
  
     stbi_image_free(img);   
}
int main(void) {
    int *h=histt();
}

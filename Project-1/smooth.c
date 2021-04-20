#include <stdint.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#define CHANNEL_NUM 1


int main(int argc,char* argv[]) {
    int width, height, bpp;

	// Reading the image
    uint8_t* rgb_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);

    printf("Width: %d  Height: %d \n",width,height);
    
	for(int i=500;i<1500;i++){
		for(int j=500;j<1100;j++){
           //printf("%d \n",rgb_image[i*width + j]);
           rgb_image[i*width + j]=255;
		}
    }

    // Stoing the image 
    stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, rgb_image, 100);
    stbi_image_free(rgb_image);

    return 0;
}

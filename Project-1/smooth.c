#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#define CHANNEL_NUM 1

int main(int argc, char *argv[])
{
    int width, height, bpp;
    clock_t start, end;
    
    // Reading the image
    uint8_t *rgb_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);
    start = clock();
    printf("Width: %d  Height: %d \n", width, height);
    
    // matrix size -1 yatay kadar komşusu var.
    int matrix_size = 3;
    int constant = matrix_size - 1;
    int distance_of_center = (((matrix_size * 2) - 2)) / 4;
    int find_center = (width * distance_of_center) + distance_of_center;


    for (int i = 0; i < height - constant; i++)
    {
        int last_index_of_matrix = ((i * width) + constant) + constant * width;
        int summation = 0;
        for (int j = 0; j < width - constant; j++)
        {
            //printf("%d. 3x3  :", rgb_image[j + 1]);
            for (int k = 0; k <= constant; k++)
            {
                int neighboor = last_index_of_matrix + j - k;
                //printf(" %d ", rgb_image[neighboor]);
                summation += rgb_image[neighboor];
                for (int l = 0; l < constant; l++)
                {
                    // yukarı çıkarken her seferinde width kadar çıkmak için
                    summation += rgb_image[(neighboor) - (width * (l + 1))];
                    //printf(", %d ", rgb_image[(neighboor) - (width * (l + 1))]);
                }
            }
            int center = last_index_of_matrix + j - find_center;

            rgb_image[center] = summation / (matrix_size * matrix_size);
            summation = 0;
            //printf(" in center : %d summation result : %d",center, rgb_image[center]);

            //printf("\n");
        }
        //printf("\n");
    }
    end = clock();
    printf("Elapsed time %.2f seconds\n", (double)(end-start) * 10.0 /  CLOCKS_PER_SEC);
    // Stoing the image
    stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, rgb_image, 100);
    stbi_image_free(rgb_image);

    return 0;
}

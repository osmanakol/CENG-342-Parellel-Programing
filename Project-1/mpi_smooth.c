#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#define CHANNEL_NUM 1

void calculateSmooth(int local_height, int local_width, int matrix_size, uint8_t *arr)
{
    int constant = matrix_size - 1;
    int distance_of_center = (((matrix_size * 2) - 2)) / 4;
    int find_center = (local_width * distance_of_center) + distance_of_center;

    for (int i = 0; i <= local_height - constant; i++)
    {
        int last_index_of_matrix = ((i * local_width) + constant) + constant * local_width;
        int summation = 0;
        for (int j = 0; j <= local_width - constant; j++)
        {
            //printf("%d. 3x3  :", rgb_image[j + 1]);
            for (int k = 0; k <= constant; k++)
            {
                int neighboor = last_index_of_matrix + j - k;
                //printf(" %d ", rgb_image[neighboor]);
                summation += arr[neighboor];
                for (int l = 0; l < constant; l++)
                {
                    // yukarı çıkarken her seferinde width kadar çıkmak için
                    summation += arr[(neighboor) - (local_width * (l + 1))];
                    //printf(", %d ", rgb_image[(neighboor) - (local_width * (l + 1))]);
                }
            }
            int center = last_index_of_matrix + j - find_center;

            arr[center] = summation / (matrix_size * matrix_size);
            summation = 0;
            //printf(" in center : %d summation result : %d",center, rgb_image[center]);

            //printf("\n");
        }
        //printf("\n");
    }
}

int main(int argc, char *argv[])
{
    int width, height, bpp;
    uint8_t *rgb_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);
    MPI_Init(NULL, NULL);
    int id, number_of_core;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_core);
    clock_t local_start, local_finish; 
    double local_elapsed, elapsed; 
    // Reading the image
    MPI_Barrier(MPI_COMM_WORLD);
    local_start = clock();
    printf("\nProcess  %d Width: %d  Height: %d \n", id, width, height);
    uint8_t *recv_buff;

    int local_n = (height * width) / number_of_core;
    int local_width = width;
    int local_height = height / number_of_core;

    recv_buff = (uint8_t *)malloc(sizeof(uint8_t) * local_n);
    MPI_Scatter(rgb_image, local_n, MPI_UINT8_T, recv_buff, local_n, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    // matrix size -1 yatay kadar komşusu var.
    int matrix_size = atoi(argv[3]);
   

    calculateSmooth(local_height, local_width, matrix_size, recv_buff);

    uint8_t *new_image = (uint8_t *)malloc(sizeof(uint8_t) * width * height);
    MPI_Allgather(recv_buff, local_n, MPI_UINT8_T, new_image, local_n, MPI_UINT8_T, MPI_COMM_WORLD);
    local_finish = clock();
    local_elapsed = (double)(local_finish - local_start) * 10.0 / CLOCKS_PER_SEC;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    // Stoing the image
    if (id == 0)
    {
        printf("Elapsed Time  = %.2f seconds\n", elapsed);
        stbi_write_jpg(argv[2], width, height, CHANNEL_NUM, new_image, 100);
        stbi_image_free(new_image);
    }

    printf("Destroy process %d\n", id);
    MPI_Finalize();
    return 0;
}

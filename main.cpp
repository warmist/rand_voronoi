#define DF_IMPLEMENTATION
#include "df.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define VIRIDIS_IMPLEMENTATION
#include "viridis.h"

#include <vector>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

int main(){
    int i;
    int nx = 1920;
    int ny = 1080;
    int cx=nx/2;
    int cy=ny/2;
    int size_c=ny/2-200;
    FILE *fp;

    uint8_t *pixels = (uint8_t*)calloc(nx*ny, 3);
    float *squared_distances = (float*)malloc(nx*ny*sizeof(*squared_distances));
    df_point *closest_points = (df_point*)calloc(nx*ny, sizeof(*closest_points));

    std::vector<int> point_coords;
    float delta_angle=3.14159*2.0/100.0;
    for (i = 0; i < 100; i++){

        int x=cos(delta_angle*i)*size_c+cx;
        int y=sin(delta_angle*i)*size_c+cy;
        
        int j = x + y*nx;
        point_coords.push_back(j);
    }
    for(int iter=0;iter<=100;iter++)
    {
        /* Initialize distances to infinity. */
        for (i = 0; i < nx*ny; i++){
            squared_distances[i] = DF_INFINITY;
        }

        for (i = 0; i < point_coords.size(); i++){
            
            int j=point_coords[i];

            /* Other distance values than 0 and INFINITY work, too! */
            squared_distances[j] = 0.0f;
        }

        /* Calculate squared distance field. */
        /* Last parameter may be NULL if not needed. */
        df(squared_distances, nx, ny, NULL);

        int max_x=0;
        int max_y=0;
        float max_d=0;
        /* Draw distance field. */
        for (i = 0; i < nx*ny; i++){
            float d = sqrt(squared_distances[i]);
            if(d>max_d)
            {
                max_d=d;
                max_x=i%nx;
                max_y=i/nx;
            }
        }
        for (i = 0; i < nx*ny; i++){
            float d = sqrt(squared_distances[i])/max_d;
            unsigned char r,g,b;
            map_to_viridis(d,r,g,b);
            pixels[i*3+0] = r;
            pixels[i*3+1] = g;
            pixels[i*3+2] = b;
        }
        //if(iter%10 == 0)
        {
            char buffer[100];
            sprintf(buffer,"aaa%d.png",iter);
            stbi_write_png(buffer,nx,ny,3,pixels,nx*3);
        }
        printf("Max d:%g x=%d y=%d\n",max_d,max_x,max_y);

        auto pt=closest_points[max_x+max_y*nx];
        int r=rand()%333;
        if(r>222)
        {
            point_coords[pt.x+pt.y*nx]=max_x+max_y*nx;
        }
        else if(r>111)
        {
            point_coords[rand()%point_coords.size()]=max_x+max_y*nx;
        }
        else
        {
            point_coords.push_back(max_x+max_y*nx);
        }
    }
    /* Cleanup. */
    free(pixels);
    free(squared_distances);
    free(closest_points);
    return 0;
}
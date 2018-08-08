#include "mat.h"
#include "type.h"
#include "stdio.h"
#include "math.h"
#include "sift.h"
#include "dector.h"
#include "bmp.h"
#include <stdlib.h>

Mat* vertical_diff(Mat* image){
    if(image->channels != 1 || image->bytes != Float){
        fprintf(stderr,"channels or bytes isn't correct");
    }

    Mat* kernel = init_mat(3,3,0,Float);
    float* pointer = kernel->buffer;
    pointer[0] = -1;pointer[1] = -2;pointer[2] = -1;pointer[6] = 1;pointer[7] = 2;pointer[8] = 1;

    Mat* res = conv(image, kernel, 1, 1);
    return res;
}

Mat* gaussian_kernel(U8 radius, float sigma){
    U16 height = 2*radius + 1;
    U16 width = 2*radius + 1;
    Mat* kernel = init_mat(height, width, 0,Float);
    float* pointer;

    int row, col;
    for(row = 0; row<height;row++){
        for(col = 0; col < width; col++){
            pointer = locate(kernel, row, col);
            float r = sqrt(pow(row - radius,2) + pow(col - radius,2))/radius;
            *pointer = gaussian(sigma, r);
        }
    }

    Mat* kernel = init_mat(3,3,0,Float);
    float* pointer = kernel->buffer;
    pointer[0] = -1;pointer[3] = -2;pointer[6] = -1;pointer[2] = 1;pointer[5] = 2;pointer[8] = 1;

    Mat* res = conv(image, kernel, 1, 1);
    return res;
}

Mat* gradient_abs(Mat* image){
    if(image->channels != 1 || image->bytes != Float){
        fprintf(stderr,"channels or bytes isn't correct");
    }
    U8 threshold = 1;
    U8 is_extreme = true;

    float* origin = locate(scala_space[level], row, col);
    /*check maxmum*/
    U8 k;
    U16 i, j;
    for(k = level -1;k < level + 2; k++){
        for(i = row - 1;i < row + 2;i++){
            for(j = col - 1;j < col + 2;j++){
                if(i == row && j == col && k == level)
                    continue;
                float* compare = locate(scala_space[k], i, j);
                if(*compare > *origin - threshold)
                    //return false;
                    is_extreme = false;
            }
        }
    }
    if(is_extreme == false)
        is_extreme = true;
    else
        return is_extreme;
    /*check minmum*/
    for(k = level -1;k < level + 2; k++){
        for(i = row - 1;i < row + 2;i++){
            for(j = col - 1;j < col + 2;j++){
                if(i == row && j == col && k == level)
                    continue;
                float* compare = locate(scala_space[k], i, j);
                if(*compare < *origin + threshold)
                    is_extreme = false;
            }
        }
        if(is_extreme == false)
            break;
    }
    Mat* h_diff = horizon_diff(image);
    Mat* v_diff = vertical_diff(image);
    U16 height = image->height;
    U16 width = image->width;
    Mat* abs_diff = init_mat(height, width, 0, Float);

    for(U16 row=0;row<height;row++){
        for(U16 col=0;col<width;col++){
            float* src1 = locate(h_diff, row, col);
            float* src2 = locate(v_diff, row, col);
            float* to = locate(abs_diff, row, col);

List* local_max(Mat** scala_space){
    U16 length = 4;
    List* key_points = init_List(sizeof(Point));
    U16 height = scala_space[0]->height;
    U16 width = scala_space[0]->width;
    U8 k;
    U16 row, col;
    for(k = 1; k < length - 1; k++){
        for(row = 1; row< height-1; row++){
            for(col=1; col < width-1; col++){
                if(check_extreme(scala_space, k,row,col))
                    push(key_points,init_point(row,col));
            }
        }
    }
    return abs_diff;
}
float main_direction(){
    return 0;
}

Mat* get_dog_kernel(U8 radius, float sigma1, float sigma2){
    Mat* kernel = image_sub(gaussian_kernel(radius,sigma1),
                     gaussian_kernel(radius,sigma2));
    float sum = 0;
    U16 row, col;
    for(row = 0; row < 2*radius + 1; row++){
        for(col=0; col < 2*radius + 1; col++){
            float* pointer = locate(kernel, row, col);
            sum = sum + *pointer;
        }
    }
}

List* Dog(Mat* image){
    float sigma = 0.3;
    int scale = 4;
    U8 radius = 2;

    Mat** scale_space = malloc(sizeof(Mat*));
    int k;
    for(k = 1; k <scale+1; k++){
        Mat* dog_kernel = get_dog_kernel(radius,sigma*2,sigma);
        sigma = sigma*2;
        print_mat(dog_kernel);
        scale_space[k-1] = conv(image, dog_kernel, 1, dog_kernel->height/2); // keep the scale is the same as the origin image
    }
    return sift;
}

List* sift_describe(Mat* image, List* key_points){
    static num = 1;
    List* sift_list = init_List(sizeof(SiftVector));
    Mat* abs_diff = gradient_abs(image);
    Mat* direction_diff= gradient_direction(image);

    int i;
    for(i = 0; i < 4; i++){
        normalize_image(scale_space[i]);
        Mat* img = float2uchar(scale_space[i]);
        char path[20];
        sprintf(path,"level-%d.bmp",(i+1));
        write_bmp(img, path);
    }
    num++;

    normalize_image(abs_diff);
    Mat* img = float2uchar(abs_diff);
    char path[20];
    sprintf(path,"diff_abs%d.bmp",num);
    write_bmp(img,path);
    free_mat(img);

    normalize_image(direction_diff);
    img = float2uchar(direction_diff);
    sprintf(path,"direction_diff%d.bmp",num);
    write_bmp(img,path);
    free_mat(img);
    free_mat(abs_diff);
    free_mat(direction_diff);

    return sift_list;
}

float vector_dot(SiftVector sift1, SiftVector sift2){
    float dot_sum = 0;
    float sum1 = 0;
    float sum2 = 0;
    for(U8 i=0;i<128;i++){
        dot_sum = dot_sum + sift1[i]*sift2[i];
        sum1 = sum1 + pow(sift1[i],2);
        sum2 = sum2 + pow(sift2[i],2);
    }
    RGB red;
    red.G = 255;
    U16 height = color_image->height;
    U16 width = color_image->width;

    Node* pointer;
    for(pointer = key_points->start; pointer != NULL; pointer = pointer->next){
        Point* point = pointer->data;
        RGB* to = locate(color_image, point->row, point->col);
        if(point->row > 1 && point->row < height- 2
                && point->col > 1&& point->col < width -2){
            *to = red;
            *(to - 1) = red;
            *(to + 1) = red;
            *(to + width) = red;
            *(to - width) = red;
        }
        push(match_list,match_pair);
        l1 = l1->next;
    }
    return match_list;
}

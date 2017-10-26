#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/timeb.h>
#include <OpenCL/opencl.h>
#include "util.h"
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#include "mandelbrot.cl.h"

int local_work_items = 0;
float avg_local_work_items = 0;
int local_group_size[] = {1, 2, 4, 8, 16 };
int threads;
float avg_threads;

int main(int argc, char** argv) {
    double times[REPEAT];
    struct timeb start, end;
    int r;
    char path[255];
    
    unsigned char* colormap = (unsigned char*)malloc(sizeof(cl_uchar) * (MAX_ITERATION+1) * 3);
    unsigned char* image = (unsigned char*)malloc(sizeof(cl_uchar) * WIDTH * HEIGHT * 4);
    
    init_colormap(MAX_ITERATION, colormap);
    
    void* colormap_input = gcl_malloc(sizeof(cl_uchar) * (MAX_ITERATION+1) * 3 , colormap, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    void* image_output = gcl_malloc(sizeof(cl_uchar) * WIDTH * HEIGHT * 4, NULL, CL_MEM_WRITE_ONLY);
    for (r = 0; r < REPEAT; r++) {
        memset(image, 0, WIDTH * HEIGHT * 4);
        
        ftime(&start);
        
        /* BEGIN: Thread-based implementation */
        /* Replace it with a GPU implementation. Modify in util.c the
         progress(), description(), and report() functions to report
         details specific to your GPU implementation (eg number of
         blocks, numer of threads, optimization approach, etc */
        dispatch_queue_t dq = gcl_create_dispatch_queue(CL_DEVICE_TYPE_GPU, NULL);
        if (!dq) {
            fprintf(stdout, "Unable to create a GPU-based dispatch queue.\n");
            exit(1);
        }
        dispatch_sync(dq, ^{
            local_work_items = WIDTH / local_group_size[r/2];
            threads = local_group_size[r/2];
            cl_ndrange range = {
                2,                        // 2 dimensions for image
                {0},                      // Start at the beginning of the range
                {HEIGHT, WIDTH},          // Execute width * height work items
                { threads, threads, 0}    // Split local work items
            };
            
            //Execute the kernel
            mandelbrot_kernel(&range, (cl_uchar*)image_output, (cl_uchar*)colormap_input, (cl_int)WIDTH, (cl_int)HEIGHT, (cl_int)MAX_ITERATION);
            
            // Copy image output
            gcl_memcpy(image, image_output, sizeof(cl_uchar) * WIDTH * HEIGHT * 4);
        });
        
        /* END: Thread-based implementation */
        
        ftime(&end);
        times[r] = end.time - start.time + ((double)end.millitm - (double)start.millitm)/1000.0;
        
        sprintf(path, IMAGE, "gpu", r);
        save_image(path, image, WIDTH, HEIGHT);
        progress("gpu", r, times[r], local_work_items, threads);
        avg_local_work_items = avg_local_work_items + local_work_items;
        avg_threads = avg_threads + threads;
        dispatch_release(dq);
    }
    report("gpu", times, avg_local_work_items / REPEAT, avg_threads / REPEAT);
    
    gcl_free(image_output);
    gcl_free(colormap_input);
    free(image);
    free(colormap);
    return 0;
}


kernel void mandelbrot(global unsigned char* image, global unsigned char* colormap, int width, int height, int max) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    float c_re, c_im, x_new, x, y;
    c_re = (col - width / 2.0)*4.0 / width;
    c_im = (row - height / 2.0)*4.0 / width;
    int iteration = 0;
    x=0; y=0;
    while (x*x + y*y <= 4 && iteration < max) {
        x_new = x*x - y*y + c_re;
        y = 2 * x*y + c_im;
        x = x_new;
        iteration++;
    }
    if (iteration > max) {
        iteration = max;
    }
    int i = iteration * 3;
    image[4 * width * row + 4 * col + 0] = colormap[i];
    image[4 * width * row + 4 * col + 1] = colormap[i + 1];
    image[4 * width * row + 4 * col + 2] = colormap[i + 2];
    image[4 * width * row + 4 * col + 3] = 255;
}

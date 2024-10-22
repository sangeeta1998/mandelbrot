// mandelbrot.cpp
#include <iostream>
#include <complex>
#include <vector>
#include <chrono>
#include <png.h>

using namespace std;

int mandelbrot(complex<double> c, int max_iter) {
    complex<double> z = 0;
    for (int n = 0; n < max_iter; n++) {
        if (abs(z) > 2) return n;
        z = z * z + c;
    }
    return max_iter;
}

void mandelbrot_set(double xmin, double xmax, double ymin, double ymax, int width, int height, int max_iter, vector<vector<int>>& image) {
    complex<double> c;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            c = xmin + (xmax - xmin) * x / width + (ymin + (ymax - ymin) * y / height) * complex<double>(0, 1);
            image[x][y] = mandelbrot(c, max_iter);
        }
    }
}

void save_image(const vector<vector<int>>& image, int width, int height) {
    FILE *fp = fopen("/output/mandelbrot.png", "wb");
    if (!fp) {
        cerr << "Error: could not open file for writing" << endl;
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    if (setjmp(png_jmpbuf(png))) {
        cerr << "Error during png creation" << endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // Write image data
    for (int y = 0; y < height; y++) {
        png_bytep row = (png_bytep) malloc(width);
        for (int x = 0; x < width; x++) {
            row[x] = static_cast<png_byte>(image[x][y] % 256); // Scale value to 0-255
        }
        png_write_row(png, row);
        free(row);
    }

    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    cout << "Image saved as mandelbrot.png in /output directory" << endl;
}

void generate_mandelbrot_image(int image_size, int max_iter) {
    auto start = chrono::high_resolution_clock::now();
    cout << "Main function started at: " << chrono::duration_cast<chrono::milliseconds>(start.time_since_epoch()).count() << " ms" << endl;
    cout << "Starting Mandelbrot set generation..." << endl;

    vector<vector<int>> image(image_size, vector<int>(image_size));
    mandelbrot_set(-2.0, 1.0, -1.5, 1.5, image_size, image_size, max_iter, image);
    save_image(image, image_size, image_size);

    auto end = chrono::high_resolution_clock::now();
    double execution_time = chrono::duration<double>(end - start).count();
    cout << "Execution Time: " << execution_time << " seconds" << endl;
}

int main() {
    generate_mandelbrot_image(1024, 500);
    return 0;
}


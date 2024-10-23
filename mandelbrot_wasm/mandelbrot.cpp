#include <iostream>
#include <chrono>
#include <cstdint>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Function to get the current time in milliseconds
uint64_t current_time_millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// Function to generate the Mandelbrot set
void generate_mandelbrot(int width, int height, int max_iterations, const char* filename) {
    std::vector<uint8_t> image(width * height * 3); // RGB image

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double real = (x - width / 2.0) * 4.0 / width; // Scale to [-2, 2]
            double imag = (y - height / 2.0) * 4.0 / height; // Scale to [-2, 2]
            double c_real = real;
            double c_imag = imag;

            int iterations = 0;
            while (iterations < max_iterations && (real * real + imag * imag) < 4.0) {
                double temp_real = real * real - imag * imag + c_real;
                imag = 2.0 * real * imag + c_imag;
                real = temp_real;
                ++iterations;
            }

            // Color based on the number of iterations
            uint8_t color = static_cast<uint8_t>(255 * iterations / max_iterations);
            image[(y * width + x) * 3 + 0] = color; // R
            image[(y * width + x) * 3 + 1] = color; // G
            image[(y * width + x) * 3 + 2] = color; // B
        }
    }

    // Write the image to a file
    stbi_write_png(filename, width, height, 3, image.data(), width * 3);
}

int main() {
    const int width = 1024;
    const int height = 1024;
    const int max_iterations = 500;
    const char* filename = "/output/mandelbrot.png";

    // Record the start time
    auto start_time = std::chrono::high_resolution_clock::now();
    std::cout << "Main function started at: " << std::chrono::duration_cast<std::chrono::milliseconds>(start_time.time_since_epoch()).count() << " ms" << std::endl;

    // Generate the Mandelbrot set
    generate_mandelbrot(width, height, max_iterations, filename);

    // Record the end time
    auto end_time = std::chrono::high_resolution_clock::now();

    // Print the execution time
    std::cout << "Execution Time: " << std::chrono::duration<double>(end_time - start_time).count() * 1000 << " ms" << std::endl;
    std::cout << "Image saved as " << filename << std::endl;

    return 0;
}


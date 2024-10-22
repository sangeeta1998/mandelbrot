Generates the Mandelbrot set. It takes parameters for the image dimensions (width, height), the maximum number of iterations to determine if a point is in the set (max_iterations), and the filename for saving the image.
It creates a 1D array (image) to hold the RGB values for each pixel.

A nested loop iterates through each pixel in the image.
It maps the pixel coordinates (x, y) to complex numbers c = c_real + i*c_imag within the range [-2, 2] for both the real and imaginary parts.

Another loop checks if the current complex number escapes to infinity. If it does not escape (i.e., the squared magnitude is less than 4) for the maximum number of iterations, the point is considered part of the Mandelbrot set.
The escape condition is based on the iterative function defined by z=z^2+c. The pixel color is determined by the number of iterations it took to escape. The more iterations before escaping, the darker the pixel (since color is computed as a fraction of the max_iterations). save the generated image in PNG format with the specified filename.


```native```

docker buildx build --platform linux/amd64,linux/arm64 --output "type=image,push=true" --tag sangeetakakati/mandelbrot-native:latest .

docker run --rm -v "$(pwd):/output" sangeetakakati/mandelbrot-native:latest

```wasm```

docker buildx build --platform wasm --tag sangeetakakati/mandelbrot-wasm:wasm --output "type=image,push=true" .

docker run --runtime=io.containerd.wasmtime.v1 --platform=wasm --rm -v "$(pwd):/output" sangeetakakati/mandelbrot-wasm:wasm

wasm-opt -O4 --strip-debug -o mandelbrot_opt.wasm mandelbrot.wasm

clang++ --target=wasm32-wasi -fno-exceptions -o mandelbrot.wasm mandelbrot.cpp


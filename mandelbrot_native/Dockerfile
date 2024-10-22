FROM alpine:latest AS builder
WORKDIR /usr/src/app
RUN apk add --no-cache clang make libc-dev

COPY mandelbrot.cpp ./mandelbrot.cpp
COPY stb_image_write.h ./stb_image_write.h
RUN clang++ mandelbrot.cpp -o mandelbrot -O3

# Runtime Stage
FROM alpine:latest
WORKDIR /output
RUN apk add --no-cache libstdc++
COPY --from=builder /usr/src/app/mandelbrot /mandelbrot
CMD ["/mandelbrot"]

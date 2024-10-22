FROM alpine:latest AS builder
WORKDIR /usr/src/app
RUN apk add --no-cache clang make libc-dev libpng-dev

COPY mandelbrot.cpp ./mandelbrot.cpp
RUN clang++ mandelbrot.cpp -o mandelbrot -O3 -lpng

# Runtime Stage
FROM alpine:latest  
WORKDIR /output  
RUN apk add --no-cache libpng libstdc++ 
COPY --from=builder /usr/src/app/mandelbrot /mandelbrot
CMD ["/mandelbrot"]


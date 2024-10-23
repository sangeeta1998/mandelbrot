#!/bin/bash

# Function to detect system architecture
detect_architecture() {
    local arch=$(uname -m)
    case $arch in
        x86_64)
            echo "amd64"
            ;;
        aarch64)
            echo "arm64"
            ;;
        *)
            echo "Unsupported architecture: $arch"
            exit 1
            ;;
    esac
}

# Function to measure the time until the main function starts
measure_main_function_start_time() {
    image=$1
    runtime=$2
    platform=$3
    fresh_pull=$4
    iterations=$5

    echo -e "\n============================================"
    echo "Running $image with $runtime (Fresh Pull: $fresh_pull) for $iterations iterations"
    echo "--------------------------------------------"

    total_pull_time=0
    total_start_time=0

    for (( i=1; i<=iterations; i++ )); do
        echo "Iteration $i:"

        if [ "$fresh_pull" = true ]; then
            # Force remove the image before pulling
            echo "Removing image $image to force fresh pull..."
            docker rmi -f $image 2>/dev/null || true

            # Measure image pull time
            start_time=$(date +%s%N)
            if [[ $image == *"wasm"* ]]; then
                docker pull --platform wasm $image
            else
                docker pull $image
            fi
            end_time=$(date +%s%N)
            image_pull_time=$((($end_time - $start_time) / 1000000))
            echo "Image Pull Time (Iteration $i): $image_pull_time ms"
        else
            image_pull_time=0
            echo "Skipping image pull for cached image: $image"
        fi

        # Measure container startup time until "Main function started"
        start_time=$(date +%s%N)
        if [ -z "$runtime" ]; then
            # For native containers, capture the output
            main_start_time=$(docker run --rm -v "$(pwd):/output" $image 2>&1 | grep "Main function started at:" | awk '{print $5}')
        else
            # For Wasm containers with specific runtime
            main_start_time=$(docker run --runtime=$runtime --platform=$platform --rm -v "$(pwd):/output" $image 2>&1 | grep "Main function started at:" | awk '{print $5}')
        fi
        end_time=$(date +%s%N)

        if [ -z "$main_start_time" ]; then
            echo "Main function start message not found!"
        else
            echo "Main function started at: $main_start_time"
        fi

        container_execution_time=$((($end_time - $start_time) / 1000000))
        echo "Container Execution Time Until Main Start (Iteration $i): $container_execution_time ms"

        # Accumulate the times for averaging
        total_pull_time=$(($total_pull_time + $image_pull_time))
        total_start_time=$(($total_start_time + $container_execution_time))
    done

    # Calculate averages
    avg_pull_time=$(($total_pull_time / $iterations))
    avg_start_time=$(($total_start_time / $iterations))

    echo -e "\n============================================"
    echo "Summary for $image:"
    echo "Average Image Pull Time: $avg_pull_time ms"
    echo "Average Time Until Main Function Start: $avg_start_time ms"
    echo -e "============================================\n"

    # Log total execution time
    echo "Image: $image" 
    echo "Average Pull Time: $avg_pull_time ms" 
    echo "Average Time Until Main Start: $avg_start_time ms" 
    echo "--------------------------------------------" 
}

# Detect current system architecture
arch=$(detect_architecture)

# Define image names
rust_native_image="sangeetakakati/mandelbrot-native:latest"
wasm_image="sangeetakakati/mandelbrot-wasm:wasm"

# Set the number of iterations for each test
iterations=15

# Measure execution time until main function starts with forced fresh pull
echo -e "\nTesting with forced fresh pull:"
measure_main_function_start_time "$rust_native_image" "" "$arch" true $iterations
measure_main_function_start_time "$wasm_image" "io.containerd.wasmtime.v1" "wasm" true $iterations

# Measure execution time using cached images (skip removal)
echo -e "\nTesting with cached images:"
if [[ "$(docker images -q $rust_native_image 2> /dev/null)" != "" ]]; then
    echo "Using cached image for $rust_native_image"
    measure_main_function_start_time "$rust_native_image" "" "$arch" false $iterations
else
    echo "Image not found in cache: $rust_native_image"
fi

if [[ "$(docker images -q $wasm_image 2> /dev/null)" != "" ]]; then
    echo "Using cached image for $wasm_image"
    measure_main_function_start_time "$wasm_image" "io.containerd.wasmtime.v1" "wasm" false $iterations
else
    echo "Image not found in cache: $wasm_image"
fi


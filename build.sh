#!/bin/bash

echo "Building Elysia Settings C++ Application..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Navigate to build directory
cd build

# Clean previous build
echo "Cleaning previous build..."


# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Check if cmake was successful
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the application
echo "Building application..."
make -j$(nproc)

# Check if make was successful
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo "You can now run the application with: ./build/SETTING_APP_CPP" 
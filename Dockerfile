FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libglib2.0-dev \
    libcjson-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the source code
COPY . .

# Build the application
RUN mkdir -p build && cd build && cmake .. && cmake --build .

# Expose the server port
EXPOSE 8888

# Run the application
CMD ["./build/baandb"]

# ==========================================
# Stage 1: Build Stage
# ==========================================
FROM gcc:12 AS builder

WORKDIR /app

# Install wget to fetch the TensorFlow C API library
RUN apt-get update && apt-get install -y wget && rm -rf /var/lib/apt/lists/*

# Download and extract TensorFlow C API for Linux (CPU version)
RUN wget -q https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-2.14.0.tar.gz \
    && tar -C /usr/local -xzf libtensorflow-cpu-linux-x86_64-2.14.0.tar.gz \
    && rm libtensorflow-cpu-linux-x86_64-2.14.0.tar.gz \
    && ldconfig

# Copy source code and headers
COPY CppWebServer.cpp Algorithm.cpp tensorFlowApp.cpp tictactoe.cpp ./
COPY include/ ./include/

# Copy configuration files and model directories if they exist locally
COPY tensorflow.ini Algorithm.ini ./ 
COPY tictactoe_tf_model/ ./tictactoe_tf_model/

# Compile the project for Linux (linking -lpthread instead of Windows -lws2_32)
RUN g++ -std=c++17 -O3 CppWebServer.cpp Algorithm.cpp tensorFlowApp.cpp tictactoe.cpp \
    -Iinclude -L/usr/local/lib -ltensorflow \
    -o cpp_web_server \
    -lpthread

# ==========================================
# Stage 2: Lightweight Runtime Stage
# ==========================================
FROM ubuntu:22.04

WORKDIR /app

# Install necessary runtime dependencies (like OpenMP for TensorFlow)
RUN apt-get update && apt-get install -y libgomp1 && rm -rf /var/lib/apt/lists/*

# Copy TensorFlow C shared libraries from the builder stage
COPY --from=builder /usr/local/lib/libtensorflow*.so* /usr/local/lib/
RUN ldconfig

# Copy artifacts and assets from the builder stage
COPY --from=builder /app/cpp_web_server /app/cpp_web_server
COPY --from=builder /app/tensorflow.ini /app/tensorflow.ini
COPY --from=builder /app/Algorithm.ini /app/Algorithm.ini
COPY --from=builder /app/tictactoe_tf_model /app/tictactoe_tf_model

# Expose the web server port
EXPOSE 8080

# Run the compiled web server executable
CMD ["./cpp_web_server"]
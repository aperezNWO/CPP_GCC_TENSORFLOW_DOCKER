# ==========================================
# Stage 1: Build Stage
# ==========================================
FROM gcc:12 AS builder

WORKDIR /app

# Install wget
RUN apt-get update && apt-get install -y wget && rm -rf /var/lib/apt/lists/*

# Download and extract TensorFlow C API (Cached permanently unless this layer changes)
RUN wget -q https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-2.14.0.tar.gz \
    && tar -C /usr/local -xzf libtensorflow-cpu-linux-x86_64-2.14.0.tar.gz \
    && rm libtensorflow-cpu-linux-x86_64-2.14.0.tar.gz \
    && ldconfig

# Copy headers and source code files
COPY include/ ./include/
COPY CppWebServer.cpp TFileManager.cpp TFileManager.h Algorithm.cpp tensorFlowApp.cpp tictactoe.cpp Sudoku.cpp SortBenchMark.cpp SortBenchMark.h Dijkstra.cpp Dijkstra.h RegExManager.cpp RegExManager.h FractalEngine.cpp ./
COPY tensorflow.ini Algorithm.ini CdCatalog.xml ./ 
COPY tictactoe_tf_model/ ./tictactoe_tf_model/

# Compile the project for Linux
RUN g++ -std=c++17 -O3 CppWebServer.cpp TFileManager.cpp Algorithm.cpp tensorFlowApp.cpp tictactoe.cpp Sudoku.cpp SortBenchMark.cpp Dijkstra.cpp  RegExManager.cpp FractalEngine.cpp \
    -Iinclude -L/usr/local/lib -ltensorflow \
    -o cpp_web_server \
    -lpthread

# ==========================================
# Stage 2: Lightweight Runtime Stage
# ==========================================
FROM ubuntu:22.04

WORKDIR /app

RUN apt-get update && apt-get install -y libgomp1 && rm -rf /var/lib/apt/lists/*

COPY --from=builder /usr/local/lib/libtensorflow*.so* /usr/local/lib/
RUN ldconfig

COPY --from=builder /app/cpp_web_server /app/cpp_web_server
COPY --from=builder /app/tensorflow.ini /app/tensorflow.ini
COPY --from=builder /app/Algorithm.ini /app/Algorithm.ini
COPY --from=builder /app/CdCatalog.xml /app/CdCatalog.xml
COPY --from=builder /app/tictactoe_tf_model /app/tictactoe_tf_model

EXPOSE 8080
CMD ["./cpp_web_server"]
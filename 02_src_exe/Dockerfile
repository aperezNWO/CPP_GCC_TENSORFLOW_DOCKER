# Stage 1: Build the C++ binary
FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    make \
#   libtesseract-dev \
#   tesseract-ocr-eng \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Added -Iinclude so the compiler can locate the include directory properly
RUN g++ -std=c++23 -O3 CppWebServer.cpp Algorithm.cpp -I. -Iinclude -o cpp_web_server -lpthread

# Stage 2: Runtime image
FROM debian:bookworm-slim
WORKDIR /app

#RUN apt-get update && apt-get install -y \
#    libtesseract5 \
#    tesseract-ocr-eng \
#    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/tensorflow.ini  /app/tensorflow.ini
COPY --from=builder /app/cpp_web_server  /app/cpp_web_server

EXPOSE 8080
CMD ["./cpp_web_server"]
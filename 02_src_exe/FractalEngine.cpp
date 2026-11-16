#include <vector>
#include <cmath>
#include <random>
#include <string>
#include <stdexcept>
#include <sstream>

// --- Bounds Structure ---
struct Bounds {
  double xMin;
  double xMax;
  double yMin;
  double yMax;
};

// --- Fractal Kind Enum ---
enum class FractalKind {
  mandelbrot = 1,
  julia = 2,
  leaf = 3
};

// --- Fractal Point Structure ---
struct FractalPoint {
  double x;
  double y;
  int intensity;

  std::string toJson() const {
    std::ostringstream ss;
    ss << "{\"x\":" << x << ",\"y\":" << y << ",\"intensity\":" << intensity << "}";
    return ss.str();
  }
};

// --- Fractal Engine Class ---
class FractalEngine {
public:
  static const int CANVAS_WIDTH = 800;
  static const int CANVAS_HEIGHT = 600;

  std::vector<FractalPoint> getFractal(FractalKind fractalKind, Bounds bounds, int maxIterations) {
    switch (fractalKind) {
      case FractalKind::mandelbrot:
        return generateMandelbrot(bounds, maxIterations);
      case FractalKind::julia:
        return generateJulia(bounds, maxIterations);
      case FractalKind::leaf:
        return generateLeaf();
    }
    return {};
  }

private:
  static int _encodeIntensity(int iter, int maxIterations) {
    return (iter == maxIterations) ? 0 : (iter * 255 / maxIterations);
  }

  std::vector<FractalPoint> generateMandelbrot(Bounds bounds, int maxIterations) {
    std::vector<FractalPoint> points;
    double xRange = bounds.xMax - bounds.xMin;
    double yRange = bounds.yMax - bounds.yMin;

    for (int screenY = 0; screenY < CANVAS_HEIGHT; screenY++) {
      for (int screenX = 0; screenX < CANVAS_WIDTH; screenX++) {
        double cRe = bounds.xMin + (screenX * xRange / CANVAS_WIDTH);
        double cIm = bounds.yMin + (screenY * yRange / CANVAS_HEIGHT);
        double zRe = 0.0, zIm = 0.0;
        int iter = 0;
        while (zRe * zRe + zIm * zIm <= 4.0 && iter < maxIterations) {
          double nextRe = zRe * zRe - zIm * zIm + cRe;
          double nextIm = 2.0 * zRe * zIm + cIm;
          zRe = nextRe;
          zIm = nextIm;
          iter++;
        }
        points.push_back({static_cast<double>(screenX), static_cast<double>(screenY), _encodeIntensity(iter, maxIterations)});
      }
    }
    return points;
  }

  std::vector<FractalPoint> generateJulia(Bounds bounds, int maxIterations) {
    std::vector<FractalPoint> points;
    double xRange = bounds.xMax - bounds.xMin;
    double yRange = bounds.yMax - bounds.yMin;
    const double cRe = -0.400;
    const double cIm = 0.600;

    for (int screenY = 0; screenY < CANVAS_HEIGHT; screenY++) {
      for (int screenX = 0; screenX < CANVAS_WIDTH; screenX++) {
        double zRe = bounds.xMin + (screenX * xRange / CANVAS_WIDTH);
        double zIm = bounds.yMin + (screenY * yRange / CANVAS_HEIGHT);
        int iter = 0;
        while (zRe * zRe + zIm * zIm <= 4.0 && iter < maxIterations) {
          double nextRe = zRe * zRe - zIm * zIm + cRe;
          double nextIm = 2.0 * zRe * zIm + cIm;
          zRe = nextRe;
          zIm = nextIm;
          iter++;
        }
        points.push_back({static_cast<double>(screenX), static_cast<double>(screenY), _encodeIntensity(iter, maxIterations)});
      }
    }
    return points;
  }

  std::vector<FractalPoint> generateLeaf() {
    std::vector<FractalPoint> points;
    std::vector<std::vector<int>> pixelGrid(CANVAS_WIDTH, std::vector<int>(CANVAS_HEIGHT, 0));
    double x = 0.0, y = 0.0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 99);

    for (int i = 0; i < 150000; i++) {
      double nextX, nextY;
      int r = dis(gen);
      if (r < 1) { nextX = 0.0; nextY = 0.16 * y; }
      else if (r < 86) { nextX = 0.85 * x + 0.04 * y; nextY = -0.04 * x + 0.85 * y + 1.6; }
      else if (r < 93) { nextX = 0.20 * x - 0.26 * y; nextY = 0.23 * x + 0.22 * y + 1.6; }
      else { nextX = -0.15 * x + 0.28 * y; nextY = 0.26 * x + 0.24 * y + 0.44; }
      x = nextX; y = nextY;
      int screenX = static_cast<int>(((x + 2.182) * (CANVAS_WIDTH - 1) / (2.655 + 2.182)));
      int screenY = static_cast<int>(((9.96 - y) * (CANVAS_HEIGHT - 1) / 9.96));
      if (screenX >= 0 && screenX < CANVAS_WIDTH && screenY >= 0 && screenY < CANVAS_HEIGHT) {
        pixelGrid[screenX][screenY] = 200;
      }
    }

    for (int px = 0; px < CANVAS_WIDTH; px++) {
      for (int py = 0; py < CANVAS_HEIGHT; py++) {
        if (pixelGrid[px][py] > 0) {
          points.push_back({static_cast<double>(px), static_cast<double>(py), pixelGrid[px][py]});
        }
      }
    }
    return points;
  }
};
#ifndef TETRIS_ENGINE_H
#define TETRIS_ENGINE_H

#include <array>
#include <random>
#include <limits>
#include <algorithm>
#include <string>
#include <fstream>
#include <iomanip>

namespace TetrisEngine {

// --- Constants ---
constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

// --- Type Definitions ---
using Shape = std::array<std::array<int, 4>, 4>;

// --- Random Number Generation ---
namespace Random {
    inline std::mt19937& Generator() {
        static std::mt19937 gen(std::random_device{}());
        return gen;
    }

    inline int Int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(Generator());
    }

    inline double Double(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(Generator());
    }

    inline double Normal(double mean, double stddev) {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(Generator());
    }
}

// --- Tetromino Definitions ---
const std::array<std::array<Shape, 4>, 7> TETROMINO_SHAPES = {{
    // I-piece
    {Shape{{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}},
     Shape{{{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}}},
     Shape{{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}}},
    // O-piece
    {Shape{{{0,2,2,0},{0,2,2,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,2,2,0},{0,2,2,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,2,2,0},{0,2,2,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,2,2,0},{0,2,2,0},{0,0,0,0},{0,0,0,0}}}},
    // T-piece
    {Shape{{{0,3,0,0},{3,3,3,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,3,0,0},{0,3,3,0},{0,3,0,0},{0,0,0,0}}},
     Shape{{{0,0,0,0},{3,3,3,0},{0,3,0,0},{0,0,0,0}}},
     Shape{{{0,3,0,0},{3,3,0,0},{0,3,0,0},{0,0,0,0}}}},
    // S-piece
    {Shape{{{0,4,4,0},{4,4,0,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,4,0,0},{0,4,4,0},{0,0,4,0},{0,0,0,0}}},
     Shape{{{0,0,0,0},{0,4,4,0},{4,4,0,0},{0,0,0,0}}},
     Shape{{{4,0,0,0},{4,4,0,0},{0,4,0,0},{0,0,0,0}}}},
    // Z-piece
    {Shape{{{5,5,0,0},{0,5,5,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,0,5,0},{0,5,5,0},{0,5,0,0},{0,0,0,0}}},
     Shape{{{0,0,0,0},{5,5,0,0},{0,5,5,0},{0,0,0,0}}},
     Shape{{{0,5,0,0},{5,5,0,0},{5,0,0,0},{0,0,0,0}}}},
    // J-piece
    {Shape{{{6,0,0,0},{6,6,6,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,6,6,0},{0,6,0,0},{0,6,0,0},{0,0,0,0}}},
     Shape{{{0,0,0,0},{6,6,6,0},{0,0,6,0},{0,0,0,0}}},
     Shape{{{0,6,0,0},{0,6,0,0},{6,6,0,0},{0,0,0,0}}}},
    // L-piece
    {Shape{{{0,0,7,0},{7,7,7,0},{0,0,0,0},{0,0,0,0}}},
     Shape{{{0,7,0,0},{0,7,0,0},{0,7,7,0},{0,0,0,0}}},
     Shape{{{0,0,0,0},{7,7,7,0},{7,0,0,0},{0,0,0,0}}},
     Shape{{{7,7,0,0},{0,7,0,0},{0,7,0,0},{0,0,0,0}}}},
}};

// --- Data Structures ---
struct Piece {
    int typeId = 0;
    int rotation = 0;
    int x = 0;
    int y = 0;

    const Shape& GetShape() const {
        return TETROMINO_SHAPES[typeId - 1][rotation];
    }
};

struct Move {
    int rotation = 0;
    int x = 0;
    double score = 0.0;
};

struct HeuristicWeights {
    double w_lines = 0.0;
    double w_height = 0.0;
    double w_holes = 0.0;
    double w_bumpiness = 0.0;

    static HeuristicWeights RandomWeights() {
        return {
            TetrisEngine::Random::Double(0.1, 1.0),
            TetrisEngine::Random::Double(-1.0, -0.1),
            TetrisEngine::Random::Double(-1.0, -0.1),
            TetrisEngine::Random::Double(-1.0, -0.1)
        };
    }
};

// --- Board Engine (Pure Logic) ---
class BoardEngine {
private:
    std::array<std::array<int, BOARD_WIDTH>, BOARD_HEIGHT> grid;

public:
    BoardEngine() { Reset(); }

    void Reset() {
        grid.fill({});
    }

    const auto& GetGrid() const { return grid; }

    // NEW: Method to load board state from array
    void LoadFromArray(const int* boardState) {
        for (int r = 0; r < BOARD_HEIGHT; ++r) {
            for (int c = 0; c < BOARD_WIDTH; ++c) {
                grid[r][c] = boardState[r * BOARD_WIDTH + c];
            }
        }
    }

    bool IsValid(const Piece& piece) const {
        const auto& shape = piece.GetShape();
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                if (shape[r][c] != 0) {
                    int boardX = piece.x + c;
                    int boardY = piece.y + r;
                    if (boardX < 0 || boardX >= BOARD_WIDTH || boardY < 0 || boardY >= BOARD_HEIGHT)
                        return false;
                    if (boardY >= 0 && grid[boardY][boardX] != 0)
                        return false;
                }
            }
        }
        return true;
    }

    void PlacePiece(const Piece& piece) {
        const auto& shape = piece.GetShape();
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                if (shape[r][c] != 0) {
                    int py = piece.y + r;
                    int px = piece.x + c;
                    if (py >= 0 && py < BOARD_HEIGHT && px >= 0 && px < BOARD_WIDTH) {
                        grid[py][px] = piece.typeId;
                    }
                }
            }
        }
    }

    int ClearLines() {
        int lines = 0;
        for (int r = BOARD_HEIGHT - 1; r >= 0; ) {
            bool full = true;
            for (int c = 0; c < BOARD_WIDTH; ++c) {
                if (grid[r][c] == 0) {
                    full = false;
                    break;
                }
            }
            if (full) {
                lines++;
                for (int rr = r; rr > 0; --rr) {
                    grid[rr] = grid[rr - 1];
                }
                grid[0].fill(0);
            } else {
                --r;
            }
        }
        return lines;
    }

    bool IsGameOver(const Piece& piece) const {
        return !IsValid(piece);
    }

    int GetAggregateHeight() const {
        int total = 0;
        for (int c = 0; c < BOARD_WIDTH; ++c) {
            for (int r = 0; r < BOARD_HEIGHT; ++r) {
                if (grid[r][c] != 0) {
                    total += BOARD_HEIGHT - r;
                    break;
                }
            }
        }
        return total;
    }

    int GetHoles() const {
        int holes = 0;
        for (int c = 0; c < BOARD_WIDTH; ++c) {
            bool block = false;
            for (int r = 0; r < BOARD_HEIGHT; ++r) {
                if (grid[r][c] != 0) block = true;
                else if (block) holes++;
            }
        }
        return holes;
    }

    int GetBumpiness() const {
        int heights[BOARD_WIDTH] = {};
        for (int c = 0; c < BOARD_WIDTH; ++c) {
            for (int r = 0; r < BOARD_HEIGHT; ++r) {
                if (grid[r][c] != 0) {
                    heights[c] = BOARD_HEIGHT - r;
                    break;
                }
            }
        }
        int bump = 0;
        for (int i = 0; i < BOARD_WIDTH - 1; ++i) {
            bump += std::abs(heights[i] - heights[i+1]);
        }
        return bump;
    }

    std::vector<int> Serialize() const {
        std::vector<int> state;
        state.reserve(BOARD_WIDTH * BOARD_HEIGHT);
        for (int r = 0; r < BOARD_HEIGHT; ++r) {
            for (int c = 0; c < BOARD_WIDTH; ++c) {
                state.push_back(grid[r][c]);
            }
        }
        return state;
    }
};

// --- AI Evaluation ---
inline Move FindBestMove(const BoardEngine& board, int pieceId, const HeuristicWeights& weights) {
    Move best = {0, 0, std::numeric_limits<double>::lowest()};
    for (int r = 0; r < 4; ++r) {
        for (int x = -3; x < BOARD_WIDTH + 3; ++x) {
            int y = 0;
            Piece testPiece{pieceId, r, x, y};
            while (!board.IsValid(testPiece) && testPiece.y > -BOARD_HEIGHT) {
                testPiece.y--;
            }
            if (testPiece.y <= -BOARD_HEIGHT) continue;
            
            while (board.IsValid({pieceId, r, x, testPiece.y + 1})) {
                testPiece.y++;
            }

            if (!board.IsValid(testPiece)) continue;

            BoardEngine next = board;
            next.PlacePiece(testPiece);
            int lines = next.ClearLines();
            int height = next.GetAggregateHeight();
            int holes = next.GetHoles();
            int bump = next.GetBumpiness();

            double score = lines * lines * weights.w_lines +
                          height * weights.w_height +
                          holes * weights.w_holes +
                          bump * weights.w_bumpiness;

            if (score > best.score) {
                best = {r, x, score};
            }
        }
    }
    return best;
}

// --- File I/O ---
inline bool SaveWeights(const char* filename, double* weights) {
    std::ofstream file(filename);
    if (!file) return false;
    file << std::fixed << std::setprecision(6);
    for (int i = 0; i < 4; ++i) file << weights[i] << "\n";
    return file.good();
}

inline bool LoadWeights(const char* filename, double* weights) {
    std::ifstream file(filename);
    if (!file) return false;
    for (int i = 0; i < 4; ++i) file >> weights[i];
    return !file.fail();
}

// --- DLL EXPORT INTERFACE ---
class  TetrisGameInstance {
public:
    BoardEngine board;
    HeuristicWeights weights;
    int score = 0, lines = 0, level = 1;
    int currentPiece = 0, nextPiece = 0;
    bool gameOver = false;
    
    TetrisGameInstance() {
        Reset();
    }
    
    void Reset() {
        board.Reset();
        score = lines = level = 0;
        gameOver = false;
        nextPiece = Random::Int(1, 7);
    }
    
    bool LoadModel(const std::string& filename) {
        return LoadWeights(filename.c_str(), &weights.w_lines);
    }
    
    void StepAI() {
        if (gameOver) return;
        
        currentPiece = nextPiece;
        nextPiece = Random::Int(1, 7);
        
        if (board.IsGameOver({currentPiece, 0, 3, 0})) {
            gameOver = true;
            return;
        }
        
        // Find best move
        int rotation = 0, x = 0;
        Move best = FindBestMove(board, currentPiece, weights);
        rotation = best.rotation;
        x = best.x;
        
        // Drop piece
        int y = 0;
        while (!board.IsValid({currentPiece, rotation, x, y})) y--;
        while (board.IsValid({currentPiece, rotation, x, y + 1})) y++;
        
        board.PlacePiece({currentPiece, rotation, x, y});
        int cleared = board.ClearLines();
        
        if (cleared) {
            lines += cleared;
            score += cleared * cleared * 100 * level;
            level = 1 + (lines / 10);
        }
    }
    
    void GetState(int* boardState, int* outScore, int* outLines, int* outLevel, int* outNext) {
        // Copy board grid
        for (int r = 0; r < BOARD_HEIGHT; ++r) {
            for (int c = 0; c < BOARD_WIDTH; ++c) {
                boardState[r * BOARD_WIDTH + c] = board.GetGrid()[r][c];
            }
        }
        *outScore = score;
        *outLines = lines;
        *outLevel = level;
        *outNext = nextPiece;
    }
    
    bool EvaluateBoard(const int* boardState) {
        // Copy board state for evaluation
        BoardEngine evalBoard;
        evalBoard.LoadFromArray(boardState);  // FIXED: Use new method
        
        // Check if this board state is game over for a new piece
        return evalBoard.IsGameOver({1, 0, 3, 0});
    }
};
    
}; // namespace TetrisEngine

#endif // TETRIS_ENGINE_H
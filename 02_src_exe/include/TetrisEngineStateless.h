#pragma once
#include <array>
#include <vector>
#include <random>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>
#include <iomanip>


typedef void* TETRIS_Game; // Opaque handle


// ==================== Constants ====================
constexpr int BOARD_WIDTH = 10;
constexpr int BOARD_HEIGHT = 20;

constexpr int POPULATION_SIZE = 50;
constexpr int NUM_GENERATIONS = 20;
constexpr int NUM_GAMES_PER_FITNESS_TEST = 1;
constexpr int MAX_MOVES_PER_GAME = 500;
constexpr double MUTATION_RATE = 0.1;
constexpr double MUTATION_STRENGTH = 0.5;
constexpr int TOURNAMENT_SIZE = 5;
constexpr double ELITISM_RATE = 0.1;

// ==================== Type Definitions ====================
using BoardGrid = std::array<std::array<int, BOARD_WIDTH>, BOARD_HEIGHT>;
using Shape = std::array<std::array<int, 4>, 4>;

struct Piece {
    int typeId;
    int rotation;
    int x;
    int y;
    const Shape& GetShape() const;
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
    
    static HeuristicWeights RandomWeights();
    void Print() const;
};

struct Individual {
    HeuristicWeights weights;
    double fitness = 0.0;
    bool operator>(const Individual& other) const { return fitness > other.fitness; }
};

// Tetromino shapes (defined in .cpp)
extern const std::array<std::array<Shape, 4>, 7> TETROMINO_SHAPES;

// ==================== Engine Functions (Pure Logic) ====================
namespace Engine {
    // Random utilities
    namespace Random {
        extern std::mt19937 generator;
        int Int(int min, int max);
        double Double(double min, double max);
        double Normal(double mean, double stddev);
    }

    // Core game logic - all state passed via parameters
    bool IsValid(const BoardGrid& grid, const Piece& piece);
    BoardGrid PlacePiece(const BoardGrid& grid, const Piece& piece);
    int ClearLines(BoardGrid& grid); // Modifies grid in-place for efficiency
    bool IsGameOver(const BoardGrid& grid, const Piece& piece);
    int GetAggregateHeight(const BoardGrid& grid);
    int GetHoles(const BoardGrid& grid);
    int GetBumpiness(const BoardGrid& grid);
    Move FindBestMove(const BoardGrid& grid, int pieceId, const HeuristicWeights& weights);
    double SimulateGame(const HeuristicWeights& weights);
    
    // Genetic Algorithm operations
    Individual TournamentSelection(const std::vector<Individual>& pop);
    HeuristicWeights Crossover(const HeuristicWeights& a, const HeuristicWeights& b);
    void Mutate(HeuristicWeights& w);
    HeuristicWeights RunGeneticAlgorithm();
    
    // File I/O
    bool SaveWeights(const HeuristicWeights& w, const std::string& filename);
    bool LoadWeights(HeuristicWeights& w, const std::string& filename);
}

struct GameState {
    BoardGrid grid;
    BoardGrid previewGrid; // Add this for preview rendering
    int lastPiece = 0;     // Store last piece for preview
    Move lastMove;         // Store last move for preview

    int score = 0;
    int lines = 0;
    int level = 1;
    int nextPiece = 0;
    bool gameOver = false;
    bool autoPlay = false;
    HeuristicWeights aiWeights;
};

// ==================== Random Implementation ====================
namespace Engine::Random {
    std::mt19937 generator(std::random_device{}());

    int Int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator);
    }

    double Double(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator);
    }

    double Normal(double mean, double stddev) {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(generator);
    }
}

// ==================== Tetromino Shapes Definition ====================
const std::array<std::array<Shape, 4>, 7> TETROMINO_SHAPES = {{
    // I Piece
    std::array<Shape, 4>{{
        Shape{{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}},
        Shape{{{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}},
        Shape{{{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}}},
        Shape{{{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}}}
    }},
    // O Piece
    std::array<Shape, 4>{{
        Shape{{{0,2,2,0}, {0,2,2,0}, {0,0,0,0}, {0,0,0,0}}},
        Shape{{{0,2,2,0}, {0,2,2,0}, {0,0,0,0}, {0,0,0,0}}},
        Shape{{{0,2,2,0}, {0,2,2,0}, {0,0,0,0}, {0,0,0,0}}},
        Shape{{{0,2,2,0}, {0,2,2,0}, {0,0,0,0}, {0,0,0,0}}}
    }},
    // T Piece
    std::array<Shape, 4>{{
        Shape{{{0,0,0,0}, {3,3,3,0}, {0,3,0,0}, {0,0,0,0}}},
        Shape{{{0,3,0,0}, {3,3,0,0}, {0,3,0,0}, {0,0,0,0}}},
        Shape{{{0,3,0,0}, {3,3,3,0}, {0,0,0,0}, {0,0,0,0}}},
        Shape{{{0,3,0,0}, {0,3,3,0}, {0,3,0,0}, {0,0,0,0}}}
    }},
    // S Piece
    std::array<Shape, 4>{{
        Shape{{{0,0,0,0}, {0,4,4,0}, {4,4,0,0}, {0,0,0,0}}},
        Shape{{{0,4,0,0}, {0,4,4,0}, {0,0,4,0}, {0,0,0,0}}},
        Shape{{{0,0,0,0}, {0,4,4,0}, {4,4,0,0}, {0,0,0,0}}},
        Shape{{{0,4,0,0}, {0,4,4,0}, {0,0,4,0}, {0,0,0,0}}}
    }},
    // Z Piece
    std::array<Shape, 4>{{
        Shape{{{0,0,0,0}, {5,5,0,0}, {0,5,5,0}, {0,0,0,0}}},
        Shape{{{0,0,5,0}, {0,5,5,0}, {0,5,0,0}, {0,0,0,0}}},
        Shape{{{0,0,0,0}, {5,5,0,0}, {0,5,5,0}, {0,0,0,0}}},
        Shape{{{0,0,5,0}, {0,5,5,0}, {0,5,0,0}, {0,0,0,0}}}
    }},
    // J Piece
    std::array<Shape, 4>{{
        Shape{{{0,0,0,0}, {6,6,6,0}, {0,0,6,0}, {0,0,0,0}}},
        Shape{{{0,6,0,0}, {0,6,0,0}, {6,6,0,0}, {0,0,0,0}}},
        Shape{{{6,0,0,0}, {6,6,6,0}, {0,0,0,0}, {0,0,0,0}}},
        Shape{{{0,6,6,0}, {0,6,0,0}, {0,6,0,0}, {0,0,0,0}}}
    }},
    // L Piece
    std::array<Shape, 4>{{
        Shape{{{0,0,0,0}, {7,7,7,0}, {7,0,0,0}, {0,0,0,0}}},
        Shape{{{7,7,0,0}, {0,7,0,0}, {0,7,0,0}, {0,0,0,0}}},
        Shape{{{0,0,7,0}, {7,7,7,0}, {0,0,0,0}, {0,0,0,0}}},
        Shape{{{0,7,0,0}, {0,7,0,0}, {0,7,7,0}, {0,0,0,0}}}
    }}
}};

const Shape& Piece::GetShape() const {
    return TETROMINO_SHAPES[typeId - 1][rotation];
}

// ==================== HeuristicWeights Implementation ====================
HeuristicWeights HeuristicWeights::RandomWeights() {
    return {
        Engine::Random::Double(0.1, 1.0),
        Engine::Random::Double(-1.0, -0.1),
        Engine::Random::Double(-1.0, -0.1),
        Engine::Random::Double(-1.0, -0.1)
    };
}

void HeuristicWeights::Print() const {
    std::cout << std::fixed << std::setprecision(4)
              << "Lines: " << w_lines
              << ", Height: " << w_height
              << ", Holes: " << w_holes
              << ", Bumpiness: " << w_bumpiness;
}

// ==================== Stateless Game Logic ====================
bool Engine::IsValid(const BoardGrid& grid, const Piece& piece) {
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

BoardGrid Engine::PlacePiece(const BoardGrid& grid, const Piece& piece) {
    BoardGrid newGrid = grid;
    const auto& shape = piece.GetShape();
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (shape[r][c] != 0) {
                int py = piece.y + r;
                int px = piece.x + c;
                if (py >= 0 && py < BOARD_HEIGHT && px >= 0 && px < BOARD_WIDTH) {
                    newGrid[py][px] = piece.typeId;
                }
            }
        }
    }
    return newGrid;
}

int Engine::ClearLines(BoardGrid& grid) {
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

bool Engine::IsGameOver(const BoardGrid& grid, const Piece& piece) {
    return !Engine::IsValid(grid, piece);
}

int Engine::GetAggregateHeight(const BoardGrid& grid) {
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

int Engine::GetHoles(const BoardGrid& grid) {
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

int Engine::GetBumpiness(const BoardGrid& grid) {
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

Move Engine::FindBestMove(const BoardGrid& grid, int pieceId, const HeuristicWeights& weights) {
    Move best = {0, 0, std::numeric_limits<double>::lowest()};
    for (int r = 0; r < 4; ++r) {
        for (int x = -3; x < BOARD_WIDTH + 3; ++x) {
            int y = 0;
            Piece testPiece{pieceId, r, x, y};
            while (!Engine::IsValid(grid, testPiece) && y > -BOARD_HEIGHT) {
                y--;
                testPiece.y = y;
            }
            
            if (y <= -BOARD_HEIGHT) continue;
            
            while (Engine::IsValid(grid, {pieceId, r, x, y + 1})) y++;
            
            Piece finalPiece{pieceId, r, x, y};
            if (!Engine::IsValid(grid, finalPiece)) continue;

            BoardGrid nextGrid = Engine::PlacePiece(grid, finalPiece);
            int lines = Engine::ClearLines(nextGrid);
            int height = Engine::GetAggregateHeight(nextGrid);
            int holes = Engine::GetHoles(nextGrid);
            int bump = Engine::GetBumpiness(nextGrid);

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

double Engine::SimulateGame(const HeuristicWeights& weights) {
    BoardGrid grid = {}; // Initialize empty grid
    int lines = 0, moves = 0;
    int nextPiece = Engine::Random::Int(1, 7);
    
    while (moves < MAX_MOVES_PER_GAME) {
        int currentPiece = nextPiece;
        nextPiece = Engine::Random::Int(1, 7);
        
        Piece p{currentPiece, 0, 3, 0};
        if (Engine::IsGameOver(grid, p)) break;
        
        Move m = Engine::FindBestMove(grid, currentPiece, weights);
        p.rotation = m.rotation; p.x = m.x;
        
        int y = 0;
        while (!Engine::IsValid(grid, {currentPiece, m.rotation, m.x, y})) y--;
        p.y = y;
        while (Engine::IsValid(grid, {currentPiece, m.rotation, m.x, p.y + 1})) p.y++;
        
        grid = Engine::PlacePiece(grid, p);
        lines += Engine::ClearLines(grid);
        moves++;
    }
    return static_cast<double>(lines);
}

// ==================== Genetic Algorithm Implementation ====================
Individual Engine::TournamentSelection(const std::vector<Individual>& pop) {
    Individual best{{}, std::numeric_limits<double>::lowest()};
    for (int i = 0; i < TOURNAMENT_SIZE; ++i) {
        const auto& c = pop[Engine::Random::Int(0, pop.size()-1)];
        if (c.fitness > best.fitness) best = c;
    }
    return best;
}

HeuristicWeights Engine::Crossover(const HeuristicWeights& a, const HeuristicWeights& b) {
    return {
        (a.w_lines + b.w_lines) / 2.0,
        (a.w_height + b.w_height) / 2.0,
        (a.w_holes + b.w_holes) / 2.0,
        (a.w_bumpiness + b.w_bumpiness) / 2.0
    };
}

void Engine::Mutate(HeuristicWeights& w) {
    if (Engine::Random::Double(0,1) < MUTATION_RATE) w.w_lines += Engine::Random::Normal(0, MUTATION_STRENGTH);
    if (Engine::Random::Double(0,1) < MUTATION_RATE) w.w_height += Engine::Random::Normal(0, MUTATION_STRENGTH);
    if (Engine::Random::Double(0,1) < MUTATION_RATE) w.w_holes += Engine::Random::Normal(0, MUTATION_STRENGTH);
    if (Engine::Random::Double(0,1) < MUTATION_RATE) w.w_bumpiness += Engine::Random::Normal(0, MUTATION_STRENGTH);
}

HeuristicWeights Engine::RunGeneticAlgorithm() {
    std::vector<Individual> pop(POPULATION_SIZE);
    for (auto& ind : pop) ind.weights = HeuristicWeights::RandomWeights();

    std::cout << "Starting Genetic Algorithm training...\n";
    for (int gen = 0; gen < NUM_GENERATIONS; ++gen) {
        for (auto& ind : pop) {
            double f = 0;
            for (int i = 0; i < NUM_GAMES_PER_FITNESS_TEST; ++i)
                f += SimulateGame(ind.weights);
            ind.fitness = f / NUM_GAMES_PER_FITNESS_TEST;
        }

        std::sort(pop.begin(), pop.end(), std::greater<Individual>());

        std::vector<Individual> newPop;
        int elite = POPULATION_SIZE * ELITISM_RATE;
        for (int i = 0; i < elite; ++i) newPop.push_back(pop[i]);

        while (newPop.size() < POPULATION_SIZE) {
            auto p1 = TournamentSelection(pop);
            auto p2 = TournamentSelection(pop);
            HeuristicWeights child = Crossover(p1.weights, p2.weights);
            Mutate(child);
            newPop.push_back({child, 0.0});
        }
        pop = newPop;

        std::cout << "Gen " << gen+1 << ": Best=" << pop[0].fitness << " ";
        pop[0].weights.Print(); std::cout << "\n";
    }
    std::cout << "Training complete!\n";
    return pop[0].weights;
}

// File I/O
bool Engine::SaveWeights(const HeuristicWeights& w, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    file << std::fixed << std::setprecision(6);
    file << w.w_lines << "\n" << w.w_height << "\n" << w.w_holes << "\n" << w.w_bumpiness << "\n";
    bool success = file.good();
    file.close();
    return success;
}

bool Engine::LoadWeights(HeuristicWeights& w, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    file >> w.w_lines >> w.w_height >> w.w_holes >> w.w_bumpiness;
    bool success = !file.fail();
    file.close();
    return success;
}

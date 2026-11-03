// tictactoe.h
#ifndef TICTACTOE_H
#define TICTACTOE_H
#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "tensorflow/c/c_api.h"

enum AIMode {
    EXPERT      = 0,
    CREATIVE    = 1,
    MINIMAX     = 2,
    RANDOM      = 3,
    TENSORFLOW  = 4
};

extern "C" {
    typedef struct {
        int finalBoard[9];
        int moves[9];
        int winner;
        int moveCount;
        int history[10][9];
        int historyCount;
    } TicTacToeResultOnline;
}

class TensorFlowTicTacToe {
public:
    TF_Graph* graph = nullptr;
    TF_Session* session = nullptr;
    TF_Status* status = nullptr;

    bool LoadModel(const char* export_dir);
    bool PredictBestMove(const float input_board[9], int& best_move);
    ~TensorFlowTicTacToe();
};

struct TicTacToe {
    std::vector<int> board{0, 0, 0, 0, 0, 0, 0, 0, 0};

    void print();
    bool isGameOver(int& winner);
    std::vector<int> getValidMoves();
    void reset();
};

class NeuralNetwork {
public:
    std::vector<double> input, hidden, output;
    std::vector<std::vector<double>> weights_ih, weights_ho;
    std::vector<double> bias_h, bias_o;
    double learningRate = 0.1;

    NeuralNetwork(int inputSize, int hiddenSize, int outputSize);

    double sigmoid(double x);
    double sigmoidDerivative(double x);
    void forward(const std::vector<double>& x);
    void backprop(const std::vector<double>& target);
    bool saveModel(const std::string& filename);
    bool loadModel(const std::string& filename);
};

// Function Prototypes (Declarations only)
std::vector<double> boardToInput(const std::vector<int>& board);
void maskOutputs(std::vector<double>& output, const TicTacToe& game);
int selectMove(const std::vector<double>& output, const TicTacToe& game);
void trainStep(NeuralNetwork& net);
std::vector<double> boardToInput(const int board[9]);
std::vector<double> softmax(const std::vector<double>& logits, double temp);
int selectGreedy(const std::vector<double>& scores, const TicTacToe& game);
int selectSampled(const std::vector<double>& probs, const TicTacToe& game);
int selectRandomMove(TicTacToe& game);
int minimax(const int board[9], int depth, bool isMaximizing, int alpha = -1000, int beta = 1000);
int minimaxMove(std::vector<int> board, int player);
int selectMove(const std::vector<double>& output, TicTacToe& game, int aiMode, double temperature);
bool RunTicTacToeSelfPlay(TicTacToeResultOnline& result, int aiMode, double temperature);

#endif
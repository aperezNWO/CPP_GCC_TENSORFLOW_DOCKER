// tictactoe.cpp
#include "tictactoe.h"

// Global random number generators defined once
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);
std::uniform_int_distribution<>  moveDis(0, 8);

// TensorFlowTicTacToe implementation
bool TensorFlowTicTacToe::LoadModel(const char* export_dir) {
    if (session) return true;

    status = TF_NewStatus();
    graph = TF_NewGraph();

    TF_SessionOptions* opts = TF_NewSessionOptions();
    TF_Buffer* meta_graph_def = TF_NewBuffer();

    const char* tags = "serve";
    int ntags = 1;

    std::cout << "?? Loading SavedModel from: " << export_dir << "\n";

    session = TF_LoadSessionFromSavedModel(opts, nullptr, export_dir, &tags, ntags, graph, meta_graph_def, status);

    TF_DeleteSessionOptions(opts);
    TF_DeleteBuffer(meta_graph_def);

    if (TF_GetCode(status) != TF_OK) {
        std::cerr << "? Failed to load model: " << TF_Message(status) << "\n";
        return false;
    }

    std::cout << "? Model loaded successfully.\n";
    return true;
}

bool TensorFlowTicTacToe::PredictBestMove(const float input_board[9], int& best_move) {
    if (!session) {
        std::cerr << "? Session not initialized.\n";
        return false;
    }

    int64_t input_dims[] = {1, 9};
    TF_Tensor* input_tensor = TF_AllocateTensor(TF_FLOAT, input_dims, 2, sizeof(float) * 9);
    float* input_data = static_cast<float*>(TF_TensorData(input_tensor));
    for (int i = 0; i < 9; ++i) input_data[i] = input_board[i];

    int64_t output_dims[] = {1, 9};
    TF_Tensor* output_tensor = nullptr;

    const char* input_name = "serving_default_dense_input";
    const char* output_name = "StatefulPartitionedCall";

    TF_Output inputs[1] = {{TF_GraphOperationByName(graph, input_name), 0}};
    TF_Output outputs[1] = {{TF_GraphOperationByName(graph, output_name), 0}};

    if (!inputs[0].oper || !outputs[0].oper) {
        std::cerr << "? Invalid tensor name!\n";
        TF_DeleteTensor(input_tensor);
        return false;
    }

    TF_SessionRun(session, nullptr, inputs, &input_tensor, 1, outputs, &output_tensor, 1, nullptr, 0, nullptr, status);

    if (TF_GetCode(status) != TF_OK) {
        std::cerr << "? Inference failed: " << TF_Message(status) << "\n";
        TF_DeleteTensor(input_tensor);
        return false;
    }

    float* probs = static_cast<float*>(TF_TensorData(output_tensor));
    std::vector<int> validMoves;
    for (int i = 0; i < 9; ++i) {
        if (input_board[i] == 0.0f) validMoves.push_back(i);
    }

    if (validMoves.empty()) {
        TF_DeleteTensor(input_tensor);
        TF_DeleteTensor(output_tensor);
        return false;
    }

    const float temperature = 1.5f;
    std::vector<float> exp_probs;
    float sum = 0.0f;
    for (int idx : validMoves) {
        float p = std::exp(probs[idx] / temperature);
        exp_probs.push_back(p);
        sum += p;
    }
    for (float& p : exp_probs) p /= sum;

    std::random_device rd_local;
    std::mt19937 gen_local(rd_local());
    std::discrete_distribution<> dist(exp_probs.begin(), exp_probs.end());
    best_move = validMoves[dist(gen_local)];

    TF_DeleteTensor(input_tensor);
    TF_DeleteTensor(output_tensor);
    return true;
}

TensorFlowTicTacToe::~TensorFlowTicTacToe() {
    if (session) TF_CloseSession(session, status);
    if (session) TF_DeleteSession(session, status);
    if (graph) TF_DeleteGraph(graph);
    if (status) TF_DeleteStatus(status);
}

// TicTacToe implementation
void TicTacToe::print() {
    for (int i = 0; i < 9; ++i) {
        char c = '.';
        if (board[i] == 1) c = 'X';
        else if (board[i] == -1) c = 'O';
        std::cout << c;
        if ((i+1) % 3 == 0) std::cout << std::endl;
    }
    std::cout << "---\n";
}

bool TicTacToe::isGameOver(int& winner) {
    const int wins[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
    };
    for (auto& w : wins) {
        if (board[w[0]] != 0 && board[w[0]] == board[w[1]] && board[w[1]] == board[w[2]]) {
            winner = board[w[0]];
            return true;
        }
    }
    if (std::find(board.begin(), board.end(), 0) == board.end()) {
        winner = 0;
        return true;
    }
    winner = 0;
    return false;
}

std::vector<int> TicTacToe::getValidMoves() {
    std::vector<int> moves;
    for (int i = 0; i < 9; ++i) if (board[i] == 0) moves.push_back(i);
    return moves;
}

void TicTacToe::reset() { board.assign(9, 0); }

// NeuralNetwork implementation
NeuralNetwork::NeuralNetwork(int inputSize, int hiddenSize, int outputSize)
    : input(inputSize), hidden(hiddenSize), output(outputSize),
      weights_ih(hiddenSize, std::vector<double>(inputSize)),
      weights_ho(outputSize, std::vector<double>(hiddenSize)),
      bias_h(hiddenSize), bias_o(outputSize) {
    for (auto& row : weights_ih) for (double& w : row) w = dis(gen) * 2.0 - 1.0;
    for (auto& row : weights_ho) for (double& w : row) w = dis(gen) * 2.0 - 1.0;
    for (double& b : bias_h) b = dis(gen) * 2.0 - 1.0;
    for (double& b : bias_o) b = dis(gen) * 2.0 - 1.0;
}

double NeuralNetwork::sigmoid(double x) { return 1.0 / (1.0 + exp(-std::max(-700.0, std::min(700.0, x)))); }
double NeuralNetwork::sigmoidDerivative(double x) { double s = sigmoid(x); return s * (1 - s); }

void NeuralNetwork::forward(const std::vector<double>& x) {
    input = x;
    for (size_t i = 0; i < hidden.size(); ++i) {
        double activation = bias_h[i];
        for (size_t j = 0; j < input.size(); ++j) activation += weights_ih[i][j] * input[j];
        hidden[i] = sigmoid(activation);
    }
    for (size_t i = 0; i < output.size(); ++i) {
        double activation = bias_o[i];
        for (size_t j = 0; j < hidden.size(); ++j) activation += weights_ho[i][j] * hidden[j];
        output[i] = sigmoid(activation);
    }
}

void NeuralNetwork::backprop(const std::vector<double>& target) {
    std::vector<double> grad_output(output.size());
    for (size_t i = 0; i < output.size(); ++i) grad_output[i] = (target[i] - output[i]) * sigmoidDerivative(output[i]);

    std::vector<double> grad_hidden(hidden.size(), 0);
    for (size_t i = 0; i < hidden.size(); ++i) {
        for (size_t j = 0; j < output.size(); ++j) grad_hidden[i] += grad_output[j] * weights_ho[j][i];
        grad_hidden[i] *= sigmoidDerivative(hidden[i]);
    }

    for (size_t i = 0; i < output.size(); ++i) {
        bias_o[i] += learningRate * grad_output[i];
        for (size_t j = 0; j < hidden.size(); ++j) weights_ho[i][j] += learningRate * grad_output[i] * hidden[j];
    }
    for (size_t i = 0; i < hidden.size(); ++i) {
        bias_h[i] += learningRate * grad_hidden[i];
        for (size_t j = 0; j < input.size(); ++j) weights_ih[i][j] += learningRate * grad_hidden[i] * input[j];
    }
}

bool NeuralNetwork::saveModel(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    auto writeMatrix = [&](const std::vector<std::vector<double>>& mat) {
        file << mat.size() << " " << mat[0].size() << "\n";
        for (const auto& row : mat) { for (double w : row) file << std::setprecision(10) << w << " "; file << "\n"; }
    };
    auto writeVector = [&](const std::vector<double>& vec) {
        file << vec.size() << "\n";
        for (double v : vec) file << std::setprecision(10) << v << " "; file << "\n";
    };
    writeMatrix(weights_ih); writeMatrix(weights_ho); writeVector(bias_h); writeVector(bias_o);
    file.close();
    return true;
}

bool NeuralNetwork::loadModel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    auto readMatrix = [&](std::vector<std::vector<double>>& mat) -> bool {
        int rows, cols; if (!(file >> rows >> cols)) return false;
        mat.resize(rows, std::vector<double>(cols));
        for (auto& row : mat) for (double& w : row) if (!(file >> w)) return false;
        return true;
    };
    auto readVector = [&](std::vector<double>& vec) -> bool {
        int size; if (!(file >> size)) return false;
        vec.resize(size); for (double& v : vec) if (!(file >> v)) return false;
        return true;
    };
    if (!readMatrix(weights_ih)) goto fail;
    if (!readMatrix(weights_ho)) goto fail;
    if (!readVector(bias_h))     goto fail;
    if (!readVector(bias_o))     goto fail;
    file.close(); return true;
fail:
    file.close(); return false;
}

// Utility functions
std::vector<double> boardToInput(const std::vector<int>& board) {
    std::vector<double> input;
    for (int cell : board) input.push_back(static_cast<double>(cell));
    return input;
}

void maskOutputs(std::vector<double>& output, const TicTacToe& game) {
    for (int i = 0; i < 9; ++i) if (game.board[i] != 0) output[i] = -1e9;
}

int selectMove(const std::vector<double>& output, const TicTacToe& game) {
    std::vector<double> masked = output;
    maskOutputs(masked, game);
    return std::distance(masked.begin(), std::max_element(masked.begin(), masked.end()));
}

void trainStep(NeuralNetwork& net) { 
    TicTacToe game;
    std::vector<std::pair<std::vector<double>, std::vector<double>>> history;
    int turn = 1;
    while (true) {
        std::vector<double> input = boardToInput(game.board);
        net.forward(input);
        auto validMoves = game.getValidMoves();
        if (validMoves.empty()) break;
        int move = selectMove(net.output, game);
        history.push_back({input, net.output});
        game.board[move] = turn;
        int winner;
        if (game.isGameOver(winner)) {
            for (auto& [state, probs] : history) {
                std::vector<double> target(9, 0.0);
                if (turn == 1) {
                    if (winner == 1) target[move] = 1.0;
                    else if (winner == -1) target[move] = -1.0;
                    else target[move] = 0.5;
                } else {
                    if (winner == -1) target[move] = 1.0;
                    else if (winner == 1) target[move] = -1.0;
                    else target[move] = 0.5;
                }
                net.backprop(target);
            }
            break;
        }
        turn = -turn;
    }
}

std::vector<double> boardToInput(const int board[9]) {
    std::vector<double> input(9);
    for (int i = 0; i < 9; ++i) input[i] = static_cast<double>(board[i]);
    return input;
}

std::vector<double> softmax(const std::vector<double>& logits, double temp) {
    std::vector<double> probs(9);
    double maxVal = *std::max_element(logits.begin(), logits.end());
    double sum = 0.0;
    for (int i = 0; i < 9; ++i) {
        double expVal = std::exp((logits[i] - maxVal) / temp);
        probs[i] = expVal;
        sum += expVal;
    }
    for (double& p : probs) p /= sum;
    return probs;
}

int selectGreedy(const std::vector<double>& scores, const TicTacToe& game) {
    std::vector<std::pair<double, int>> candidates;
    for (int i = 0; i < 9; ++i) if (game.board[i] == 0) candidates.emplace_back(scores[i], i);
    if (candidates.empty()) return -1;
    return std::max_element(candidates.begin(), candidates.end())->second;
}

int selectSampled(const std::vector<double>& probs, const TicTacToe& game) {
    std::vector<int> valid;
    std::vector<double> weights;
    for (int i = 0; i < 9; ++i) {
        if (game.board[i] == 0) { valid.push_back(i); weights.push_back(probs[i]); }
    }
    if (valid.empty()) return -1;
    std::random_device rd_local;
    std::mt19937 gen_local(rd_local());
    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return valid[dist(gen_local)];
}

int selectRandomMove(TicTacToe& game) {
    auto valid = game.getValidMoves();
    if (valid.empty()) return -1;
    std::random_device rd_local;
    std::mt19937 gen_local(rd_local());
    std::uniform_int_distribution<> dist(0, static_cast<int>(valid.size()) - 1);
    return valid[dist(gen_local)];
}

int minimax(const int board[9], int depth, bool isMaximizing, int alpha, int beta) {
    const int wins[8][3] = {{0,1,2}, {3,4,5}, {6,7,8}, {0,3,6}, {1,4,7}, {2,5,8}, {0,4,8}, {2,4,6}};
    for (const auto& w : wins) {
        if (board[w[0]] != 0 && board[w[0]] == board[w[1]] && board[w[1]] == board[w[2]])
            return board[w[0]] == 1 ? 10 - depth : depth - 10;
    }
    bool isFull = true;
    for (int i = 0; i < 9; ++i) if (board[i] == 0) { isFull = false; break; }
    if (isFull) return 0;

    if (isMaximizing) {
        int best = -1000;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) {
                int temp[9]; for (int j = 0; j < 9; ++j) temp[j] = board[j];
                temp[i] = 1;
                best = std::max(best, minimax(temp, depth + 1, false, alpha, beta));
                alpha = std::max(alpha, best);
                if (beta <= alpha) break;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) {
                int temp[9]; for (int j = 0; j < 9; ++j) temp[j] = board[j];
                temp[i] = -1;
                best = std::min(best, minimax(temp, depth + 1, true, alpha, beta));
                beta = std::min(beta, best);
                if (beta <= alpha) break;
            }
        }
        return best;
    }
}

int minimaxMove(std::vector<int> board, int player) {
    int bestMove = -1;
    int bestValue = (player == 1) ? -1000 : 1000;
    for (int i = 0; i < 9; ++i) {
        if (board[i] == 0) {
            int temp[9]; for (int j = 0; j < 9; ++j) temp[j] = board[j];
            temp[i] = player;
            int moveValue = minimax(temp, 0, player == -1);
            if (player == 1 && moveValue > bestValue) { bestValue = moveValue; bestMove = i; }
            if (player == -1 && moveValue < bestValue) { bestValue = moveValue; bestMove = i; }
        }
    }
    if (bestMove == -1) { for (int i = 0; i < 9; ++i) if (board[i] == 0) return i; }
    return bestMove;
}

int selectMove(const std::vector<double>& output, TicTacToe& game, int aiMode, double temperature) {
    if (aiMode == MINIMAX || aiMode == TENSORFLOW) return -1;
    auto probs = softmax(output, temperature);
    switch (aiMode) {
        case RANDOM: return selectRandomMove(game);
        case EXPERT: return selectGreedy(probs, game);
        case CREATIVE: return selectSampled(probs, game);
        default: return selectGreedy(probs, game);
    }
}

bool RunTicTacToeSelfPlay(TicTacToeResultOnline& result, int aiMode, double temperature) {
    TensorFlowTicTacToe tf;
    NeuralNetwork netStandalone(9, 18, 9);
    TicTacToe game;

    std::random_device rd_local;
    std::mt19937 gen_local(rd_local());
    std::uniform_int_distribution<> starter(0, 1);
    int turn = (starter(gen_local) == 0) ? 1 : -1;

    int winner;
    int move = -1;
    std::vector<int> moves;

    for (int i = 0; i < 9; ++i) result.history[0][i] = game.board[i];
    result.historyCount = 1;

    if (aiMode == TENSORFLOW) {
        if (!tf.LoadModel("tictactoe_tf_model")) return false;
    } else {
        const std::string modelFile = "tictactoe_model.txt";
        if (aiMode != MINIMAX && !netStandalone.loadModel(modelFile)) {
            for (int i = 0; i < 5000; ++i) trainStep(netStandalone);
            netStandalone.saveModel(modelFile);
        }
    }

    while (true) {
        if (aiMode == TENSORFLOW) {
            float input[9];
            for (int i = 0; i < 9; ++i) input[i] = static_cast<float>(game.board[i]);
            if (!tf.PredictBestMove(input, move)) return false;
        } else if (aiMode == MINIMAX) {
            move = minimaxMove(game.board, turn);
        } else {
            std::vector<double> input = boardToInput(game.board);
            netStandalone.forward(input);
            move = selectMove(netStandalone.output, game, aiMode, temperature);
        }

        if (move < 0 || move >= 9 || game.board[move] != 0) {
            auto valid = game.getValidMoves();
            if (valid.empty()) break;
            move = valid[0];
        }

        game.board[move] = turn;
        moves.push_back(move);

        if (result.historyCount < 10) {
            for (int i = 0; i < 9; ++i) result.history[result.historyCount][i] = game.board[i];
            result.historyCount++;
        }

        if (game.isGameOver(winner)) break;
        turn = -turn;
    }

    result.winner = winner;
    for (int i = 0; i < 9; ++i) {
        result.finalBoard[i] = game.board[i];
        result.moves[i] = (i < static_cast<int>(moves.size())) ? moves[i] : -1;
    }
    result.moveCount = static_cast<int>(moves.size());
    return true;
}
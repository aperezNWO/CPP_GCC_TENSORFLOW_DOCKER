// tictactoe.h
#ifndef TICTACTOE_H // include guard
#define TICTACTOE_H
#endif
#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "tensorflow/c/c_api.h"

// ----------------------------
// Move Selection Strategies
// ----------------------------

enum AIMode {
    EXPERT      = 0,
    CREATIVE    = 1,
    MINIMAX     = 2,
    RANDOM      = 3,
    TENSORFLOW  = 4  // ← Placeholder
};

// ----------------------------
// C-Style Export Types
// ----------------------------

extern "C" {
    typedef struct {
        int finalBoard[9];
        int moves[9];              // -1 if not used
        int winner;
        int moveCount;

        // NEW: Game history (for animation)
        int history[10][9];        // Up to 10 states
        int historyCount;
    } TicTacToeResultOnline;
}

// ======================================
// TensorFlow Integration
// ======================================

class TensorFlowTicTacToe {
public:
    TF_Graph* graph = nullptr;
    TF_Session* session = nullptr;
    TF_Status* status = nullptr;

    bool LoadModel(const char* export_dir) {
        if (session) return true; // Already loaded

        status = TF_NewStatus();
        graph = TF_NewGraph();

        TF_SessionOptions* opts = TF_NewSessionOptions();
        TF_Buffer* meta_graph_def = TF_NewBuffer();

        const char* tags = "serve";
        int ntags = 1;

        std::cout << "🔍 Loading SavedModel from: " << export_dir << "\n";

        session = TF_LoadSessionFromSavedModel(opts, nullptr,
                                               export_dir,
                                               &tags, ntags,
                                               graph, meta_graph_def, status);

        TF_DeleteSessionOptions(opts);
        TF_DeleteBuffer(meta_graph_def);

        if (TF_GetCode(status) != TF_OK) {
            std::cerr << "❌ Failed to load model: " << TF_Message(status) << "\n";
            return false;
        }

        std::cout << "✅ Model loaded successfully.\n";
        return true;
    }

    bool PredictBestMove(const float input_board[9], int& best_move) {
        if (!session) {
            std::cerr << "❌ Session not initialized.\n";
            return false;
        }

        // Input tensor: [1, 9]
        int64_t input_dims[] = {1, 9};
        TF_Tensor* input_tensor = TF_AllocateTensor(TF_FLOAT, input_dims, 2, sizeof(float) * 9);
        float* input_data = static_cast<float*>(TF_TensorData(input_tensor));
        for (int i = 0; i < 9; ++i) {
            input_data[i] = input_board[i];
        }

        // Output tensor
        int64_t output_dims[] = {1, 9};
        TF_Tensor* output_tensor = nullptr;

        const char* input_name = "serving_default_dense_input";   // Adjust if needed
        const char* output_name = "StatefulPartitionedCall";      // Adjust if needed

        TF_Output inputs[1] = {{TF_GraphOperationByName(graph, input_name), 0}};
        TF_Output outputs[1] = {{TF_GraphOperationByName(graph, output_name), 0}};

        if (!inputs[0].oper || !outputs[0].oper) {
            std::cerr << "❌ Invalid tensor name!\n";
            std::cerr << "   Input '" << input_name << "': " << (inputs[0].oper ? "FOUND" : "NOT FOUND") << "\n";
            std::cerr << "   Output '" << output_name << "': " << (outputs[0].oper ? "FOUND" : "NOT FOUND") << "\n";
            std::cerr << "💡 Use Netron.app to inspect saved_model.pb and get correct names.\n";
            TF_DeleteTensor(input_tensor);
            return false;
        }

        TF_SessionRun(session,
                      nullptr,
                      inputs, &input_tensor, 1,
                      outputs, &output_tensor, 1,
                      nullptr, 0,
                      nullptr, status);

        if (TF_GetCode(status) != TF_OK) {
            std::cerr << "❌ Inference failed: " << TF_Message(status) << "\n";
            TF_DeleteTensor(input_tensor);
            return false;
        }

        float* probs = static_cast<float*>(TF_TensorData(output_tensor));

        // Find valid moves
        std::vector<int> validMoves;
        for (int i = 0; i < 9; ++i) {
            if (input_board[i] == 0.0f) {
                validMoves.push_back(i);
            }
        }

        if (validMoves.empty()) {
            TF_DeleteTensor(input_tensor);
            TF_DeleteTensor(output_tensor);
            return false;
        }

        // === SOFTMAX SAMPLING WITH TEMPERATURE ===
        const float temperature = 1.5f; // >1.0 = more random, <1.0 = more greedy
        std::vector<float> exp_probs;
        float sum = 0.0f;

        for (int idx : validMoves) {
            float p = std::exp(probs[idx] / temperature);
            exp_probs.push_back(p);
            sum += p;
        }

        // Normalize
        for (float& p : exp_probs) p /= sum;

        // Sample move
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<> dist(exp_probs.begin(), exp_probs.end());

        best_move = validMoves[dist(gen)];

        TF_DeleteTensor(input_tensor);
        TF_DeleteTensor(output_tensor);
        return true;
    }

    ~TensorFlowTicTacToe() {
        if (session) TF_CloseSession(session, status);
        if (session) TF_DeleteSession(session, status);
        if (graph) TF_DeleteGraph(graph);
        if (status) TF_DeleteStatus(status);
    }
};

// ----------------------------
// Game Logic: TicTacToe Board
// ----------------------------

struct TicTacToe {
public:
    std::vector<int> board{0, 0, 0, 0, 0, 0, 0, 0, 0}; // 0=empty, 1=X, -1=O

    void print() {
        for (int i = 0; i < 9; ++i) {
            char c = '.';
            if (board[i] == 1) c = 'X';
            else if (board[i] == -1) c = 'O';
            std::cout << c;
            if ((i+1) % 3 == 0) std::cout << std::endl;
        }
        std::cout << "---\n";
    }

    bool isGameOver(int& winner) {
        const int wins[8][3] = {
            {0,1,2}, {3,4,5}, {6,7,8}, // rows
            {0,3,6}, {1,4,7}, {2,5,8}, // cols
            {0,4,8}, {2,4,6}           // diagonals
        };

        for (auto& w : wins) {
            if (board[w[0]] != 0 && board[w[0]] == board[w[1]] && board[w[1]] == board[w[2]]) {
                winner = board[w[0]];
                return true;
            }
        }

        // Draw?
        if (std::find(board.begin(), board.end(), 0) == board.end()) {
            winner = 0;
            return true;
        }

        winner = 0;
        return false;
    }

    std::vector<int> getValidMoves() {
        std::vector<int> moves;
        for (int i = 0; i < 9; ++i)
            if (board[i] == 0) moves.push_back(i);
        return moves;
    }

    void reset() {
        board.assign(9, 0);
    }
};

// ----------------------------
// Neural Network Stub
// ----------------------------

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);
std::uniform_int_distribution<>  moveDis(0, 8);

class NeuralNetwork {
public:
 
    std::vector<double> input, hidden, output;
    std::vector<std::vector<double>> weights_ih, weights_ho;
    std::vector<double> bias_h, bias_o;
    double learningRate = 0.1;

    NeuralNetwork(int inputSize, int hiddenSize, int outputSize)
        : input(inputSize), hidden(hiddenSize), output(outputSize),
          weights_ih(hiddenSize, std::vector<double>(inputSize)),
          weights_ho(outputSize, std::vector<double>(hiddenSize)),
          bias_h(hiddenSize), bias_o(outputSize) {

        // Initialize weights and biases randomly
        for (auto& row : weights_ih)
            for (double& w : row)
                w = dis(gen) * 2.0 - 1.0;

        for (auto& row : weights_ho)
            for (double& w : row)
                w = dis(gen) * 2.0 - 1.0;

        for (double& b : bias_h)
            b = dis(gen) * 2.0 - 1.0;

        for (double& b : bias_o)
            b = dis(gen) * 2.0 - 1.0;
    }

	// Sigmoid function
	double sigmoid(double x) {
	    return 1.0 / (1.0 + exp(-std::max(-700.0, std::min(700.0, x))));
	}
	
	// Derivative of sigmoid
	double sigmoidDerivative(double x) {
	    double s = sigmoid(x);
	    return s * (1 - s);
	}

    void forward(const std::vector<double>& x) {
        input = x;
        for (int i = 0; i < hidden.size(); ++i) {
            double activation = bias_h[i];
            for (int j = 0; j < input.size(); ++j) {
                activation += weights_ih[i][j] * input[j];
            }
            hidden[i] = sigmoid(activation);
        }

        for (int i = 0; i < output.size(); ++i) {
            double activation = bias_o[i];
            for (int j = 0; j < hidden.size(); ++j) {
                activation += weights_ho[i][j] * hidden[j];
            }
            output[i] = sigmoid(activation);
        }
    }

    void backprop(const std::vector<double>& target) {
        std::vector<double> outputError(output.size());
        std::vector<double> hiddenError(hidden.size());

        // Output layer gradients
        std::vector<double> grad_output(output.size());
        for (int i = 0; i < output.size(); ++i) {
            double error = target[i] - output[i];
            grad_output[i] = error * sigmoidDerivative(output[i]);
        }

        // Hidden layer gradients
        std::vector<double> grad_hidden(hidden.size(), 0);
        for (int i = 0; i < hidden.size(); ++i) {
            for (int j = 0; j < output.size(); ++j) {
                grad_hidden[i] += grad_output[j] * weights_ho[j][i];
            }
            grad_hidden[i] *= sigmoidDerivative(hidden[i]);
        }

        // Update weights and biases (output layer)
        for (int i = 0; i < output.size(); ++i) {
            bias_o[i] += learningRate * grad_output[i];
            for (int j = 0; j < hidden.size(); ++j) {
                weights_ho[i][j] += learningRate * grad_output[i] * hidden[j];
            }
        }

        // Update weights and biases (hidden layer)
        for (int i = 0; i < hidden.size(); ++i) {
            bias_h[i] += learningRate * grad_hidden[i];
            for (int j = 0; j < input.size(); ++j) {
                weights_ih[i][j] += learningRate * grad_hidden[i] * input[j];
            }
        }
    }
    
    //  ADD THE saveModel FUNCTION HERE, INSIDE THE CLASS
	bool saveModel(const std::string& filename) {
	        std::ofstream file(filename);
	        if (!file.is_open()) {
	            return false;
	        }
	
	        auto writeMatrix = [&](const std::vector<std::vector<double>>& mat) {
	            file << mat.size() << " " << mat[0].size() << "\n";
	            for (const auto& row : mat) {
	                for (double w : row) {
	                    file << std::setprecision(10) << w << " ";
	                }
	                file << "\n";
	            }
	        };
	
	        auto writeVector = [&](const std::vector<double>& vec) {
	            file << vec.size() << "\n";
	            for (double v : vec) {
	                file << std::setprecision(10) << v << " ";
	            }
	            file << "\n";
	        };
	
	        writeMatrix(weights_ih);
	        writeMatrix(weights_ho);
	        writeVector(bias_h);
	        writeVector(bias_o);
	
	        file.close();
	        return true;
	    }
    //
    // Loads the model from a text file
	bool loadModel(const std::string& filename) {
	    std::ifstream file(filename);
	    if (!file.is_open()) {
	        return false; // File not found or can't open
	    }
	
	    auto readMatrix = [&](std::vector<std::vector<double>>& mat) -> bool {
	        int rows, cols;
	        if (!(file >> rows >> cols)) return false;
	        mat.resize(rows, std::vector<double>(cols));
	        for (auto& row : mat) {
	            for (double& w : row) {
	                if (!(file >> w)) return false;
	            }
	        }
	        return true;
	    };
	
	    auto readVector = [&](std::vector<double>& vec) -> bool {
	        int size;
	        if (!(file >> size)) return false;
	        vec.resize(size);
	        for (double& v : vec) {
	            if (!(file >> v)) return false;
	        }
	        return true;
	    };
	
	    // Read in the same order as saveModel()
	    if (!readMatrix(weights_ih)) goto fail;
	    if (!readMatrix(weights_ho)) goto fail;
	    if (!readVector(bias_h))     goto fail;
	    if (!readVector(bias_o))     goto fail;
	
	    file.close();
	    return true;
	
	fail:
	    file.close();
	    return false;
	}
};

//-----------------------------
// utilities
//-----------------------------

// Convert board to network input (-1, 0, 1) -> (double)
std::vector<double> boardToInput(const std::vector<int>& board) {
    std::vector<double> input;
    for (int cell : board) {
        input.push_back(static_cast<double>(cell));
    }
    return input;
}

// Mask invalid outputs (already taken spots)
void maskOutputs(std::vector<double>& output, const TicTacToe& game) {
    for (int i = 0; i < 9; ++i) {
        if (game.board[i] != 0) {
            output[i] = -1e9; // effectively zero after softmax-like selection
        }
    }
}

// Choose action: pick highest scoring valid move
int selectMove(const std::vector<double>& output, const TicTacToe& game) {
    std::vector<double> masked = output;
    maskOutputs(masked, game);

    return std::distance(masked.begin(), std::max_element(masked.begin(), masked.end()));
}

//
void trainStep(NeuralNetwork& net) { 
    TicTacToe game;
    std::vector<std::pair<std::vector<double>, std::vector<double>>> history; // (state, move_prob)

    int turn = 1; // 1 = X (network), -1 = O (network too)
    while (true) {
        std::vector<double> input = boardToInput(game.board);
        net.forward(input);

        auto validMoves = game.getValidMoves();
        if (validMoves.empty()) break;

        int move = selectMove(net.output, game);

        // Store state and raw output before masking
        history.push_back({input, net.output});

        game.board[move] = turn;

        int winner;
        if (game.isGameOver(winner)) {
            // Generate targets based on outcome
            for (auto& [state, probs] : history) {
                std::vector<double> target(9, 0.0);
                if (turn == 1) {
                    // Last move was winning/drawing
                    if (winner == 1) target[move] = 1.0;   // win
                    else if (winner == -1) target[move] = -1.0; // loss
                    else target[move] = 0.5;             // draw
                } else {
                    if (winner == -1) target[move] = 1.0;
                    else if (winner == 1) target[move] = -1.0;
                    else target[move] = 0.5;
                }

                // Simple TD-style update: reinforce final result
                net.backprop(target);
            }
            break;
        }

        turn = -turn; // switch player
    }
	
}

//
std::vector<double> boardToInput(const int board[9]) {
    std::vector<double> input(9);
    for (int i = 0; i < 9; ++i) {
        input[i] = static_cast<double>(board[i]);
    }
    return input;
}

//
std::vector<double> softmax(const std::vector<double>& logits, double temp) {
	//
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

//
int selectGreedy(const std::vector<double>& scores, const TicTacToe& game) {
    std::vector<std::pair<double, int>> candidates;
    for (int i = 0; i < 9; ++i) {
        if (game.board[i] == 0) {
            candidates.emplace_back(scores[i], i);
        }
    }
    if (candidates.empty()) return -1;
    return std::max_element(candidates.begin(), candidates.end())->second;
}

//
int selectSampled(const std::vector<double>& probs, const TicTacToe& game) {
    std::vector<int> valid;
    std::vector<double> weights;
    for (int i = 0; i < 9; ++i) {
        if (game.board[i] == 0) {
            valid.push_back(i);
            weights.push_back(probs[i]);
        }
    }
    if (valid.empty()) return -1;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return valid[dist(gen)];
}

//
int selectRandomMove(TicTacToe& game) {
    auto valid = game.getValidMoves();
    if (valid.empty()) return -1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(valid.size()) - 1);
    return valid[dist(gen)];
}

// ----------------------------
// Deterministic Minimax AI
// ----------------------------

int minimax(const int board[9], int depth, bool isMaximizing, int alpha = -1000, int beta = 1000) {
    const int wins[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
    };

    for (const auto& w : wins) {
        if (board[w[0]] != 0 &&
            board[w[0]] == board[w[1]] &&
            board[w[1]] == board[w[2]]) {
            return board[w[0]] == 1 ? 10 - depth : depth - 10;
        }
    }

    bool isFull = true;
    for (int i = 0; i < 9; ++i) {
        if (board[i] == 0) {
            isFull = false;
            break;
        }
    }
    if (isFull) return 0;

    if (isMaximizing) {
        int best = -1000;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) {
                int temp[9];
                for (int j = 0; j < 9; ++j) temp[j] = board[j];
                temp[i] = 1;
                int score = minimax(temp, depth + 1, false, alpha, beta);
                best = std::max(best, score);
                alpha = std::max(alpha, score);
                if (beta <= alpha) break;
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) {
                int temp[9];
                for (int j = 0; j < 9; ++j) temp[j] = board[j];
                temp[i] = -1;
                int score = minimax(temp, depth + 1, true, alpha, beta);
                best = std::min(best, score);
                beta = std::min(beta, score);
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
            int temp[9];
            for (int j = 0; j < 9; ++j) temp[j] = board[j];
            temp[i] = player;

            int moveValue = minimax(temp, 0, player == -1);

            if (player == 1 && moveValue > bestValue) {
                bestValue = moveValue;
                bestMove = i;
            }
            if (player == -1 && moveValue < bestValue) {
                bestValue = moveValue;
                bestMove = i;
            }
        }
    }

    if (bestMove == -1) {
        for (int i = 0; i < 9; ++i) {
            if (board[i] == 0) return i;
        }
    }
    return bestMove;
}

// ----------------------------
// Main Move Selector
// ----------------------------

int selectMove(const std::vector<double>& output, TicTacToe& game, int aiMode, double temperature) {

    if (aiMode == MINIMAX || aiMode == TENSORFLOW) {
        return -1;
    }

    auto probs = softmax(output, temperature);

    switch (aiMode) {
        case RANDOM:
	        return selectRandomMove(game);
        case EXPERT:
            return selectGreedy(probs, game);
        case CREATIVE:
            return selectSampled(probs, game);
        default:
            return selectGreedy(probs, game);
    }
}

// ----------------------------
// Main Program Entrance
// ----------------------------

bool RunTicTacToeSelfPlay(TicTacToeResultOnline& result, int aiMode, double temperature) {
    //////////////////////////////////////////////////////
    // VARIABLE INITIALIZATION
    //////////////////////////////////////////////////////

	//
    TensorFlowTicTacToe tf;
	NeuralNetwork       netStandalone(9, 18, 9);;		
    TicTacToe game;

   	//
   	std::random_device              rd;
    std::mt19937                    gen(rd());
    std::uniform_int_distribution<> starter(0, 1);
    int                             turn = (starter(gen) == 0) ? 1 : -1;

	//
    int              winner;
	int              move = -1;
    std::vector<int> moves;

	//
    for (int i = 0; i < 9; ++i) {
        result.history[0][i] = game.board[i];
    }
    result.historyCount = 1;

    //////////////////////////////////////////////////////
    // MODEL LOAD 
    //////////////////////////////////////////////////////

    if (aiMode == TENSORFLOW) {
	    
	    // load model
	    const char* modelFile = "tictactoe_tf_model";
		
		//	
		if (!tf.LoadModel(modelFile)) {
	        std::cerr << "❌ Failed to initialize TensorFlow model.\n";
	        return false;
	    }
	    
	} 
	else 
	{
		// Load model
	    const std::string modelFile = "tictactoe_model.txt";
	
		//
	    if (aiMode != MINIMAX && !netStandalone.loadModel(modelFile)) {
	     	
			//
	        for (int i = 0; i < 5000; ++i) 
				trainStep(netStandalone);
	        //
			netStandalone.saveModel(modelFile);
	    }
	}  

    //////////////////////////////////////////////////////
    // GAME LOOP
    //////////////////////////////////////////////////////
   
	//
    while (true) {

		///////////////////////////////////////////
		// DECIDE MOVE
		///////////////////////////////////////////
		if (aiMode == TENSORFLOW) {
		 	
            float input[9];
			for (int i = 0; i < 9; ++i) input[i] = static_cast<float>(game.board[i]);
		        if (!tf.PredictBestMove(input, move)) {
		            std::cerr << "❌ Prediction failed!\n";
		            return false;
		        }
		        
		} else if (aiMode == MINIMAX) {
            move = minimaxMove(game.board, turn);
        } else {
            std::vector<double> input = boardToInput(game.board);
            netStandalone.forward(input);
            move = selectMove(netStandalone.output, game, aiMode, temperature);
        }

		///////////////////////////////////////////
		// VALIDATE MOVE
		///////////////////////////////////////////
        
		if (move < 0 || move >= 9 || game.board[move] != 0) {
            auto valid = game.getValidMoves();
            if (valid.empty()) break;
            move = valid[0];
        }

		///////////////////////////////////////////
		// WRITE ON HISTORY
		///////////////////////////////////////////

		//
        game.board[move] = turn;
        moves.push_back(move);

		//
        if (result.historyCount < 10) {
            for (int i = 0; i < 9; ++i) {
                result.history[result.historyCount][i] = game.board[i];
            }
            result.historyCount++;
        }

		///////////////////////////////////////////
		// DECIDE WINNER
		///////////////////////////////////////////

        if (game.isGameOver(winner)) {
            //
            break;
        }
        
        //
        turn = -turn;
	}	

	
	//////////////////////////////////////////////////////
    // RETURN FINAL RESULT
    //////////////////////////////////////////////////////
   
	result.winner = winner;  
    for (int i = 0; i < 9; ++i) {
        result.finalBoard[i] = game.board[i];
        result.moves[i] = (i < static_cast<int>(moves.size())) ? moves[i] : -1;
    }
    result.moveCount          = static_cast<int>(moves.size());
	
	//
    return true;
}



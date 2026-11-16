#include <stdio.h>
#include <math.h>
#include <ostream>
#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <cstring>
#include "TFileManager.h"

using namespace std;

/* C++ program for Sudoku generator */
// This code is contributed by Aarti_Rathi

class SudokuGenerator {

public:
	//
	int** mat;
	int N;
	// number of columns/rows.
	int SRN;
	// square root of N
	int K;
	// No. Of missing digits
	// Constructor
	SudokuGenerator(int N, int K)
	{
		//
		this->N = N;
		this->K = K;
		// Compute square root of N
		double SRNd = sqrt(N);
		SRN = (int)SRNd;
		mat = new int*[N];
		// Create a row for every pointer
		for (int i = 0; i < N; i++)
		{
			// Note : Rows may not be contiguous
			mat[i] = new int[N];
			// Initialize all entries as false to indicate
			// that there are no edges initially
			memset(mat[i], 0, N * sizeof(int));
		}
	}
	// Sudoku Generator
	void fillValues()
	{
		// Fill the diagonal of SRN x SRN matrices
		fillDiagonal();
		// Fill remaining blocks
		fillRemaining(0, SRN);
		// Remove Randomly K digits to make game
		removeKDigits();
	}
	// Fill the diagonal SRN number of SRN x SRN matrices
	void fillDiagonal()
	{
		for (int i = 0; i < N; i = i + SRN)
		{
			// for diagonal box, start coordinates->i==j
			fillBox(i, i);
		}
	}
	// Returns false if given 3 x 3 block contains num.
	bool unUsedInBox(int rowStart, int colStart, int num)
	{
		for (int i = 0; i < SRN; i++) {
			for (int j = 0; j < SRN; j++) {
				if (mat[rowStart + i][colStart + j]
					== num) {
					return false;
				}
			}
		}
		return true;
	}
	// Fill a 3 x 3 matrix.
	void fillBox(int row, int col)
	{
		int num;
		for (int i = 0; i < SRN; i++) {
			for (int j = 0; j < SRN; j++) {
				do {
					num = randomGenerator(N);
				} while (!unUsedInBox(row, col, num));
				mat[row + i][col + j] = num;
			}
		}
	}
	// Random generator
	int randomGenerator(int num)
	{
		//
		unsigned                           seed         = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine         generator(seed);
		std::uniform_int_distribution<int> distribution(1, num);
		int                                randomValue  = distribution(generator);

		//
		return randomValue;
	}
	// Check if safe to put in cell
	bool CheckIfSafe(int i, int j, int num)
	{
		return (
			unUsedInRow(i, num) && unUsedInCol(j, num)
			&& unUsedInBox(i - i % SRN, j - j % SRN, num));
	}
	// check in the row for existence
	bool unUsedInRow(int i, int num)
	{
		for (int j = 0; j < N; j++) {
			if (mat[i][j] == num) {
				return false;
			}
		}
		return true;
	}
	// check in the row for existence
	bool unUsedInCol(int j, int num)
	{
		for (int i = 0; i < N; i++) {
			if (mat[i][j] == num) {
				return false;
			}
		}
		return true;
	}
	// A recursive function to fill remaining matrix
	bool fillRemaining(int i, int j)
	{
		// System.out.println(i+" "+j);
		if (j >= N && i < N - 1) {
			i = i + 1;
			j = 0;
		}
		if (i >= N && j >= N) {
			return true;
		}
		if (i < SRN) {
			if (j < SRN) {
				j = SRN;
			}
		}
		else if (i < N - SRN) {
			if (j == (int)(i / SRN) * SRN) {
				j = j + SRN;
			}
		}
		else {
			if (j == N - SRN) {
				i = i + 1;
				j = 0;
				if (i >= N) {
					return true;
				}
			}
		}
		for (int num = 1; num <= N; num++) {
			if (CheckIfSafe(i, j, num)) {
				mat[i][j] = num;
				if (fillRemaining(i, j + 1)) {
					return true;
				}
				mat[i][j] = 0;
			}
		}
		return false;
	}
	// Remove the K no. of digits to complete game
	void removeKDigits()
	{
		int count = K;
		while (count != 0) {
			int cellId = randomGenerator(N * N) - 1;
			// extract coordinates i and j
			int i = (cellId / N);
			int j = cellId % N;
			if (j != 0) {
				j = j - 1;
			}
			//
			if (mat[i][j] != 0) {
				count--;
				mat[i][j] = 0;
			}
		}
	}
	// Print sudoku
	string printSudoku()
	{
		//
		cout << endl << "Sudoku to Solve : "  << endl;
		//
		cout << endl;
		//
		string string_matrix     = "";

		//
		FileManager* fileManager = new FileManager();
		fileManager->SaveLineToFile("\nSudoku To Solve \n","Sudoku.txt");

		//
		for (int i = 0; i < N; i++)
		{
			string row = "{";
			for (int j = 0; j < N; j++)
			{
				//
				string cell = std::to_string(mat[i][j]);
				//
				cout << cell << " ";
				//
				row += (cell + ((j < (N-1))?  "," : ""));
			}
			//
			row += ((i < (N-1))?  "}," : "}");
			//
			string_matrix += row;
			//
			cout << endl;
			//
			fileManager->SaveLineToFile(row,"Sudoku.txt");
		}
		//
		string_matrix = "[" + string_matrix  + "]";
		//
		cout << endl;
		//
		return string_matrix;
	}
	// Driver code
	string Run()
	{
		//
		this->fillValues();

		//
		string str_matrix =	this->printSudoku();

		//
		return str_matrix;
	}
};

/* C++ program for Sudoku Solving */

class SudokuSolver
{
	public:

		// Size of Sudoku grid
		const static int N = 9;

		//
		SudokuSolver()
		{

		}

		//
		~SudokuSolver()
		{

		}

		// Helper function to check if a value is safe to be placed at a given position
		bool isSafe(int grid[N][N], int row, int col, int num) {
			// Check if the same number exists in the row or column
			for (int x = 0; x < N; ++x) {
				if (grid[row][x] == num || grid[x][col] == num) {
					return false;
				}
			}
			// Check if the number exists in the 3x3 subgrid
			int startRow = row - row % 3;
			int startCol = col - col % 3;
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					if (grid[i + startRow][j + startCol] == num) {
						return false;
					}
				}
			}
			return true; // Number can be placed safely
		}

		// Recursive function to solve the Sudoku puzzle
		bool solveSudoku(int grid[N][N]) {
			int row = -1, col = -1;
			bool isEmpty = false;
			// Find the first empty cell in the grid
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					if (grid[i][j] == 0) {
						row = i;
						col = j;
						isEmpty = true;
						break;
					}
				}
				if (isEmpty) {
					break;
				}
			}
			// If there is no empty cell, the puzzle is solved
			if (!isEmpty) {
				return true;
			}
			// Try placing numbers from 1 to 9 at the empty cell
			for (int num = 1; num <= 9; ++num) {
				if (isSafe(grid, row, col, num)) {
					grid[row][col] = num;
					if (solveSudoku(grid)) {
						return true; // Recursively solve the rest of the puzzle
					}
					// If the current placement doesn't lead to a solution, backtrack
					grid[row][col] = 0;
				}
			}
			return false; // Backtrack to previous step
		}

		// Function to print the solved Sudoku grid
		string printGrid(int grid[N][N]) {
			//
			cout << endl;
			//
			string string_matrix     = "";
			//
			FileManager* fileManager = new FileManager();
			fileManager->SaveLineToFile("\nSudoku Solved\n","Sudoku.txt");
			//
			for (int i = 0; i < N; ++i) {
				//
				string row = "{";
				//
				for (int j = 0; j < N; ++j) {
					//
					string cell = std::to_string(grid[i][j]);
					//
					cout << cell << " ";
					//
					row += (cell + ((j < (N-1))?  "," : ""));
				}
				//
				row += ((i < (N-1))?  "}," : "}");
				//
				string_matrix += row;
				//
				cout << endl;
				//
				fileManager->SaveLineToFile(row,"Sudoku.txt");
			}
			//
			string_matrix = "[" + string_matrix  + "]";
			//
			cout << endl;
			//
			return string_matrix;
		}

		//
		int Run()
		{
			/*
			{{0, 4, 0, 1, 0, 2, 6, 5, 7},
			{2, 7, 3, 6, 8, 5, 4, 1, 9},
			{0, 6, 0, 9, 0, 4, 2, 8, 3},
			{0, 9, 0, 3, 2, 8, 7, 0, 5},
			{0, 5, 7, 4, 0, 9, 0, 6, 2},
			{4, 2, 8, 5, 6, 7, 3, 9, 1},
			{0, 3, 2, 0, 0, 1, 0, 7, 4},
			{7, 1, 4, 2, 0, 6, 9, 3, 8},
			{0, 8, 0, 7, 4, 0, 1, 2, 6}};
			*/
			
			// Example Sudoku grid (0 represents empty cells)
			int grid[N][N] =
			   {{0, 4, 0, 1, 0, 2, 6, 5, 7},
				{2, 7, 3, 6, 8, 5, 4, 1, 9},
				{0, 6, 0, 9, 0, 4, 2, 8, 3},
				{0, 9, 0, 3, 2, 8, 7, 0, 5},
				{0, 5, 7, 4, 0, 9, 0, 6, 2},
				{4, 2, 8, 5, 6, 7, 3, 9, 1},
				{0, 3, 2, 0, 0, 1, 0, 7, 4},
				{7, 1, 4, 2, 0, 6, 9, 3, 8},
				{0, 8, 0, 7, 4, 0, 1, 2, 6}};
			//
			if (solveSudoku(grid)) {
				cout << "Sudoku solved:\n" << endl;
				printGrid(grid);
			} else {
				cout << "No solution exists.\n";
			}
			//
			return 0;
		};

		//
		string Solve(int grid[N][N])
		{
			// Ej:
			// [{2,0,8,5,1,3,4,6,7},{0,3,6,2,4,0,1,9,8},{0,0,0,8,6,0,2,5,3},{3,2,5,4,7,6,0,1,9},{0,6,9,1,8,2,3,7,5},{8,7,0,3,0,5,0,0,2},{0,4,7,9,2,8,0,3,1},{1,8,0,7,5,4,0,2,6},{9,5,2,0,0,1,7,0,4}]
	
			//
			string str_matrix = "";
			//
			if (solveSudoku(grid)) {
				cout << endl <<  "Sudoku solved :" << endl;
				str_matrix = printGrid(grid);
			} else {
				cout << "No solution exists.\n";
			}
			//
			return str_matrix;
		};
};


/*

	Generated Board:

	0 4 0 1 0 2 6 5 7
	2 7 3 6 8 5 4 1 9
	0 6 0 9 0 4 2 8 3
	0 9 0 3 2 8 7 0 5
	0 5 7 4 0 9 0 6 2
	4 2 8 5 6 7 3 9 1
	0 3 2 0 0 1 0 7 4
	7 1 4 2 0 6 9 3 8
	0 8 0 7 4 0 1 2 6

	Sudoku solved:

	8 4 9 1 3 2 6 5 7
	2 7 3 6 8 5 4 1 9
	5 6 1 9 7 4 2 8 3
	1 9 6 3 2 8 7 4 5
	3 5 7 4 1 9 8 6 2
	4 2 8 5 6 7 3 9 1
	6 3 2 8 9 1 5 7 4
	7 1 4 2 5 6 9 3 8
	9 8 5 7 4 3 1 2 6

*/


#include <iostream>
#include <thread>
#include <cmath>
#include <vector>
#include <random>
#include <cassert>

using namespace std;
static bool primitive_semaphore = false;


vector<vector<int>> create_matrix(int rows, int columns)
{
    // Pre-allocate matrix
    vector<vector<int>> result(rows, std::vector<int>(columns));
    for (int i = 0; i < rows; i++ )
    {   
        for (int j =0; j < columns; j++)
        {
            // Generate a random integer between 0-9;
            result[i][j] = rand() % 10;
        }
    }
}


void calculate_matrix_product_at_index(int row, int column, const vector<vector<int>>& A, vector<vector<int>>)
{

}

vector<vector<int>> multi_threaded_matrix_multiplication(vector<vector<int>>& A, vector<vector<int>>& B)
{
    // Check if number of columns in A is equal to number of rows in B
    if(A[0].size() != B.size())
    {
        cout << "ERROR: The dimensions of the matrices do not match!\n";
    }

    vector<thread> thread_storage()
}


int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cout << "Expected call of the format multi-threaded-matrix <p> <q> <r>!\n";
        return 0;
    }

    // Extract p,q,r from the input args
    int p = atoi(argv[1]);
    int q = atoi(argv[2]);
    int r = atoi(argv[3]);

    // Make sure our parameters are within range
    assert(p > 0 && p < 100);
    assert(q > 0 && q < 100);
    assert(r > 0 && r < 100);

    vector<vector<int>> A = create_matrix(p, q);
    vector<vector<int>> B = create_matrix(q, r);

    vector<vector<int>> C = multi_threaded_matrix_multiplication(A, B);

    return 1;

}


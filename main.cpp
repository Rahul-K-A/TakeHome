#include <iostream>
#include <pthread.h>
#include <cmath>
#include <vector>
#include <random>
#include <cassert>
#include <semaphore.h>

using namespace std;
static bool primitive_semaphore = false;


// Instructions for the thread
struct instruction_data
{
    size_t row;
    size_t column;
};

// Argument to be passed into each thread
struct multi_threaded_input_arg
{
    size_t id;
};



vector<sem_t> semaphores;
vector<multi_threaded_input_arg> input_arg;
vector<pthread_t> thread_storage;
vector<instruction_data> instructions;

vector<vector<int>> A,B, C;

void create_matrix(vector<vector<int>>& result, int rows, int columns)
{
    // Pre-allocate matrix
    result.resize(rows, vector<int>(columns));
    
    for (int i = 0; i < rows; i++ )
    {   
        for (int j =0; j < columns; j++)
        {
            // Generate a random integer between 0-9;
            result[i][j] = rand() % 10;
        }
    }
}


void* calculate_matrix_product_at_index(void* parameters)
{
    multi_threaded_input_arg* arg = static_cast<multi_threaded_input_arg*>(parameters);

    sem_wait(&semaphores[arg->id]);


    size_t row = instructions[arg->id].row;
    size_t column = instructions[arg->id].column;

    int matrix_product = 0;
    size_t current_b_row = 0;

    for (size_t i = 0; i < A[row].size(); i++)
    {
        matrix_product = matrix_product + A[row][i] * B[current_b_row][column];
        current_b_row++;
    }

    C[row][column] = matrix_product;
    return nullptr;
}

void multi_threaded_matrix_multiplication()
{
    // Basic idea: Have semaphores and instructions for each thread
    // The question specifies that instructions must be sent from the main thread to the executing thread
    // So initialize each thread, create an associated semaphore, and make the thread block on the semaphore
    // Till the main thread fills in the instructions
    // Once the main thread has filled in the instructions, release the semaphore

    // Check if number of columns in A is equal to number of rows in B (just in case)
    if(A[0].size() != B.size())
    {
        cout << "ERROR: The dimensions of the matrices do not match!\n";
    }

    int product_rows = A.size();
    int product_cols = B[0].size();

    C.resize(product_rows, vector<int>(product_cols));

    thread_storage.resize(product_rows * product_cols);
    input_arg.resize(product_rows * product_cols);
    semaphores.resize(product_rows * product_cols);
    instructions.resize(product_rows * product_cols);

    for (size_t i = 0; i < thread_storage.size(); i++)
    {
        // Give each thread an ID with which it can access a semaphore and an instruction
        input_arg[i].id = i;
        assert( sem_init(&semaphores[i], 0, 0) == 0 );

        pthread_create(&thread_storage[i], NULL, &calculate_matrix_product_at_index, &input_arg[i]);
    }

    for (size_t i = 0; i < product_rows; i++)
    {
        for (size_t j = 0; j < product_cols; j++)
        {
            // Fill in instructions and release the semaphore
            size_t current_index = i*product_rows + j;
            instructions[current_index].row = i;
            instructions[current_index].column = j;
            sem_post(&semaphores[current_index]);
        }
    }

    // Wait till all the threads finish executing
    for (size_t i = 0; i < thread_storage.size(); i++)
    {
        pthread_join(thread_storage[i], nullptr);
    }

}


void print_matrix(vector<vector<int>>& mat)
{
    for(size_t i =0 ; i < mat.size(); i++ )
    {
        for(size_t j = 0; j < mat[0].size(); j++)
        {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        cout << "Expected call of the format multi-threaded-matrix <p> <q> <r>!\n";
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

    // Create matrices
    create_matrix(A, p, q);
    create_matrix(B, q, r);

    cout << "MATRIX A:\n";
    print_matrix(A);

    cout << "MATRIX B:\n";
    print_matrix(B);


    multi_threaded_matrix_multiplication();

    cout << "MATRIX C:\n";
    print_matrix(C);

    return 1;

}


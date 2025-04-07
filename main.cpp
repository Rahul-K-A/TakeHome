#include <iostream>
#include <pthread.h>
#include <cmath>
#include <vector>
#include <random>
#include <cassert>
#include <semaphore.h>

using namespace std;
static bool primitive_semaphore = false;

struct multi_threaded_input_arg
{
    const vector<vector<int>>* A;
    const vector<vector<int>>* B;
    size_t semaphore_id;
    multi_threaded_input_arg() :  A(nullptr), B(nullptr){}; 
};


struct multi_threaded_message
{
    size_t row;
    size_t column;
};


vector<sem_t> semaphores;
vector<multi_threaded_input_arg> input_arg;
vector<pthread_t> thread_storage;

vector<vector<int>> A,B,C;

void create_matrix(vector<vector<int>>& result, int rows, int columns)
{
    // Pre-allocate matrix
    result.resize(rows, std::vector<int>(columns));

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

    sem_wait(&semaphores[arg->semaphore_id]);
    std::cout << "Hello\n";
    return nullptr;
}

vector<vector<int>> multi_threaded_matrix_multiplication(vector<vector<int>>& A, vector<vector<int>>& B)
{
    // Check if number of columns in A is equal to number of rows in B
    if(A[0].size() != B.size())
    {
        cout << "ERROR: The dimensions of the matrices do not match!\n";
    }

    int product_rows = A.size();
    int product_cols = B[0].size();

    std::cout << "Product Rows " << product_rows << std::endl;
    std::cout << "Product Cols " << product_cols << std::endl;

    C.resize(product_rows * product_cols);
    thread_storage.resize(product_rows * product_cols);
    vector<vector<int>> result(product_rows * product_cols);
    input_arg.resize(product_rows * product_cols);
    semaphores.resize(product_rows * product_cols);

    for (size_t i = 0; i < thread_storage.size(); i++)
    {
        // Give each thread an
        input_arg[i].A = &A;
        input_arg[i].B = &B;
        input_arg[i].semaphore_id = i;
        assert( sem_init(&semaphores[i], 0, 1) == 0 );

        pthread_create(&thread_storage[i], NULL, &calculate_matrix_product_at_index, &input_arg[i]);
    }

    for (size_t i = 0; i < thread_storage.size(); i++)
    {
        sem_post(&semaphores[i]);
        pthread_join(thread_storage[i], nullptr);
    }

    return result;
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

    create_matrix(A, p, q);
    create_matrix(B, q, r);

    multi_threaded_matrix_multiplication(A, B);

    return 1;

}


#ifndef _DATAFILE_
#include "datafile.hpp"
#endif


DataFile* read_input_files(int, char**);
void clean(DataFile*, int);
DataFile* sinusoidal_regression(DataFile*, int);
float* get_initial_guess(void);
float get_model(float, float*);
float* get_residuals(float*, float*, float*, int);
float** get_jacobian(float*, float*, int);
float* get_dfdb1(float*, float*, int);
float* get_dfdb2(float*, float*, int);
float* get_dfdb3(float*, float*, int);
float* get_dfdb4(float*, float*, int);
float** get_delta_beta(float**, float*, int);
float** make_column_vector(float*, int);
float* make_row_vector(float**, int);
float** transpose(float**, int, int);
float** matrix_multiply(float**, float**, int, int, int);
float** invert_matrix(float**, int);
void get_cofactor(float**, float**, int, int, int);
float determinant(float**, int, int);
void adjoint(float**, float**, int);
float* update_beta(float*, float**, float);
bool convergence_check(float**, float*, float, int);
int find_closest_index(float*, float, int);
DataFile* find_peaks(DataFile*, int);
DataFile* align_data(DataFile*, int);
void save_aligned(DataFile*, int);

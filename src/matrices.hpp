float** make_column_vector(float*, int);
float* make_row_vector(float**, int);
float** transpose(float**, int, int);
float** matrix_multiply(float**, float**, int, int, int);
float** invert_matrix(float**, int);
void get_cofactor(float**, float**, int, int, int);
float determinant(float**, int, int);
void adjoint(float**, float**, int);

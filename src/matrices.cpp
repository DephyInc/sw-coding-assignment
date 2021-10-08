#include "matrices.hpp"


/*********************************************
             make_column_vector
*********************************************/
float** make_column_vector(float* a, int n)
{
    /***
    Takes a row vector a with n columns and makes it a column vector
    with n rows.
    ***/
    int i;
    float** colVec;

    colVec = new float* [n];
    for (i=0; i<n; i++)
    {
        colVec[i] = new float [1];
    }

    for (i=0; i<n; i++)
    {
        colVec[i][0] = a[i];
    }

    return colVec;
}


/*********************************************
                make_row_vector
*********************************************/
float* make_row_vector(float** a, int n)
{
    /***
    Takes a column vector a with n rows and makes it a row vector
    with n columns.
    ***/
    int i;
    float* rowVec;

    rowVec = new float [n];

    for (i=0; i<n; i++)
    {
        rowVec[i] = a[i][0];
    }

    return rowVec;
}


/*********************************************
                  transpose
*********************************************/
float** transpose(float** m, int nRows, int nCols)
{
    float** mT;
    int i;
    int j;

    mT = new float* [nCols];
    for (i=0; i<nCols; i++)
    {
        mT[i] = new float [nRows];
    }
    for (i=0; i<nRows; i++)
    {
        for (j=0; j<nCols; j++)
        {
            mT[j][i] = m[i][j];
        }
    }

    return mT;
}


/*********************************************
                matrix_multiply
*********************************************/
float** matrix_multiply(float** a, float** b, int nRowsA, int nRowsB, int nColsB)
{
    int i;
    int j;
    int k;
    float val;
    float** p;

    p = new float* [nRowsA];
    for (i=0; i<nRowsA; i++)
    {
        p[i] = new float [nColsB];
    }

    for (i=0; i<nRowsA; i++)
    {
        for (j=0; j<nRowsA; j++)
        {
            val = 0.;
            for (k=0; k<nRowsB; k++)
            {
                val = val + a[i][k] * b[k][j];
            }
            p[i][j] = val;
        }
    }

    return p;
}


/*********************************************
                invert_matrix
*********************************************/
float** invert_matrix(float** A, int N)
{
    /***
    Calculates the inverse of a matrix. From:
    https://tinyurl.com/kwpp8e3c
    ***/
    int i;
    int j;
    float det;
    float** adj;
    float** inverse;

    // Find determinant of A[][]
    det = determinant(A, N, N);
 
    // Find adjoint
    adj = new float* [N];
    inverse = new float* [N];
    for(i=0; i<N; i++)
    {
        adj[i] = new float [N];
        inverse[i] = new float [N];
    }
    adjoint(A, adj, N);
 
    // Find Inverse using formula "inverse(A) = adj(A)/det(A)"
    for (i=0; i<N; i++)
    {
        for (j=0; j<N; j++)
        {
            inverse[i][j] = adj[i][j]/float(det);
        }
    }

    for (i=0; i<N; i++)
    {
        delete[] adj[i];
    }
    delete[] adj;
 
    return inverse;
}


/*********************************************
                get_cofactor
*********************************************/
void get_cofactor(float** A, float** temp, int p, int q, int n)
{
    /***
    Gets cofactor of A[p][q] in temp[][]. n is current dimension of
    A[][].
    From: https://tinyurl.com/kwpp8e3c
    ***/
    int i = 0;
    int j = 0;
    int row;
    int col;
 
    // Looping for each element of the matrix
    for (row = 0; row < n; row++)
    {
        for (col = 0; col < n; col++)
        {
            //  Copying into temporary matrix only those elements
            //  that are not in given row and column
            if (row != p && col != q)
            {
                temp[i][j++] = A[row][col];
                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

 
/*********************************************
                 determinant
*********************************************/
float determinant(float** A, int n, int N)
{
    /***
    Recursive function for finding determinant of matrix. n is current
    dimension of A[][].
    From: https://tinyurl.com/kwpp8e3c
    ***/
    float D = 0.; // Initialize result
    float** temp; // To store cofactors
    int sign = 1;  // To store sign multiplier
    int i;
    int f;

    temp = new float* [N];
    for (i=0; i<N; i++)
    {
        temp[i] = new float [N];
    }
 
    //  Base case : if matrix contains single element
    if (n == 1)
        return A[0][0];
 
    // Iterate for each element of first row
    for (f = 0; f < n; f++)
    {
        // Getting Cofactor of A[0][f]
        get_cofactor(A, temp, 0, f, n);
        D += sign * A[0][f] * determinant(temp, n - 1, N);
 
        // terms are to be added with alternate sign
        sign = -sign;
    }

    for (i=0; i<N; i++)
    {
        delete[] temp[i];
    }
    delete[] temp;
 
    return D;
}
 

/*********************************************
                   adjoint
*********************************************/
void adjoint(float** A, float** adj, int N)
{
    /***
    Gets adjoint of A[N][N] in adj[N][N].
    From: https://tinyurl.com/kwpp8e3c
    ***/
    int i;
    int j;
    int sign = 1;
    float** temp;

    temp = new float* [N];
    for (i=0; i<N; i++)
    {
        temp[i] = new float [N];
    }

    if (N == 1)
    {
        adj[0][0] = 1;
        return;
    }
 
    for (i=0; i<N; i++)
    {
        for (j=0; j<N; j++)
        {
            // Get cofactor of A[i][j]
            get_cofactor(A, temp, i, j, N);
 
            // sign of adj[j][i] positive if sum of row
            // and column indexes is even.
            sign = ((i+j)%2==0)? 1: -1;
 
            // Interchanging rows and columns to get the
            // transpose of the cofactor matrix
            adj[j][i] = (sign)*(determinant(temp, N-1, N));
        }
    }

    for (i=0; i<N; i++)
    {
        delete[] temp[i];
    }
    delete[] temp;
}

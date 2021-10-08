#include <math.h>

#include "datafile.hpp"
#include "proto.hpp"


/*********************************************
            sinusoidal_regression
*********************************************/
DataFile* sinusoidal_regression(DataFile* dataFiles, int nFiles)
{
    /***
    Uses the Gauss-Newton method to fit a sine curve to the data from
    each file.

    The Gauss-Newton method is a technique for performing ordinary
    (i.e. least-squares) regression when the model function is
    non-linear. The idea is to use an iterative procedure, staring with
    some guess, to find the model parameters Beta that give rise to a
    model that best-fits the data. Here, best-fit is taken to mean that
    the sum of the squared residuals is minimized.

    If J is the jacobian matrix such that:

    J_{ij} = \frac{\partial f(x_i, Beta)}{\partial Beta_j}

    and r is the column vector of residuals:

    r_i = y_i - f(x_i, Beta)

    and f(x,B) is the model function (sine), then the equations are:

    \Delta Beta = (J.T @ J)^{-1} @ J.T @ r
    Beta^{k+1} = Beta^k + a * \Delta Beta

    where J.T is the transpose of J and a is a damping term.

    We loop until we've either met our convergence condition or
    exceeded a maximum number of iterations.

    See: https://tinyurl.com/7t4x26b4
    ***/
    int i;
    int j;
    int nRecords;
    int curIteration;
    int maxIterations = 1000;
    float tolerance = 0.00001;
    float damping = 0.01;
    float* t;
    float* xObs;
    float* beta;
    float* r;
    float** deltaB;
    float** J;

    // Loop over each file
    for (i=0; i<nFiles; i++)
    {
        // Extract time and x acceleration
        t = dataFiles[i].extract_times();
        xObs = dataFiles[i].extract_x();
        nRecords = dataFiles[i].nRecords;
        // Reset the iteration counter
        curIteration = 0;
        // Reset Beta parameters to initial guess
        beta = get_initial_guess();
        // Perform the regression
        while (curIteration < maxIterations)
        {
            r = get_residuals(t, xObs, beta, nRecords);
            J = get_jacobian(t, beta, nRecords);
            deltaB = get_delta_beta(J, r, nRecords);
            beta = update_beta(beta, deltaB, damping);
            if (convergence_check(deltaB, beta, tolerance, 4))
            {
                break;
            }
            else
            {
                curIteration++;
            }
        }
        // Save final parameters
        dataFiles[i].save_beta(beta, 4);
        // Clean up for the next file
        delete[] t;
        delete[] xObs;
        delete[] r;
        delete[] beta;
        delete[] deltaB;
        for (j=0; j<nRecords; j++)
        {
            delete[] J[j];
        }
        delete[] J;
    }

    return dataFiles;
}


/*********************************************
              get_initial_guess
*********************************************/
float* get_initial_guess(void)
{
    /***
    Returns an array containing the initial guess values for the
    parameters beta.

    The model is a general sine wave:

    f(t, beta) = beta[0] + beta[1] * sin(beta[2] * t + beta[3])
    ***/
    return new float[4] {0.1, 0.1, 0.1, 0.1};
}


/*********************************************
                  get_model
*********************************************/
float get_model(float t, float* b)
{
    /***
    Returns the current model prediction for the given time t.

    f(t, b) = b[0] + b[1] * sin(b[2] * t + b[3])
    ***/
    return b[0] + b[1] * sin(b[2] * t + b[3]);
}


/*********************************************
                get_residuals
*********************************************/
float* get_residuals(float* t, float* obs, float* beta, int n)
{
    /***
    Calculates the residuals between the data and current model
    predictions.

    r_i = obs_i - f(x_i, beta)
    ***/
    int i;
    float model;
    float* r = new float[n];

    for (i=0; i<n; i++)
    {
        model = get_model(t[i], beta);
        r[i] = obs[i] - model;
    }

    return r;
}


/*********************************************
                 get_jacobian
*********************************************/
float** get_jacobian(float* t, float* b, int n)
{
    /***
    Calculates df(x_i)/db_j
    ***/
    int i;
    int j;
    float* dfdb1;
    float* dfdb2;
    float* dfdb3;
    float* dfdb4;
    float** J;

    // Calculate the derivatives
    dfdb1 = get_dfdb1(t, b, n);
    dfdb2 = get_dfdb2(t, b, n);
    dfdb3 = get_dfdb3(t, b, n);
    dfdb4 = get_dfdb4(t, b, n);

    // Allocate for J
    J = new float* [n];

    // Package them into a matrix
    for (i=0; i<n; i++)
    {
        J[i] = new float [4];
        for (j=0; j<4; j++)
        {
            if (j == 0)
            {
                J[i][j] = dfdb1[i];
            }
            else if (j == 1)
            {
                J[i][j] = dfdb2[i];
            }
            else if (j == 2)
            {
                J[i][j] = dfdb3[i];
            }
            else if (j == 3)
            {
                J[i][j] = dfdb4[i];
            }
        }
    }

    return J;
}


/*********************************************
                  get_dfdb1
*********************************************/
float* get_dfdb1(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[0]:

    dfdb1 = 1
    ***/
    int i;
    float* dfdb1 = new float[n];

    for (i=0; i<n; i++)
    {
        dfdb1[i] = 1.0;
    }

    return dfdb1;
}


/*********************************************
                  get_dfdb2
*********************************************/
float* get_dfdb2(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[1]:

    dfdb2 = sin(b[2] * t + b[3]
    ***/
    int i;
    float* dfdb2 = new float[n];

    for (i=0; i<n; i++)
    {
        dfdb2[i] = sin(b[2] * t[i] + b[3]);
    }

    return dfdb2;
}


/*********************************************
                  get_dfdb3
*********************************************/
float* get_dfdb3(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[2]:

    dfdb3 = b[1] * t * cos(b[2] * t + b[3]
    ***/
    int i;
    float* dfdb3 = new float[n];

    for (i=0; i<n; i++)
    {
        dfdb3[i] = b[1] * t[i] * cos(b[2] * t[i] + b[3]);
    }

    return dfdb3;
}


/*********************************************
                  get_dfdb4
*********************************************/
float* get_dfdb4(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[3]:

    dfdb4 = b[1] * cos(b[2] * t + b[3]
    ***/
    int i;
    float* dfdb4 = new float[n];

    for (i=0; i<n; i++)
    {
        dfdb4[i] = b[1] * cos(b[2] * t[i] + b[3]);
    }

    return dfdb4;
}


/*********************************************
                get_delta_beta
*********************************************/
float** get_delta_beta(float** J, float* r, int nRows)
{
    /***
    Computes the update factor for the parameters beta.

    \Delta \beta = (J^T @ J)^{-1} @ J^T @ r
    ***/
    int i;
    float** jTranspose;
    float** H;
    float** hInv;
    float** temp;
    float** deltaB;
    float** rReshaped;

    jTranspose = transpose(J, nRows, 4);
    // matrix_multiply takes nRowsA and nColsA as the last two args.
    // matrix A is the first matrix passed (jTranspose). Recall that
    // nRows was defined as the number of rows in J, which, in turn,
    // is the number of columns in jTranspose, which is why nRows is
    // passed as a columns argument
    H = matrix_multiply(jTranspose, J, 4, nRows, 4);
    hInv = invert_matrix(H, 4);
    temp = matrix_multiply(hInv, jTranspose, 4, nRows, 4);
    // Need to reshape the residuals array. It's currently flat, but
    // in order to do matrix multiplication it needs to be (m,1), where
    // m is nRows
    rReshaped = make_column_vector(r, nRows);
    deltaB = matrix_multiply(temp, rReshaped, 4, nRows, 1);

    // Clean up
    for (i=0; i<4; i++)
    {
        delete[] jTranspose[i];
        delete[] H[i];
        delete[] hInv[i];
        delete[] temp[i];
    }
    delete[] jTranspose;
    delete[] H;
    delete[] hInv;
    delete[] temp;

    for (i=0; i<nRows; i++)
    {
        delete[] rReshaped[i];
    }
    delete[] rReshaped;

    return deltaB;
}


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


/*********************************************
                 update_beta
*********************************************/
float* update_beta(float* b, float** db, float damping)
{
    /***
    Performs the update: beta = beta + alpha * delta_Beta
    ***/
    int i;
    float* dbReshaped;

    // beta (b) is a row vector (1, 4), but delta beta (db) is a
    // column vector (4, 1). In order to add them they need the same
    // shape. The rest of the code assumes b is a row vector, so here
    // we transform db to a row vector
    dbReshaped = make_row_vector(db, 4);

    for (i=0; i<4; i++)
    {
        b[i] = b[i] + damping * dbReshaped[i];
    }

    delete[] dbReshaped;

    // I know you don't actually need to return b here since it was
    // passed by reference, but doing so is conceptually easier for
    // me
    return b;
}


/*********************************************
              convergence_check
*********************************************/
bool convergence_check(float** db, float* b, float tolerance, int n)
{
    /***
    Checks to see if the variation in each model parameter is within
    the desired tolerance.

    Checks:

    |\Delta \beta / \beta | < \epsilon

    Returns true if the condition is met for every model parameter and
    false otherwise.
    ***/
    int i;
    float* dbReshaped;

    dbReshaped = make_row_vector(db, 4);

    for (i=0; i<n; i++)
    {
        if (fabs(dbReshaped[i] / b[i]) > tolerance)
        {
            return false;
        }
    }

    delete[] dbReshaped;

    return true;
}

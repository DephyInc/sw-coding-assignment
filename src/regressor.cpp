#include <math.h>

#include "regressor.hpp"
#include "matrices.hpp"


void SineRegressor::do_regression(float** xData, float** yData, int* nCols, int nRows)
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
    int curIteration;
    int maxIterations = 1000;
    float tolerance = 0.00001;
    float damping = 0.01;
    float* x;
    float* y;
    float* beta;
    float* r;
    float** deltaB;
    float** J;

    modelParams = new float* [nRows];

    // Loop over each row
    for (i=0; i<nRows; i++)
    {
        // Extract time and x acceleration
        x = new float [nCols[i]];
        y = new float [nCols[i]];
        for (j=0; j<nCols[i]; j++)
        {
            x[j] = xData[i][j];
            y[j] = yData[i][j];
        }
        // Reset the iteration counter
        curIteration = 0;
        // Reset Beta parameters to initial guess
        beta = get_initial_guess();
        // Perform the regression
        while (curIteration < maxIterations)
        {
            r = get_residuals(x, y, beta, nCols[i]);
            J = get_jacobian(x, beta, nCols[i]);
            deltaB = get_delta_beta(J, r, nCols[i]);
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
        save_beta(i, beta, 4);
        // Clean up for the next file
        delete[] x;
        delete[] y;
        delete[] r;
        delete[] beta;
        delete[] deltaB;
        for (j=0; j<nCols[i]; j++)
        {
            delete[] J[j];
        }
        delete[] J;
    }
}

void SineRegressor::save_beta(int idx, float* beta, int n)
{
    int i;
    
    modelParams[idx] = new float [n];

    for (i=0; i<n; i++)
    {
        modelParams[idx][i] = beta[i];
    }
}

float* SineRegressor::get_initial_guess(void)
{
    /***
    Returns an array containing the initial guess values for the
    parameters beta.

    The model is a general sine wave:

    f(t, beta) = beta[0] + beta[1] * sin(beta[2] * t + beta[3])
    ***/
    return new float[4] {0.1, 0.1, 0.1, 0.1};
}

float SineRegressor::get_model(float t, float* b)
{
    /***
    Returns the current model prediction for the given time t.

    f(t, b) = b[0] + b[1] * sin(b[2] * t + b[3])
    ***/
    return b[0] + b[1] * sin(b[2] * t + b[3]);
}

float* SineRegressor::get_residuals(float* t, float* obs, float* beta, int n)
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

float** SineRegressor::get_jacobian(float* t, float* b, int n)
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

float* SineRegressor::get_dfdb1(float* t, float* b, int n)
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

float* SineRegressor::get_dfdb2(float* t, float* b, int n)
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

float* SineRegressor::get_dfdb3(float* t, float* b, int n)
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

float* SineRegressor::get_dfdb4(float* t, float* b, int n)
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

float** SineRegressor::get_delta_beta(float** J, float* r, int nRows)
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
    temp = matrix_multiply(hInv, jTranspose, 4, 4, nRows);
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

float* SineRegressor::update_beta(float* b, float** db, float damping)
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

bool SineRegressor::convergence_check(float** db, float* b, float tolerance, int n)
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

void SineRegressor::find_peaks(float** x, int* nCols, int nRows)
{
    /***
    Locates the index of the data arrays closest to the first peak in
    the model sine wave.

    The sensor data was fit with a generic sine wave:

    y(x) = A + B * sin(C * x + D),

    where A, B, C, and D are the four beta parameters found by
    sinusoidal regression.

    Sine has a max when its argument theta = n * pi / 2. The first peak
    occurs when n = 1, so we have:

    Cx + D = pi / 2 => x = (pi/2 - D) / C.

    We then search our timestamps for the value closest to x. This
    gives us the index corresponding to the first peak.
    ***/
    int i;
    int idx;
    float peakLoc;

    firstPeakIndices = new int [nRows];

    for (i=0; i<nRows; i++)
    {
        peakLoc = (M_PI / 2. - modelParams[i][3]) / modelParams[i][2];
        idx = find_closest_index(x[i], peakLoc, nCols[i]);
        firstPeakIndices[i] = idx;
    }
}

int SineRegressor::find_closest_index(float* a, float x, int n)
{
    /***
    Searches through the array a in order to find the index
    corresponsing to the value in a closest to x.
    ***/
    int idx;
    int i;
    float diff;
    float temp;

    diff = fabs(x - a[0]);
    idx = 0;

    for (i=1; i<n; i++)
    {
        temp = fabs(x - a[i]);
        if (temp < diff)
        {
            diff = temp;
            idx = i;
        }
    }

    return idx;
}

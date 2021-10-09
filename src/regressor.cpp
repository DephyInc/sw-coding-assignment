#include <math.h>

#include "regressor.hpp"
#include "matrices.hpp"


/*********************************************
                  do_regression
*********************************************/
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
    the sum of the squared residuals is minimized (least-squares).

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

    Arguments:
    ----------
        xData : float**
            Ragged array holding all of the data from each file
            corresponding to the dependent variable. The row index is
            the file index and the column index is the data line number
            in that file.
        yData : float**
            Ragged array holding all of the data from each file
            corresponding to the independent variable. The row index is
            the file index and the column index is the data line number
            in that file.
        nCols : int*
            Array of length nRows. Each element gives the number of
            elements in the corresponding row of xData and yData. For
            example, nCols[0] gives the number of elements in xData[0]
            and yData[0].
        nRows : int
            The length of the first dimension of xData and yData, as
            well as the length of nCols.
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

    numFiles = nRows;
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


/*********************************************
                   save_beta
*********************************************/
void SineRegressor::save_beta(int idx, float* beta, int n)
{
    /***
    Saves a copy of the final model parameters determined by the
    regression process.

    Arguments:
    ----------
        idx : int
            Gives the file index that the parameters belong to.
        beta : float*
            Array of model parameters to save.
        n : int
            The number of model parameters (i.e., the length of beta).
    ***/
    int i;
    
    modelParams[idx] = new float [n];

    for (i=0; i<n; i++)
    {
        modelParams[idx][i] = beta[i];
    }
}


/*********************************************
               get_initial_guess
*********************************************/
float* SineRegressor::get_initial_guess(void)
{
    /***
    Returns an array containing the initial guess values for the
    parameters beta. These values are arbitrarily chosen.

    The model is a general sine wave:

    f(t, beta) = beta[0] + beta[1] * sin(beta[2] * t + beta[3])
    ***/
    return new float[4] {0.1, 0.1, 0.1, 0.1};
}


/*********************************************
                   get_model
*********************************************/
float SineRegressor::get_model(float t, float* b)
{
    /***
    Returns the current model prediction for the given time t.

    f(t, b) = b[0] + b[1] * sin(b[2] * t + b[3])

    Arguments:
    ----------
        t : float
            Value of the dependent variable to calculate the model for.
        b : float*
            Array containing the model's parameters.

    Returns:
    --------
        y : float
            The value of the model function: y = A + B * sin(C * t + D)
    ***/
    return b[0] + b[1] * sin(b[2] * t + b[3]);
}


/*********************************************
                 get_residuals
*********************************************/
float* SineRegressor::get_residuals(float* t, float* obs, float* beta, int n)
{
    /***
    Calculates the residuals between the data and current model
    predictions.

    The residual is the quantity minimized by the regression process.

    r_i = obs_i - f(x_i, beta)

    Arguments:
    ----------
        t : float*
            Array containing the values of the dependent variable that
            our model needs to be determined at.
        obs : float*
            Array containing the actual values of the datapoints we are
            trying to fit with out model.
        beta : float*
            Array containing the model parameters.
        n : int
            The number of elements in t and obs.

    Returns:
    --------
        r : float*
            Array of length n whose elements are equal to:
            obs[i] - f(t_i, beta)
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
float** SineRegressor::get_jacobian(float* t, float* b, int n)
{
    /***
    Calculates df(x_i)/db_j.

    This is the matrix of first derivatives. It's needed because in
    order to solve the system of non-linear equations in the Guass-
    Newton method, we approximate the function using a first-order
    Taylor series, which requires the first derivatives.

    Arguments:
    ----------
        t : float*
            Array containing the dependent variable values.
        b : float*
            Array containing the model parameters.
        n : int
            The number of elements in t.

    Returns:
    --------
        J : float**
            The matrix of first order derivatives. Each row corresponds
            to a different data point and each column corresponds to
            a different model parameter. J[i][j] = df[t[i]]/dbeta[j].
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
float* SineRegressor::get_dfdb1(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[0]:

    dfdb1 = 1

    Arguments:
    ----------
        t : float*
            Array containing the dependent variable values.
        b : float*
            Array containing the model parameters.
        n : int
            The number of elements in t.

    Returns:
    --------
        dfdb1 : float*
            An array containing the values of the derivative at each
            data point.
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
float* SineRegressor::get_dfdb2(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[1]:

    dfdb2 = sin(b[2] * t + b[3]

    Arguments:
    ----------
        t : float*
            Array containing the dependent variable values.
        b : float*
            Array containing the model parameters.
        n : int
            The number of elements in t.

    Returns:
    --------
        dfdb2 : float*
            An array containing the values of the derivative at each
            data point.
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
float* SineRegressor::get_dfdb3(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[2]:

    dfdb3 = b[1] * t * cos(b[2] * t + b[3]

    Arguments:
    ----------
        t : float*
            Array containing the dependent variable values.
        b : float*
            Array containing the model parameters.
        n : int
            The number of elements in t.

    Returns:
    --------
        dfdb3 : float*
            An array containing the values of the derivative at each
            data point.
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
float* SineRegressor::get_dfdb4(float* t, float* b, int n)
{
    /***
    Derivative of the model f with respect to beta[3]:

    dfdb4 = b[1] * cos(b[2] * t + b[3]

    Arguments:
    ----------
        t : float*
            Array containing the dependent variable values.
        b : float*
            Array containing the model parameters.
        n : int
            The number of elements in t.

    Returns:
    --------
        dfdb4 : float*
            An array containing the values of the derivative at each
            data point.
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
float** SineRegressor::get_delta_beta(float** J, float* r, int nRows)
{
    /***
    Computes the update factor for the parameters beta.

    \Delta \beta = (J^T @ J)^{-1} @ J^T @ r

    Arguments:
    ----------
        J : float**
            The Jacobian.
        r : float*
            The residuals.
        nRows : int
            The number of data points (number of rows in J).

    Returns:
    --------
        deltaB : float**
            Array containing the updates to each model parameter. Is
            4xnRows. 4 because there are 4 model parameters.
    ***/
    int i;
    float** jTranspose;
    float** H;
    float** hInv;
    float** temp;
    float** deltaB;
    float** rReshaped;

    // J is mx4 (m is # of data points), so jTranspose will be 4xm.
    jTranspose = transpose(J, nRows, 4);
    // matrix_multiply takes nRowsA and nColsA as the last two args.
    // matrix A is the first matrix passed (jTranspose). Recall that
    // nRows was defined as the number of rows in J, which, in turn,
    // is the number of columns in jTranspose, which is why nRows is
    // passed as a columns argument
    // Multiplying a matrix and its transpose results in a square
    // matrix. In this case, H will be 4x4.
    H = matrix_multiply(jTranspose, J, 4, nRows, 4);
    // The inverse of a matrix is also square
    hInv = invert_matrix(H, 4);
    // hInv is 4x4 and jTranspose is 4xm, so temp will be 4xm.
    temp = matrix_multiply(hInv, jTranspose, 4, 4, nRows);
    // Need to reshape the residuals array. It's currently flat, but
    // in order to do matrix multiplication it needs to be (m,1), where
    // m is nRows
    rReshaped = make_column_vector(r, nRows);
    // deltaB will be 4x1.
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
                  update_beta
*********************************************/
float* SineRegressor::update_beta(float* b, float** db, float damping)
{
    /***
    Performs the update: beta = beta + alpha * delta_Beta

    Arguments:
    ----------
        b : float*
            Array of model parameters to be updated.
        db : float**
            4x1 array containing the updates for each model parameter.
        damping : float
            The step parameter that controls how strongly each update
            affects the result.

    Returns:
    --------
        b : float*
            The updated model parameters. Shape 1x4.
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
bool SineRegressor::convergence_check(float** db, float* b, float tolerance, int n)
{
    /***
    Checks to see if the variation in each model parameter is within
    the desired tolerance.

    Checks:

    |\Delta \beta / \beta | < \epsilon

    Returns true if the condition is met for every model parameter and
    false otherwise.

    Arguments:
    ----------
        db : float**
            4x1 array containing the updates to each model parameter.
        b : float*
            1x4 array containing the model parameters.
        tolerance : float
           Threshold that db/d needs to be below in order to say that
           we have converged.
        n : int
            The number of parameters.

    Returns:
    --------
        converged : bool
            True if db/b < tolerance and false otherwise.
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


/*********************************************
                  find_peaks
*********************************************/
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

    Arguments:
    ----------
        x : float**
            Array containing the dependent variable data for each file.
            The row index is the file index and the column index is the
            data line.

        nCols : int*
            Array containing the number of entries in each file.

        nRows : int
            The number of files.
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


/*********************************************
              find_closest_index
*********************************************/
int SineRegressor::find_closest_index(float* a, float x, int n)
{
    /***
    Searches through the array a in order to find the index
    corresponsing to the value in a closest to x.

    Arguments:
    ----------
        a : float*
            Array to be searched through.
        x : float
            Value we want to be closest to.
        n : int
            The number of elements in a.

    Returns:
    --------
        idx : int
            The index of a corresponding to the value closest to x.
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


/*********************************************
                  destructor
*********************************************/
SineRegressor::~SineRegressor()
{
    int i;

    for (i=0; i<numFiles; i++)
    {
        delete[] modelParams[i];
    }
    delete[] modelParams;
    delete[] firstPeakIndices;
}

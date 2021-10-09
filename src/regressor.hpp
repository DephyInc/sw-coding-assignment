#ifndef _REGRESSOR_
#define _REGRESSOR_


class SineRegressor
{
    public:
        int numFiles;
        float** modelParams;
        int* firstPeakIndices;

        void do_regression(float**, float**, int*, int);
        void find_peaks(float**, int*, int);
        ~SineRegressor();

    private:
        void save_beta(int, float*, int);
        float* get_initial_guess(void);
        float get_model(float, float*);
        float* get_residuals(float*, float*, float*, int);
        float** get_jacobian(float*, float*, int);
        float* get_dfdb1(float*, float*, int);
        float* get_dfdb2(float*, float*, int);
        float* get_dfdb3(float*, float*, int);
        float* get_dfdb4(float*, float*, int);
        float** get_delta_beta(float**, float*, int);
        float* update_beta(float*, float**, float);
        bool convergence_check(float**, float*, float, int);
        int find_closest_index(float*, float, int);
};
#endif

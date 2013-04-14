#include "polifitgsl.hpp"

// C version
bool polynomialfit(int obs, int degree, double *dx, double *dy, double *store) /* n, p */
{
    gsl_multifit_linear_workspace *ws;
    gsl_matrix *cov, *X;
    gsl_vector *y, *c;
    double chisq;

    int i, j;

    X = gsl_matrix_alloc(obs, degree);
    y = gsl_vector_alloc(obs);
    c = gsl_vector_alloc(degree);
    cov = gsl_matrix_alloc(degree, degree);

    for(i=0; i < obs; i++)
    {
        gsl_matrix_set(X, i, 0, 1.0);
        for(j=0; j < degree; j++)
        {
            gsl_matrix_set(X, i, j, pow(dx[i], j));
        }
        gsl_vector_set(y, i, dy[i]);
    }

    ws = gsl_multifit_linear_alloc(obs, degree);
    gsl_multifit_linear(X, y, c, cov, &chisq, ws);

    /* store result ... */
    for(i=0; i < degree; i++)
    {
        store[i] = gsl_vector_get(c, i);
    }

    gsl_multifit_linear_free(ws);
    gsl_matrix_free(X);
    gsl_matrix_free(cov);
    gsl_vector_free(y);
    gsl_vector_free(c);
    return true; /* we do not "analyse" the result (cov matrix mainly)
          to know if the fit is "good" */
}

// C++ - Qt version
QVector<double> polynomialfit(QVector<QPointF> const& points, int degree)
{
    gsl_multifit_linear_workspace *ws;
    gsl_matrix *cov, *X;
    gsl_vector *y, *c;
    double chisq;

    X = gsl_matrix_alloc(points.count(), degree);
    y = gsl_vector_alloc(points.count());
    c = gsl_vector_alloc(degree);
    cov = gsl_matrix_alloc(degree, degree);

    for(int i(0); i < points.count(); ++i)
    {
        gsl_matrix_set(X, i, 0, 1.0);

        for(int j(0); j < degree; ++j)
            gsl_matrix_set(X, i, j, qPow(points.at(i).x(), j));

        gsl_vector_set(y, i, points.at(i).y());
    }

    ws = gsl_multifit_linear_alloc(points.count(), degree);
    gsl_multifit_linear(X, y, c, cov, &chisq, ws);

    // store result
    QVector<double> coeff;
    for(int i(0); i < degree; ++i)
        coeff.append(gsl_vector_get(c, i));

    // free memory
    gsl_multifit_linear_free(ws);
    gsl_matrix_free(X);
    gsl_matrix_free(cov);
    gsl_vector_free(y);
    gsl_vector_free(c);

    return coeff;
}

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cap.h"


/* Function to fit a list of lists from Python into a continuous array in C */
void fit_list_of_lists_to_array(PyObject* input_list, double* array, Py_ssize_t list_size, Py_ssize_t sublist_size) {
    for (Py_ssize_t i = 0; i < list_size; ++i) {
        PyObject* sublist = PyList_GetItem(input_list, i);

        for (Py_ssize_t j = 0; j < sublist_size; ++j) {
            PyObject* item = PyList_GetItem(sublist, j);
            array[i * sublist_size + j] = PyFloat_AsDouble(item);
        }
    }
}

/* Function to fit a C array into a Python list of lists */
static PyObject* fit_array_to_list_of_lists(double* array, Py_ssize_t rows, Py_ssize_t cols) {
    /* Create a new Python list object */
    PyObject* list_of_lists = PyList_New(rows);

    /* Check if creating the list of lists was successful */
    if (list_of_lists == NULL){
        return NULL;
    }
    /* Fit the array values into the Python list of lists */
    for (Py_ssize_t i = 0; i < rows; ++i) {
        /* Create a new sublist for each row */
        PyObject* sublist = PyList_New(cols);
        if (sublist == NULL) {
            return NULL;
        }

        /* Fit the array values into the sublist */
        for (Py_ssize_t j = 0; j < cols; ++j) {
            PyObject* value = PyFloat_FromDouble(array[i * cols + j]);
            if (value == NULL) {
                return NULL;
            }
            PyList_SET_ITEM(sublist, j, value);
        }
        /* Set the sublist into the list of lists */
        PyList_SET_ITEM(list_of_lists, i, sublist);
    }

    return list_of_lists;
}


static PyObject* fit(PyObject *self, PyObject *args)
{
    printf("fit\n");
    PyObject* vectorsListPy = NULL;
    PyObject* centroidsPy = NULL;
    double* centroids_c;
    double* vectorsList_c;
    int max_iter_c;
    int k_c;
    double epsilon_c;
    int num_vectors_c;
    int vector_length_c;


    /* Parse the arguments from Python*/
    if (!PyArg_ParseTuple(args, "OOiiiid", &vectorsListPy, &centroidsPy, &k_c, &max_iter_c, &num_vectors_c, &vector_length_c, &epsilon_c)){
        return NULL;
    }
    vectorsList_c = (double*)malloc(num_vectors_c * vector_length_c * sizeof(double));
    centroids_c = (double*)malloc(k_c * vector_length_c * sizeof(double));
    /*fit the python list to c array*/
    fit_list_of_lists_to_array(vectorsListPy, vectorsList_c, num_vectors_c, vector_length_c);
    fit_list_of_lists_to_array(centroidsPy, centroids_c, k_c, vector_length_c);

    if (vectorsList_c == NULL || centroids_c == NULL){
        return NULL;
    }  
   
    double* result_centroids_c  = kmeans_c(vectorsList_c, centroids_c, k_c, max_iter_c, num_vectors_c, vector_length_c, epsilon_c);

    
    if(result_centroids_c == NULL){
        return NULL;
    }
    Py_ssize_t size = k_c;
    /*fit the c array to python list*/
    PyObject* resultCentroidsPy = fit_array_to_list_of_lists(result_centroids_c, size, vector_length_c);
    free(vectorsList_c);
    free(centroids_c);
    return resultCentroidsPy;
}

/* module's function table */
static PyMethodDef kmeansMethods[] = {
    {"fit",                   /* the Python method name that will be used */
      (PyCFunction) fit, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters
accepted for this function */

PyDoc_STR("Perform the k-means clustering algorithm on a set of vectors.\n\n"
"Args:\n"
"    self (PyObject*): The reference to the current Python object.\n"
"    args (PyObject*): The arguments passed to the function, including:\n\n"
"                      vectorsList - The data points to perform the clustering\n"
"                      centroids - Initial centroids to start the clustering\n"
"                      k - The number of clusters to create\n"
"                      max_iter - The maximum number of iterations to perform\n"
"                      num_vectors - The number of vectors in the vectorsList\n"
"                      vector_length - The length of each vector in the vectorsList\n"
"                      epsilon - The minimum distance between two centroids to stop the clustering\n"
"\n"
"Returns:\n"
"    PyObject*: The Python list containing the resulting centroids after the k-means clustering."
)}, /*  The docstring for the function */

    {NULL, NULL, 0, NULL}     
};

/* modules definition */
static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "kmeanssp", 
    NULL, 
    -1,  
    kmeansMethods 
};

PyMODINIT_FUNC PyInit_kmeanssp(void)
{
    printf("PyInit_kmeanssp\n");
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m) {
        return NULL;
    }
    return m;
}

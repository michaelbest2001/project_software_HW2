#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "cap.h"

/* Function to fit a Python list into a C array */
static void fit_list_to_array(PyObject* input_list, double* array, Py_ssize_t list_size) {
    for (Py_ssize_t i = 0; i < list_size; ++i) {
        PyObject* item = PyList_GetItem(input_list, i);
        array[i] = PyFloat_AsDouble(item);
    }
}

/* Function to fit a C array into a Python list */
static PyObject* fit_array_to_list(double* array, Py_ssize_t size) {

    PyObject* list = PyList_New(size);

    /* Check if creating the list was successful */
    if (list == NULL) {
        return NULL;
    }

    /* Fit the array values into the Python list */
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* value = PyFloat_FromDouble(array[i]);
        if (value == NULL) {
            return NULL;
        }
        PyList_SET_ITEM(list, i, value);
    }

    return list;
}

static PyObject* fit(PyObject *self, PyObject *args)
{
    PyObject* vectorsListPy;
    PyObject* centroidsPy;
    double* centroids_c;
    double* vectorsList_c;
    int max_iter_c;
    int k_c;
    double epsilon_c;
    int num_vectors_c;
    int vector_length_c;

    /* Parse the arguments from Python*/
    if (!PyArg_ParseTuple(args, "OOii", &vectorsListPy, &centroisPy, &k_c, &max_iter_c, &num_vectors_c, &vector_length_c, &epsilon_c)){
        return NULL;
    }
    /*fit the python list to c array*/
    fit_list_to_array(vectorsListPy, vectorsList_c, k);
    fit_list_to_array(centroidsPy, centroids_c, k);

    double* result_centroids_c  = kmeans_c(vectorsList_c, centroids_c, k_c, max_iter_c, num_vectors_c, vector_length_c, epsilon_c);
    if(result_centroids_c == NULL){
        return NULL;
    }
    Py_ssize_t size = sizeof(array) / sizeof(array[0]);

    /*fit the c array to python list*/
    PyObject* resultCentroidsPy = fit_array_to_list(result_centroids_c, size);
    return resultCentroidsPy;
}

/* module's function table */
static PyMethodDef kmeansMethods[] = {
    {"fit",                   /* the Python method name that will be used */
      (PyCFunction) fit, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters
accepted for this function */
      PyDoc_STR('''
Perform the k-means clustering algorithm on a set of vectors.

Args:
    self (PyObject*): The reference to the current Python object.
    args (PyObject*): The arguments passed to the function, including:

                        vectorsList - The data points to preform the clustering
                        centroids - initital centroids to start the clustering
                        k - The number of clusters to create
                        max_iter - The maximum number of iterations to perform
                        num_vectors - The number of vectors in the vectorsList
                        vector_length - The length of each vector in the vectorsList
                        epsilon - The minimum distance between two centroids to stop the clustering
                
Returns:
    PyObject*: The Python list containing the resulting centroids after the k-means clustering.

'''
)}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}     
};

/* modules definition */
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "kmeanssp", 
    NULL, 
    -1,  
    kmeansMethods 
};

PyMODINIT_FUNC PyInit_kmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}

#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject* fit(PyObject *self, PyObject *args)
{
    PyObject* vectorsList;
    PyObject* centroids;
    int iter;
    int k;

    /* Parse the arguments from Python*/
    if (!PyArg_ParseTuple(args, "OOii", &input_list1, &input_list2, &iter, &k)) {
        return NULL;
    }

    return;
}

static void fit_list_to_array(PyObject* input_list, double* array, Py_ssize_t list_size) {
    for (Py_ssize_t i = 0; i < list_size; ++i) {
        PyObject* item = PyList_GetItem(input_list, i);
        array[i] = PyFloat_AsDouble(item);
    }
}
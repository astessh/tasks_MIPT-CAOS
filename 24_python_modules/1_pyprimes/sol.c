#include <stdlib.h>
#include <string.h>
#include <Python.h>

int64_t* Factorize(int64_t num, size_t* size) {
  int div = 2;
  *size = 0;
  int64_t* out = NULL;
  while (num != 1) {
    if (num % div == 0) {
      int64_t* temp = (int64_t*) realloc(out, sizeof(int64_t) * (*size + 1));
      if (temp != NULL) {
        out = temp;
        out[*size] = div;
        ++(*size);
      }
      num /= div;
    } else {
      div++;
    }
  }
  return out;
}

static PyObject* arr_to_pyobject(size_t size, int64_t* arr) {
  PyObject* result = PyList_New(size);
  for (size_t i = 0; i < size; ++i) {
    PyObject* elem = PyLong_FromLongLong(arr[i]);
    PyList_SetItem(result, i, elem);
  }
  free(arr);
  return result;
}

static PyObject* factor_out(PyObject* module, PyObject* args) {
  if (PyTuple_Size(args) != 1) {
    PyErr_SetString(PyExc_TypeError, "only one parameter is acceptable");
    return NULL;
  }
  PyObject* py_num = PyTuple_GetItem(args, 0);
  if (!PyLong_Check(py_num)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be an integer type");
    return NULL;
  }
  int64_t num = PyLong_AsLongLong(py_num);
  int64_t* ptr = NULL;
  size_t size = 0;
  Py_BEGIN_ALLOW_THREADS
      ptr = Factorize(num, &size);
  Py_END_ALLOW_THREADS
  if (size == 1) {
    PyObject* str = PyUnicode_FromFormat("Prime!");
    return str;
  }
  return arr_to_pyobject(size, ptr);
}

PyMODINIT_FUNC PyInit_primes() {
  static PyMethodDef methods[] = {
      {"factor_out", factor_out, METH_VARARGS,
       "Factorization"},
      {NULL, NULL, 0, NULL}
  };

  static PyModuleDef moduleDef = {
      PyModuleDef_HEAD_INIT,
      "primes",
      "Number primarization",
      -1,
      methods
  };

  return PyModule_Create(&moduleDef);
}

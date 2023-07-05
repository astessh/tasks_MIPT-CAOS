#include <stdio.h>
#include <Python.h>
#include <frameobject.h>

char* ReadFile(char* path, size_t* size) {
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    exit(EXIT_FAILURE);
  }
  const size_t kPart = 512;
  char* store = (char*) malloc(sizeof(char) * kPart + 1);
  size_t read = 0;
  *size = kPart;
  while ((read = fread(store, sizeof(char), kPart, file)) > 0) {
    if (read == kPart) {
      *size += kPart;
      store = realloc(store, (sizeof(char) * *size) + 1);
    } else {
      *size += -kPart + read;
    }
  }
  fclose(file);
  store[*size] = '\0';
  return store;
}

long Action(PyObject* state) {
  PyObject* state_tuple = PyList_GetItem(state, 0);
  return PyLong_AsLong(PyTuple_GetItem(state_tuple, 0));
}

Py_UCS4* Expr(PyObject* state) {
  PyObject* state_tuple = PyList_GetItem(state, 0);
  return PyUnicode_AsUCS4Copy(PyTuple_GetItem(state_tuple, 1));
}

void Pop(PyObject* list) {
  if (PyList_Size(list) > 0) {
    PyList_SetSlice(list, 0, 1, NULL);
  }
}

static int first_call = 0;
static int entries_count = 0;
static int calls_count = 0;

void CallTrace(PyObject* state) {
  if (first_call == 0) {
    first_call = 1;
  } else if (Action(state) == 3) {
    calls_count++;
    entries_count = 1;
    Pop(state);
  } else {
    entries_count = 1;
  }
}

void LineTrace(PyObject* state) {
  if (entries_count == 0 && calls_count == 0 && Action(state) == 2) {
    Pop(state);
  } else {
    if (entries_count == 1 && calls_count == 1 && Action(state) == 2) {
      Pop(state);
    }
    if (Action(state) == 4) {
      char* command = (char*) malloc(sizeof(char) * 64);
      sprintf(command, "print(repr(%ls))", Expr(state));
      PyRun_String(command,
                   Py_single_input,
                   PyEval_GetGlobals(),
                   PyEval_GetLocals());
      free(command);
      Pop(state);
    }
  }
}

void ReturnTrace(PyObject* state) {
  if (entries_count == 1 && calls_count == 0) {
    entries_count = 0;
  }
  if (entries_count == 1 && calls_count > 0 && Action(state) == 2) {
    entries_count = 0;
    calls_count--;
    Pop(state);
    if (calls_count == 0 && Action(state) == 4) {
      char* command = (char*) malloc(sizeof(char) * 64);
      sprintf(command, "print(repr(%ls))", Expr(state));
      PyRun_String(command,
                   Py_single_input,
                   PyEval_GetGlobals(),
                   PyEval_GetLocals());
      free(command);
      Pop(state);
    }
    if (calls_count == 0 && Action(state) == 2) {
      Pop(state);
    }
  }
}

int Trace(PyObject* obj, PyFrameObject* frame, int what, PyObject* arg) {
  const Py_UNICODE* filename = _PyUnicode_AsUnicode(frame->f_code->co_filename);
  if (filename[0] != '<') {
    return 0;
  }
  PyObject* state = PyList_GetItem(obj, 0);
  if (PyList_Size(state) == 0) {
    return 0;
  }
  if (what == PyTrace_CALL) {
    CallTrace(state);
  } else if (what == PyTrace_LINE) {
    LineTrace(state);
  } else if (what == PyTrace_RETURN) {
    ReturnTrace(state);
  }
  return 0;
}

PyObject* Tuple(size_t val, PyObject* command) {
  PyObject* tuple = PyTuple_New(2);
  PyTuple_SetItem(tuple, 0, PyLong_FromLong(val));
  PyTuple_SetItem(tuple, 1, command);
  return tuple;
}

PyObject* Process(PyObject* lines) {
  PyObject* processed = PyList_New(0);
  for (size_t i = 0; i < PyList_Size(lines); ++i) {
    PyObject* command = PyList_GetItem(lines, i);
    if (PyUnicode_Contains(command, PyUnicode_FromString("next"))) {
      PyList_Append(processed, Tuple(2, NULL));
    } else if (PyUnicode_Contains(command, PyUnicode_FromString("step"))) {
      PyList_Append(processed, Tuple(3, NULL));
    } else if (PyUnicode_Contains(command, PyUnicode_FromString("print"))) {
      PyObject
          * data = PyUnicode_Split(command, PyUnicode_FromString("print"), -1);
      PyObject* expr = PyList_GetItem(data, 1);
      PyList_Append(processed, Tuple(4, expr));
    } else if (PyUnicode_Contains(command, PyUnicode_FromString("continue"))) {
      break;
    }
  }
  return processed;
}

void Debug(char* code, PyObject* list) {
  PyObject* deb_obj = PyList_New(3);
  PyObject* state = PyDict_New();
  PyList_SetItem(deb_obj, 0, list);
  PyList_SetItem(deb_obj, 1, PyList_New(0));
  PyDict_SetItemString(state, "calls_count", PyLong_FromLong(0));
  PyDict_SetItemString(state, "entries_count", PyLong_FromLong(0));
  PyList_SetItem(deb_obj, 2, state);
  PyEval_SetTrace(Trace, deb_obj);
  PyRun_SimpleString(code);
}

int main(int argc, char* argv[]) {
  Py_Initialize();
  if (argc < 2) {
    exit(EXIT_FAILURE);
  }
  char* code_file = argv[2];
  char* deb_file = argv[1];
  wchar_t** _argv = PyMem_Malloc(sizeof(wchar_t*) * (argc - 3));
  for (int i = 3; i < argc; i++) {
    wchar_t* arg = Py_DecodeLocale(argv[i], NULL);
    _argv[i - 3] = arg;
  }
  PySys_SetArgv(argc - 3, _argv);
  size_t code_size = 0;
  size_t deb_size = 0;
  char* code_raw = ReadFile(code_file, &code_size);
  char* deb_raw = ReadFile(deb_file, &deb_size);
  PyObject* deb = PyUnicode_FromStringAndSize(deb_raw, deb_size);
  PyObject* list = Process(PyUnicode_Splitlines(deb, 0));
  Debug(code_raw, list);
  free(code_raw);
  free(deb_raw);
  PyMem_Free(_argv);
  Py_Finalize();
  return 0;
}


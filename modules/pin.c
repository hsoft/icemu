#define Py_LIMITED_API
#include <Python.h>
#include <stdbool.h>

#define RETBOOL(x) if (x) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }

typedef struct {
    PyObject_HEAD
    bool output;
    bool high;
} Pin;

static PyObject *Pin_Type;

// Inner functions

static bool
pin_set(Pin *pin, bool high)
{
    if (high == pin->high) {
        return false;
    }
    pin->high = high;
    return true;
}

// Interface functions

static int
Pin_init(Pin *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"output", "high", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "pp", kwlist,
            &self->output,
            &self->high
            )) {
        return -1;
    }

    return 0;
}
static PyObject *
Pin_ishigh(PyObject *self)
{
    RETBOOL(((Pin *)self)->high);
}

static PyObject *
Pin_isoutput(PyObject *self)
{
    RETBOOL(((Pin *)self)->output);
}

static PyObject *
Pin_set(PyObject *self, PyObject *arg)
{
    RETBOOL(pin_set((Pin *)self, PyObject_IsTrue(arg)));
}

static PyObject *
Pin_setoutput(PyObject *self)
{
    ((Pin *)self)->output = true;
    return Py_None;
}

static PyObject *
Pin_setinput(PyObject *self)
{
    ((Pin *)self)->output = false;
    return Py_None;
}

// Python boilerplate
static PyMethodDef Pin_methods[] = {
    {"ishigh", (PyCFunction)Pin_ishigh, METH_NOARGS, ""},
    {"isoutput", (PyCFunction)Pin_isoutput, METH_NOARGS, ""},
    {"set", (PyCFunction)Pin_set, METH_O, ""},
    {"setoutput", (PyCFunction)Pin_setoutput, METH_NOARGS, ""},
    {"setinput", (PyCFunction)Pin_setinput, METH_NOARGS, ""},
    {0, 0, 0, 0},
};

static PyType_Slot Pin_Slots[] = {
    {Py_tp_init, Pin_init},
    {Py_tp_methods, Pin_methods},
    {0, 0},
};

static PyType_Spec Pin_Type_Spec = {
    "_pin.Pin",
    sizeof(Pin),
    0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    Pin_Slots,
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

static struct PyModuleDef PinDef = {
    PyModuleDef_HEAD_INIT,
    "_pin",
    NULL,
    -1,
    module_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyObject *
PyInit__pin(void)
{
    PyObject *m;

    Pin_Type = PyType_FromSpec(&Pin_Type_Spec);

    m = PyModule_Create(&PinDef);
    if (m == NULL) {
        return NULL;
    }

    PyModule_AddObject(m, "Pin", Pin_Type);
    return m;
}

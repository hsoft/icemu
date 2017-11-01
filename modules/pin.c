#define Py_LIMITED_API
#include <Python.h>
#include <stdbool.h>

#define MAX_WIRE_COUNT 20
#define RETBOOL(x) if (x) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }

typedef struct Pin {
    PyObject_HEAD
    bool output;
    bool high;
    PyObject *chip;
    struct Pin *wires[MAX_WIRE_COUNT];
} Pin;

static PyObject *Pin_Type;

#define Pin_Check(v) (Py_TYPE(v) == (PyTypeObject *)Pin_Type)

// Inner functions

static void
pin_set(Pin *pin, bool high)
{
    if (high == pin->high) {
        return;
    }
    pin->high = high;

    if (!pin->output && (pin->chip != Py_None)) {
        PyObject_CallMethod(pin->chip, "update", NULL);
    }
    if (pin->output) {
        for (int i=0; i<MAX_WIRE_COUNT; i++) {
            if (pin->wires[i] == NULL) {
                break;
            }
            if (!pin->wires[i]->output && (pin->wires[i]->chip != Py_None)) {
                PyObject_CallMethod(pin->wires[i]->chip, "update", NULL);
            }
        }
    }
}

static void
pin_wire_to(Pin *pin, Pin *other)
{
    for (int i=0; i<MAX_WIRE_COUNT; i++) {
        if (pin->wires[i] == other) {
            break;
        }
        if (pin->wires[i] == NULL) {
            pin->wires[i] = other;
            break;
        }
    }
}

// Interface functions

static int
Pin_init(Pin *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"output", "high", "chip", NULL};


    for (int i=0; i<MAX_WIRE_COUNT; i++) {
        self->wires[i] = NULL;
    }
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "ppO", kwlist,
            &self->output,
            &self->high,
            &self->chip
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
    pin_set((Pin *)self, PyObject_IsTrue(arg));
    Py_RETURN_NONE;
}

static PyObject *
Pin_setraw(PyObject *self, PyObject *arg)
{
    ((Pin *)self)->high = PyObject_IsTrue(arg);
    Py_RETURN_NONE;
}

static PyObject *
Pin_setoutput(PyObject *self)
{
    ((Pin *)self)->output = true;
    Py_RETURN_NONE;
}

static PyObject *
Pin_setinput(PyObject *self)
{
    ((Pin *)self)->output = false;
    Py_RETURN_NONE;
}

static PyObject *
Pin_wire_to(PyObject *self, PyObject *arg)
{
    if (Pin_Check(arg)) {
        pin_wire_to((Pin *)self, (Pin *)arg);
    }
    Py_RETURN_NONE;
}

// Python boilerplate
static PyMethodDef Pin_methods[] = {
    {"ishigh", (PyCFunction)Pin_ishigh, METH_NOARGS, ""},
    {"isoutput", (PyCFunction)Pin_isoutput, METH_NOARGS, ""},
    {"set", (PyCFunction)Pin_set, METH_O, ""},
    {"setraw", (PyCFunction)Pin_setraw, METH_O, ""},
    {"setoutput", (PyCFunction)Pin_setoutput, METH_NOARGS, ""},
    {"setinput", (PyCFunction)Pin_setinput, METH_NOARGS, ""},
    {"wire_to", (PyCFunction)Pin_wire_to, METH_O, ""},
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

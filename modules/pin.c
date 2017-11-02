#define Py_LIMITED_API
#include <Python.h>
#include <stdbool.h>

#define MAX_WIRE_COUNT 50
#define RETBOOL(x) if (x) { Py_RETURN_TRUE; } else { Py_RETURN_FALSE; }

typedef struct Pin {
    PyObject_HEAD
    bool output;
    bool high;
    bool low_means_enabled;
    unsigned int oscillating_freq;
    int next_oscillation_in;
    bool oscillating_rapidly;
    PyObject *chip;
    struct Pin *wires[MAX_WIRE_COUNT];
} Pin;

static PyObject *Pin_Type;

#define Pin_Check(v) (Py_TYPE(v) == (PyTypeObject *)Pin_Type)

// Inner functions

static Pin *
pin_wired_oscillator(Pin *pin)
{
    if (pin->output) {
        if (pin->oscillating_freq) {
            return pin;
        } else {
            return NULL;
        }
    } else {
        int i;
        for (i=0; i<MAX_WIRE_COUNT; i++) {
            if (pin->wires[i] == NULL) {
                break;
            }
            if (pin->wires[i]->output && pin->wires[i]->oscillating_freq) {
                return pin->wires[i];
            }
        }
        return NULL;
    }
}

static void
pin_update_chips(Pin *pin)
{
    if (!pin->output && (pin->chip != Py_None)) {
        PyObject_CallMethod(pin->chip, "update", NULL);
    }
    if (pin->output) {
        int i;
        for (i=0; i<MAX_WIRE_COUNT; i++) {
            if (pin->wires[i] == NULL) {
                break;
            }
            if (!pin->wires[i]->output && (pin->wires[i]->chip != Py_None)) {
                PyObject_CallMethod(pin->wires[i]->chip, "update", NULL);
            }
        }
    }
}

static bool
pin_is_oscillating_rapidly(Pin *pin)
{
    Pin *oscillator = pin_wired_oscillator(pin);
    if (oscillator) {
        return (oscillator->oscillating_freq && oscillator->oscillating_rapidly);
    } else {
        return false;
    }
}

static bool
pin_ishigh(Pin *pin)
{
    if (pin_is_oscillating_rapidly(pin)) {
        return true;
    }
    if (pin->output) {
        return pin->high;
    } else {
        bool hasoutput = false;
        int i;
        for (i=0; i<MAX_WIRE_COUNT; i++) {
            if (pin->wires[i] == NULL) {
                break;
            }
            if (pin->wires[i]->output) {
                hasoutput = true;
                if (pin->wires[i]->high) {
                    return true;
                }
            }
        }
        if (hasoutput) {
            return false; // we have output, but none is high, thus, we're low.
        } else {
            // no output, return inner value
            return pin->high;
        }
    }
}

static bool
pin_is_oscillating_slowly(Pin *pin)
{
    Pin *oscillator = pin_wired_oscillator(pin);
    if (oscillator) {
        return (oscillator->oscillating_freq && !oscillator->oscillating_rapidly);
    } else {
        return false;
    }
}

static unsigned int
pin_oscillating_freq(Pin *pin)
{
    Pin *oscillator = pin_wired_oscillator(pin);
    if (oscillator != NULL) {
        return oscillator-> oscillating_freq;
    } else {
        return 0;
    }
}

static void
pin_set(Pin *pin, bool high)
{
    if (pin->oscillating_rapidly) {
        pin->oscillating_freq = 0;
    }

    if (high == pin->high) {
        return;
    }
    pin->high = high;

    pin_update_chips(pin);
}

static void
pin_set_oscillating_freq(Pin *pin, unsigned int freq)
{
    pin->oscillating_freq = freq;
    pin->next_oscillation_in = 0;
    pin->oscillating_rapidly = true;
    pin_update_chips(pin);
}

static void
pin_tick(Pin *pin, unsigned int usecs)
{
    if (!(pin->output && pin->oscillating_freq)) {
        return;
    }
    unsigned int freq = pin->oscillating_freq;
    unsigned int every_usecs = (1000 * 1000) / freq;
    pin->oscillating_rapidly = every_usecs * 2 <= usecs;
    if (pin->oscillating_rapidly) {
        pin->next_oscillation_in = 0;
    } else {
        pin->next_oscillation_in -= usecs;
        while (pin->next_oscillation_in < 0) {
            pin_set(pin, !pin->high);
            pin->next_oscillation_in += every_usecs;
        }
    }
}

static void
pin_wire_to(Pin *pin, Pin *other)
{
    int i;
    for (i=0; i<MAX_WIRE_COUNT; i++) {
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
    static char *kwlist[] = {
        "output", "high", "low_means_enabled", "oscillating_freq", "chip", NULL
    };
    int i;


    for (i=0; i<MAX_WIRE_COUNT; i++) {
        self->wires[i] = NULL;
    }
    self->next_oscillation_in = 0;
    self->oscillating_rapidly = true;
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "pppIO", kwlist,
            &self->output,
            &self->high,
            &self->low_means_enabled,
            &self->oscillating_freq,
            &self->chip
            )) {
        return -1;
    }

    return 0;
}

static PyObject *
Pin_disable(PyObject *self)
{
    Pin *pin = (Pin *)self;
    pin_set(pin, pin->low_means_enabled);
    Py_RETURN_NONE;
}

static PyObject *
Pin_enable(PyObject *self)
{
    Pin *pin = (Pin *)self;
    pin_set(pin, !pin->low_means_enabled);
    Py_RETURN_NONE;
}

static PyObject *
Pin_isenabled(PyObject *self)
{
    Pin *pin = (Pin *)self;
    RETBOOL(pin_ishigh(pin) != pin->low_means_enabled);
}

static PyObject *
Pin_ishigh(PyObject *self)
{
    RETBOOL(pin_ishigh((Pin *)self));
}

static PyObject *
Pin_isoutput(PyObject *self)
{
    RETBOOL(((Pin *)self)->output);
}

static PyObject *
Pin_is_oscillating(PyObject *self)
{
    Pin *oscillator = pin_wired_oscillator((Pin *)self);
    RETBOOL(oscillator != NULL);
}

static PyObject *
Pin_is_oscillating_rapidly(PyObject *self)
{
    RETBOOL(pin_is_oscillating_rapidly((Pin *)self));
}

static PyObject *
Pin_is_oscillating_slowly(PyObject *self)
{
    RETBOOL(pin_is_oscillating_slowly((Pin *)self));
}

static PyObject *
Pin_next_oscillation_in(PyObject *self)
{
    Pin *pin;

    pin = (Pin *)self;
    if (pin->output) {
        if (pin->oscillating_freq) {
            if (pin->oscillating_rapidly) {
                Py_RETURN_NONE;
            } else {
                return PyLong_FromLong(pin->next_oscillation_in);
            }
        } else {
            Py_RETURN_NONE;
        }
    } else {
        Pin *oscillator = pin_wired_oscillator(pin);
        return Pin_next_oscillation_in((PyObject *)oscillator);
    }
}

static PyObject *
Pin_oscillating_freq(PyObject *self)
{
    return PyLong_FromUnsignedLong(pin_oscillating_freq((Pin *)self));
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
Pin_setlow(PyObject *self)
{
    pin_set((Pin *)self, false);
    Py_RETURN_NONE;
}

static PyObject *
Pin_sethigh(PyObject *self)
{
    pin_set((Pin *)self, true);
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
Pin_set_oscillating_freq(PyObject *self, PyObject *arg)
{
    unsigned long freq = PyLong_AsUnsignedLong(arg);
    if (PyErr_Occurred()) {
        return NULL;
    }
    pin_set_oscillating_freq((Pin *)self, freq);
    Py_RETURN_NONE;
}

static PyObject *
Pin_tick(PyObject *self, PyObject *arg)
{
    long freq = PyLong_AsLong(arg);
    if (PyErr_Occurred()) {
        return NULL;
    }
    pin_tick((Pin *)self, PyLong_AsLong(arg));
    Py_RETURN_NONE;
}

static PyObject *
Pin_toggle(PyObject *self)
{
    Pin *pin = (Pin *)self;
    pin_set(pin, !pin_ishigh(pin));
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
    {"disable", (PyCFunction)Pin_disable, METH_NOARGS, ""},
    {"enable", (PyCFunction)Pin_enable, METH_NOARGS, ""},
    {"ishigh", (PyCFunction)Pin_ishigh, METH_NOARGS, ""},
    {"isoutput", (PyCFunction)Pin_isoutput, METH_NOARGS, ""},
    {"isenabled", (PyCFunction)Pin_isenabled, METH_NOARGS, ""},
    {"is_oscillating", (PyCFunction)Pin_is_oscillating, METH_NOARGS, ""},
    {"is_oscillating_rapidly", (PyCFunction)Pin_is_oscillating_rapidly, METH_NOARGS, ""},
    {"is_oscillating_slowly", (PyCFunction)Pin_is_oscillating_slowly, METH_NOARGS, ""},
    {"next_oscillation_in", (PyCFunction)Pin_next_oscillation_in, METH_NOARGS, ""},
    {"oscillating_freq", (PyCFunction)Pin_oscillating_freq, METH_NOARGS, ""},
    {"set", (PyCFunction)Pin_set, METH_O, ""},
    {"sethigh", (PyCFunction)Pin_sethigh, METH_NOARGS, ""},
    {"setlow", (PyCFunction)Pin_setlow, METH_NOARGS, ""},
    {"setinput", (PyCFunction)Pin_setinput, METH_NOARGS, ""},
    {"setoutput", (PyCFunction)Pin_setoutput, METH_NOARGS, ""},
    {"setraw", (PyCFunction)Pin_setraw, METH_O, ""},
    {"set_oscillating_freq", (PyCFunction)Pin_set_oscillating_freq, METH_O, ""},
    {"tick", (PyCFunction)Pin_tick, METH_O, ""},
    {"toggle", (PyCFunction)Pin_toggle, METH_NOARGS, ""},
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

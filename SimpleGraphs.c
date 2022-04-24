#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

typedef struct ListElemStruct {
    unsigned short edge;
    struct ListElemStruct* nextElem;
    struct ListElemStruct* prevElem;
} ListElem;

typedef struct {
    PyObject_HEAD
	ListElem* matrix;
    unsigned short vertices;
} IncidenceMatrixObject;

static void
IncidenceMatrix_dealloc(IncidenceMatrixObject* self)
{
    while (self->matrix) {
        ListElem* elemToRemove = self->matrix;
        self->matrix = elemToRemove->nextElem;
        free(elemToRemove);
    }

    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject*
IncidenceMatrix_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    IncidenceMatrixObject* self;
    self = (IncidenceMatrixObject*)type->tp_alloc(type, 0);
    return (PyObject*)self;
}

static int
IncidenceMatrix_init(IncidenceMatrixObject* self, PyObject* args, PyObject* kwds)
{
    char* text;
    if (!PyArg_ParseTuple(args, "s", &text))
        return -1;
    int length = (int)text[0] - 63;

    unsigned short number = 0x8000;
    self->vertices = 0;
    for (int j = 0; j < length; j++) {
        self->vertices = (self->vertices | number);
        number = (number >> 1);
    }
    
    self->matrix = NULL;

    int k = 0, i = 1, c;
    for (int v = 1; v < length; v++) {
        for (int u = 0; u < v; u++) {
            if (k == 0) {
                c = (int)text[i] - 63;
                i++;
                k = 6;
            }
            k--;
            unsigned short number = 0x8000;
            if ((c & (1 << k)) != 0) {
                ListElem* newEdge = calloc(1, sizeof(*newEdge));
                if (!newEdge) return -1;
                unsigned short vertex_1 = 0x8000 >> v;
                unsigned short vertex_2 = 0x8000 >> u;
                newEdge->edge = newEdge->edge | vertex_1 | vertex_2;
				newEdge->nextElem = self->matrix;
                newEdge->prevElem = NULL;
                if (newEdge->nextElem) {
                    newEdge->nextElem->prevElem = newEdge;
                }
                self->matrix = newEdge;
            }
        }
    }
    return 0;
}

static PyMemberDef IncidenceMatrix_members[] = {
    {"matrix", T_UINT, offsetof(IncidenceMatrixObject, matrix), 0, "incidence matrix"},
    {"vertices", T_USHORT, offsetof(IncidenceMatrixObject, vertices), 0, "vertices"},
    {NULL}  /* Sentinel */
};

static PyObject* number_of_vertices(IncidenceMatrixObject* self)
{
    int counter = 0;
    unsigned short number = 0x8000;
    for (int i = 0; i < 16; i++) {
        if ((self->vertices & number) == number) {
            counter++;
        }
        number = number >> 1;
    }
    return Py_BuildValue("i", counter);
}

static PyObject* vertices(IncidenceMatrixObject* self)
{
    PyObject* set = PySet_New(NULL);

    unsigned short number = 0x8000;
    for (int i = 0; i < 16; i++) {
        if ((self->vertices & number) == number) {
            PyObject* python_int = Py_BuildValue("i", i);
            PySet_Add(set, python_int);
            Py_DECREF(python_int);
        }
        number = number >> 1;
    }

    return set;
}

static PyObject* number_of_edges(IncidenceMatrixObject* self)
{
    int counter = 0;

    ListElem* edgePtr = self->matrix;
    while (edgePtr) {
        counter++;
        edgePtr = edgePtr->nextElem;
    }

    return Py_BuildValue("i", counter);
}

static PyObject* edges(IncidenceMatrixObject* self)
{
    PyObject* set = PySet_New(NULL);

    ListElem* edgePtr = self->matrix;
    while (edgePtr) {
        unsigned short edge = edgePtr->edge;
        int read_counter = 0;
        int edges[2];
        for (int i = 0; i < 16; i++) {
            if ((edge & (0x8000 >> i)) != 0) {
                edges[read_counter] = i;
                read_counter++;
            }
            if (read_counter >= 2) break;
        }
        PyObject* python_int = Py_BuildValue("(ii)", edges[0], edges[1]);
        PySet_Add(set, python_int);
        Py_DECREF(python_int);
        edgePtr = edgePtr->nextElem;
    }

    return set;
}

static PyObject* is_edge(IncidenceMatrixObject* self, PyObject* vertices)
{
    int u, v;
    if (!PyArg_ParseTuple(vertices, "ii", &u, &v))
        return NULL;

    ListElem* edgePtr = self->matrix;
    while (edgePtr) {
        unsigned short edge = edgePtr->edge;
        if ((edge & (0x8000 >> u)) != 0 && (edge & (0x8000 >> v)) != 0) {
            Py_RETURN_TRUE;
        }
        edgePtr = edgePtr->nextElem;
    }
    Py_RETURN_FALSE;
}

static PyObject* delete_vertex(IncidenceMatrixObject* self, PyObject* vertex)
{
    int v;
    if (!PyArg_ParseTuple(vertex, "i", &v))
        Py_RETURN_NONE;

    unsigned short number = 0x8000;
    number = number >> v;
    self->vertices = self->vertices & (~number);

    ListElem* edgePtr = self->matrix;
    while (edgePtr) {
        if ((edgePtr->edge & number) != 0) {
            if (edgePtr->nextElem) {
                edgePtr->nextElem->prevElem = edgePtr->prevElem;
            }
            if (edgePtr->prevElem) {
                edgePtr->prevElem->nextElem = edgePtr->nextElem;
            }
            else {
                self->matrix = edgePtr->nextElem;
            }
            ListElem* edgePtrRm = edgePtr;
            edgePtr = edgePtr->nextElem;
            free(edgePtrRm);
            continue;
        }
        edgePtr = edgePtr->nextElem;
    }

    Py_RETURN_NONE;
}

static PyObject* add_vertex(IncidenceMatrixObject* self, PyObject* vertex)
{
    int v;
    if (!PyArg_ParseTuple(vertex, "i", &v))
        Py_RETURN_NONE;

    unsigned short number = 0x8000;
    number = (number >> v);
    self->vertices = (self->vertices | number);

    Py_RETURN_NONE;
}

static PyObject* delete_edge(IncidenceMatrixObject* self, PyObject* vertices)
{
    int u, v;
    if (!PyArg_ParseTuple(vertices, "ii", &u, &v))
        Py_RETURN_NONE;

    ListElem* edgePtr = self->matrix;
    while (edgePtr) {
        if ((edgePtr->edge & (0x8000 >> u)) != 0 && (edgePtr->edge & (0x8000 >> v)) != 0) {
            if (edgePtr->nextElem) {
                edgePtr->nextElem->prevElem = edgePtr->prevElem;
            }
            if (edgePtr->prevElem) {
                edgePtr->prevElem->nextElem = edgePtr->nextElem;
            }
            else {
                self->matrix = edgePtr->nextElem;
            }
            free(edgePtr);
            Py_RETURN_NONE;
        }
        edgePtr = edgePtr->nextElem;
    }

    Py_RETURN_NONE;
}

static PyObject* add_edge(IncidenceMatrixObject* self, PyObject* vertices)
{
    int u, v;
    if (!PyArg_ParseTuple(vertices, "ii", &u, &v))
        Py_RETURN_NONE;

    //potentially to remove - first, check if vertices exist
    if ((self->vertices & (0x8000 >> u)) == 0 || (self->vertices & (0x8000 >> v)) == 0) {
        Py_RETURN_NONE;
    }

    ListElem* edgePtr = self->matrix;
    while (edgePtr) {
        unsigned short edge = edgePtr->edge;
        if ((edge & (0x8000 >> u)) != 0 && (edge & (0x8000 >> v)) != 0) {
            Py_RETURN_NONE;
        }
        edgePtr = edgePtr->nextElem;
    }

    ListElem* newEdge = calloc(1, sizeof(*newEdge));
    if (!newEdge) Py_RETURN_FALSE;
    unsigned short vertex_1 = 0x8000 >> v;
    unsigned short vertex_2 = 0x8000 >> u;
    newEdge->edge = newEdge->edge | vertex_1 | vertex_2;
    newEdge->nextElem = self->matrix;
    newEdge->prevElem = NULL;
    if (newEdge->nextElem) {
        newEdge->nextElem->prevElem = newEdge;
    }
    self->matrix = newEdge;

    Py_RETURN_NONE;
}

static PyMethodDef IncidenceMatrix_methods[] = {
	{"number_of_vertices", (PyCFunction)number_of_vertices, METH_NOARGS, "Returns the number of vertices."},
    {"vertices", (PyCFunction)vertices, METH_NOARGS, "Returns the vertices."},
    {"number_of_edges", (PyCFunction)number_of_edges, METH_NOARGS, "Returns the number of edges."},
    {"edges", (PyCFunction)edges, METH_NOARGS, "Returns the edges."},
    {"is_edge", (PyCFunction)is_edge, METH_VARARGS, "Returns the vertices are neighbours."},
    // {"vertex_degree", (PyCFunction)vertex_degree, METH_VARARGS, "Returns the degree of vertex"},
    // {"vertex_neighbors", (PyCFunction)vertex_neighbors, METH_VARARGS, "Returns the neighbors of vertex"},
    {"delete_vertex", (PyCFunction)delete_vertex, METH_VARARGS, "Delete vertex and incidental edges"},
    {"add_vertex", (PyCFunction)add_vertex, METH_VARARGS, "Add vertex"},
    {"delete_edge", (PyCFunction)delete_edge, METH_VARARGS, "Delete edge"},
    {"add_edge", (PyCFunction)add_edge, METH_VARARGS, "Add edge"},
    // {"is_bipartite", (PyCFunction)is_bipartite, METH_NOARGS, "If is bipartite"},
    {NULL}
};

static PyTypeObject IncidenceMatrixType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "incidencematrix.incidenceMatrix",
    .tp_basicsize = sizeof(IncidenceMatrixObject),
    .tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_new = IncidenceMatrix_new,
	.tp_init = (initproc)IncidenceMatrix_init,
    .tp_dealloc = (destructor)IncidenceMatrix_dealloc,
	.tp_members = IncidenceMatrix_members,
	.tp_methods = IncidenceMatrix_methods,
};

static PyModuleDef incidencematrixmodule = {
    PyModuleDef_HEAD_INIT,
    "incidencematrix",
    "Example module that creates an extension type.",
    -1,
};

PyMODINIT_FUNC
PyInit_simple_graphs(void)
{
    PyObject* m;
    if (PyType_Ready(&IncidenceMatrixType) < 0)
        return NULL;

    m = PyModule_Create(&incidencematrixmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&IncidenceMatrixType);
    if (PyModule_AddObject(m, "IncidenceMatrix", (PyObject*)&IncidenceMatrixType) < 0) {
        Py_DECREF(&IncidenceMatrixType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../application/interactors.h"
#include "../external/tcp/redis.h"
//
// static void print(char str[]) {
//     for (; *str != '\0'; str++) {
//         if (*str == '\n') {
//             putchar('\\');
//             putchar('n');
//         }
//
//         else if (*str == '\r') {
//             putchar('\\');
//             putchar('r');
//         }
//         else {
//             putchar(*str);
//         }
//     }
//
//     putchar('\n');
// }

int redis_server_socket_desc;

static PyObject *CommandSendingError;

static PyObject *UnknownServerResponseError;

static PyObject *
_myredis_connect_to_redis_server(PyObject *self, PyObject *args)
{
    const char *host;
    int port;

    if (!PyArg_ParseTuple(args, "si", &host, &port))
        return NULL;

    int result = connect_(host, port);

    switch (result) {
        case CONNECTION_REFUSED_ERROR:
            PyErr_SetString(PyExc_ConnectionRefusedError, "");
            return NULL;
    }

    return PyLong_FromLong(connect_(host, port));
}

static PyObject *
_myredis_disconnect_from_redis_server(PyObject *self, PyObject *args)
{
    int socket_desc;

    if (!PyArg_ParseTuple(args, "i", &socket_desc))
        return NULL;

    close_connection(socket_desc);

    Py_RETURN_NONE;
}

static PyObject *
_myredis_get_response(PyObject *self, PyObject *args)
{
    int socket_desc;

    if (!PyArg_ParseTuple(args, "i", &socket_desc))
        return NULL;

    redis_server_socket_desc = socket_desc;

    char *out = malloc(10000);
    out[0] = '\0';
    int status = get_response_interactor(out);

    switch (status) {
        case UNKNOWN_SERVER_RESPONSE_ERROR:
            PyErr_SetString(UnknownServerResponseError, "");
            return NULL;
    }

    if (out[0] == '\0') {
        Py_RETURN_NONE;
    }

    PyObject *result = PyBytes_FromString(out);
    free(out);
    return result;
}

static PyObject *
_myredis_send_ping_request(PyObject *self, PyObject *args)
{
    int socket_desc;

    if (!PyArg_ParseTuple(args, "i", &socket_desc))
        return NULL;

    redis_server_socket_desc = socket_desc;
    int status = ping_interactor();

    switch (status) {
        case COMMAND_SENDING_ERROR:
            PyErr_SetString(CommandSendingError, "");
            return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
_myredis_send_echo_request(PyObject *self, PyObject *args)
{
    int socket_desc;
    const char *str;

    if (!PyArg_ParseTuple(args, "is", &socket_desc, &str))
        return NULL;

    redis_server_socket_desc = socket_desc;
    int status = echo_interactor(str);

    switch (status) {
        case COMMAND_SENDING_ERROR:
            PyErr_SetString(CommandSendingError, "");
            return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
_myredis_send_get_request(PyObject *self, PyObject *args)
{
    int socket_desc;
    const char *key;

    if (!PyArg_ParseTuple(args, "is", &socket_desc, &key))
        return NULL;

    redis_server_socket_desc = socket_desc;
    int status = get_interactor(key);

    switch (status) {
        case COMMAND_SENDING_ERROR:
            PyErr_SetString(CommandSendingError, "");
            return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
_myredis_send_set_request(PyObject *self, PyObject *args)
{
    int socket_desc;
    const char *key;
    const char *value;
    int lifetime;

    if (!PyArg_ParseTuple(args, "issi", &socket_desc, &key, &value, &lifetime))
        return NULL;

    redis_server_socket_desc = socket_desc;
    int status = set_interactor(key, value, lifetime);

    switch (status) {
        case COMMAND_SENDING_ERROR:
            PyErr_SetString(CommandSendingError, "");
            return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
_myredis_send_wait_request(PyObject *self, PyObject *args)
{
    int socket_desc;
    int replicas_count;
    int timeout;

    if (!PyArg_ParseTuple(args, "iii", &socket_desc, &replicas_count, &timeout))
        return NULL;

    redis_server_socket_desc = socket_desc;
    int status = wait_interactor(replicas_count, timeout);

    switch (status) {
        case COMMAND_SENDING_ERROR:
            PyErr_SetString(CommandSendingError, "");
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
_myredis_send_config_get_request(PyObject *self, PyObject *args)
{
    int socket_desc;
    const char *key;

    if (!PyArg_ParseTuple(args, "is", &socket_desc, &key))
        return NULL;

    redis_server_socket_desc = socket_desc;
    int status = config_get_interactor(key);

    switch (status) {
        case COMMAND_SENDING_ERROR:
            PyErr_SetString(CommandSendingError, "");
            return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef MyredisMethods[] = {
    {"connect_to_redis_server",  _myredis_connect_to_redis_server, METH_VARARGS, NULL},
    {"disconnect_from_redis_server",  _myredis_disconnect_from_redis_server, METH_VARARGS, NULL},
    {"send_ping_request",  _myredis_send_ping_request, METH_VARARGS, NULL},
    {"send_echo_request",  _myredis_send_echo_request, METH_VARARGS, NULL},
    {"send_get_request",  _myredis_send_get_request, METH_VARARGS, NULL},
    {"send_set_request",  _myredis_send_set_request, METH_VARARGS, NULL},
    {"send_wait_request",  _myredis_send_wait_request, METH_VARARGS, NULL},
    {"send_config_get_request",  _myredis_send_config_get_request, METH_VARARGS, NULL},
    {"get_response",  _myredis_get_response, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef _myredis_libmodule = {
    PyModuleDef_HEAD_INIT,
    "_myredis",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    MyredisMethods,
};

PyMODINIT_FUNC PyInit__myredis(void)
{
    PyObject *module = PyModule_Create(&_myredis_libmodule);

    if (module == NULL)
        return NULL;

    CommandSendingError = PyErr_NewException("myredis.CommandSendingError", NULL, NULL);
    Py_IncRef(CommandSendingError);
    if (PyModule_AddObject(module, "CommandSendingError", CommandSendingError) < 0) {
        Py_XDECREF(CommandSendingError);
        Py_CLEAR(CommandSendingError);
        Py_DECREF(CommandSendingError);
        return NULL;
    }

    UnknownServerResponseError = PyErr_NewException("myredis.UnknownServerResponseError", NULL, NULL);
    Py_IncRef(UnknownServerResponseError);
    if (PyModule_AddObject(module, "UnknownServerResponseError", UnknownServerResponseError) < 0) {
        Py_XDECREF(UnknownServerResponseError);
        Py_CLEAR(UnknownServerResponseError);
        Py_DECREF(UnknownServerResponseError);
        return NULL;
    }

    return module;
}

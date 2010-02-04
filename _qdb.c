#include "Python.h"
#include <dystopia.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/** type TCQDBError **/
static PyObject *Py_TCQDBError;

/** type TCQDB **/
typedef struct {
	PyObject_HEAD
	TCQDB *qdb;
} TCQDB_object;

/** TCQDB functions **/
static void TCQDB_dealloc(TCQDB_object *self) {
	tcqdbdel(self->qdb);
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *TCQDB_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	TCQDB_object *self;
	
	self = (TCQDB_object *)type->tp_alloc(type, 0);
	if(self != NULL) {
		self->qdb = tcqdbnew();
		if(!self->qdb) {
			Py_DECREF(self);
			return NULL;
		}
	}
	return (PyObject *)self;
}

static PyObject *TCQDB_error_from_qdb(TCQDB_object *self) {
	int ecode = tcqdbecode(self->qdb);
	
	return PyErr_Format(Py_TCQDBError, "tcqdb returns error code %d", ecode);
}

static PyObject *TCQDB_open(TCQDB_object *self, PyObject *args) {
	char *path = NULL;
	int omode = QDBOREADER;
	
	if(!PyArg_ParseTuple(args, "s|i", &path, &omode))
		return NULL;

	if(!tcqdbopen(self->qdb, path, omode)) {
		// error
		return TCQDB_error_from_qdb(self);
	}
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *TCQDB_close(TCQDB_object *self) {
	if(!tcqdbclose(self->qdb))
		return TCQDB_error_from_qdb(self);
	
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *TCQDB_put(TCQDB_object *self, PyObject *args) {
	PY_LONG_LONG record_id = 0;
	char *text = NULL;
	bool result;
	
	if(!PyArg_ParseTuple(args, "Les", &record_id, "utf-8", &text))
		return NULL;
	
	Py_BEGIN_ALLOW_THREADS
	result = tcqdbput(self->qdb, record_id, text);
	Py_END_ALLOW_THREADS
	if(!result) {
		// error
		PyMem_Free(text);
		return TCQDB_error_from_qdb(self);
	}
	
	PyMem_Free(text);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *TCQDB_out(TCQDB_object *self, PyObject *args) {
	PY_LONG_LONG record_id = 0;
	char *text = NULL;
	bool result;
		
	if(!PyArg_ParseTuple(args, "Les", &record_id, "utf-8", &text))
		return NULL;

	Py_BEGIN_ALLOW_THREADS
	result = tcqdbout(self->qdb, record_id, text);
	Py_END_ALLOW_THREADS
	
	if(!result) {
		// error
		PyMem_Free(text);
		return TCQDB_error_from_qdb(self);
	}
	
	PyMem_Free(text);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *TCQDB_search(TCQDB_object *self, PyObject *args) {
	char *word = NULL;
	int smode = QDBSSUBSTR;
	uint64_t *results;
	int nresults = 0, i;
	PyObject *pyresult = NULL, *var;
	
	if(!PyArg_ParseTuple(args, "es|i", "utf-8", &word, &smode))
		return NULL;
	
	Py_BEGIN_ALLOW_THREADS
	results = tcqdbsearch(self->qdb, word, smode, &nresults);
	Py_END_ALLOW_THREADS
	
	if(!results) {
		return TCQDB_error_from_qdb(self);
	}
	
	// copy results to list
	// shuld consider using array
	pyresult = PyList_New(nresults);
	for(i=0;i<nresults;++i) {
		PyList_SET_ITEM(pyresult, i, PyLong_FromLongLong(results[i]));
	}
	free(results);
	
	return pyresult;
}

static PyMethodDef tcqdb_methods[] = {
	{ "open",	TCQDB_open,		METH_VARARGS,
	  "open a q-gram database object." },
	{ "close",	TCQDB_close,	METH_NOARGS,
	  "close a q-gram database object." },
	{ "put",	TCQDB_put,		METH_VARARGS,
	  "store a record." },
	{ "out",	TCQDB_out,		METH_VARARGS,
	  "remove a record." },
	{ "search",	TCQDB_search,	METH_VARARGS,
	  "search a database." },
	{ NULL, NULL },
};

static PyTypeObject TCQDB_type = {
	PyObject_HEAD_INIT(NULL)
    0,							/* ob_size*/
    "_qdb.TCQDB",             	/* tp_name*/
    sizeof(TCQDB_object), 		/* tp_basicsize*/
    0,							/* tp_itemsize*/
    (destructor)TCQDB_dealloc,	/* tp_dealloc*/
    0,							/* tp_print*/
    0,							/* tp_getattr*/
    0,							/* tp_setattr*/
    0,							/* tp_compare*/
    0,							/* tp_repr*/
    0,							/* tp_as_number*/
    0,							/* tp_as_sequence*/
    0,							/* tp_as_mapping*/
    0,							/* tp_hash */
    0,							/* tp_call*/
    0,							/* tp_str*/
    0,							/* tp_getattro*/
    0,							/* tp_setattro*/
    0,							/* tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,	/* tp_flags*/
    "TCQDB Wrapper",           	/* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    tcqdb_methods,				/* tp_methods */
    0,        				    /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,   					   /* tp_init */
    0,                         /* tp_alloc */
    TCQDB_new,					/* tp_new */
};

/** module initializing functions **/
static PyMethodDef module_methods[] = {
  {NULL, NULL},
};

void init_qdb(){
    PyObject *module;
    
    if(PyType_Ready(&TCQDB_type) < 0)
    	return;
    
    module = Py_InitModule("_qdb", module_methods);
    if(!module)
    	return;
    
    Py_TCQDBError = PyErr_NewException("_qdb.TCQDBError", NULL, NULL);
    PyModule_AddObject(module, "TCQDBError", Py_TCQDBError);
    
    Py_INCREF(&TCQDB_type);
    PyModule_AddObject(module, "TCQDB", (PyObject *)&TCQDB_type);
    
	PyModule_AddIntConstant(module, "QDBOREADER", QDBOREADER);
	PyModule_AddIntConstant(module, "QDBOWRITER", QDBOWRITER);
	PyModule_AddIntConstant(module, "QDBOCREAT", QDBOCREAT);
	PyModule_AddIntConstant(module, "QDBOTRUNC", QDBOTRUNC);
	PyModule_AddIntConstant(module, "QDBONOLCK", QDBONOLCK);
	PyModule_AddIntConstant(module, "QDBOLCKNB", QDBOLCKNB);
	
	PyModule_AddIntConstant(module, "TCESUCCESS", TCESUCCESS);
	PyModule_AddIntConstant(module, "TCETHREAD", TCETHREAD);
	PyModule_AddIntConstant(module, "TCEINVALID", TCEINVALID);
	PyModule_AddIntConstant(module, "TCENOFILE", TCENOFILE);
	PyModule_AddIntConstant(module, "TCENOPERM", TCENOPERM);
	PyModule_AddIntConstant(module, "TCEMETA", TCEMETA);
	PyModule_AddIntConstant(module, "TCERHEAD", TCERHEAD);
}

#if 0
  TCESUCCESS,                            /* success */
  TCETHREAD,                             /* threading error */
  TCEINVALID,                            /* invalid operation */
  TCENOFILE,                             /* file not found */
  TCENOPERM,                             /* no permission */
  TCEMETA,                               /* invalid meta data */
  TCERHEAD,                              /* invalid record header */
  TCEOPEN,                               /* open error */
  TCECLOSE,                              /* close error */
  TCETRUNC,                              /* trunc error */
  TCESYNC,                               /* sync error */
  TCESTAT,                               /* stat error */
  TCESEEK,                               /* seek error */
  TCEREAD,                               /* read error */
  TCEWRITE,                              /* write error */
  TCEMMAP,                               /* mmap error */
  TCELOCK,                               /* lock error */
  TCEUNLINK,                             /* unlink error */
  TCERENAME,                             /* rename error */
  TCEMKDIR,                              /* mkdir error */
  TCERMDIR,                              /* rmdir error */
  TCEKEEP,                               /* existing record */
  TCENOREC,                              /* no record found */
  TCEMISC = 9999                         /* miscellaneous error */
#endif

#include "Python.h"
#include <dystopia.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


static PyObject* idbclose(PyObject *self, PyObject *args)
{
  int ecode;
  TCIDB* idb;
  PyObject *obj;
  if (!PyArg_ParseTuple(args, "O", &obj ))
        return NULL;
  idb = (TCIDB *) PyCObject_AsVoidPtr(obj);
  if(!tcidbclose(idb)){
        ecode = tcidbecode(idb);
        fprintf(stderr, "open error: %s\n", tcidberrmsg(ecode));
    }
  tcidbdel(idb);
  return Py_None;
}

static PyObject *open(PyObject *self, PyObject *args)
{
  const char *dbname;
  int mask;
  TCIDB *idb;
  int ecode;
  PyObject *c_object;

  if (!PyArg_ParseTuple(args, "si", &dbname, &mask))
        return NULL;
  idb = tcidbnew();
  if(!tcidbopen(idb, dbname, mask)){
        ecode = tcidbecode(idb);
        fprintf(stderr, "open error: %s\n", tcidberrmsg(ecode));
    }
  c_object = PyCObject_FromVoidPtr(idb, NULL);
  return c_object;
}



static PyObject *
put(PyObject *self,PyObject *args){
    PyObject *db;
    const char *stext;
    const int *docid;
    int ecode;
    bool result;
    TCIDB *idb;
    if (!PyArg_ParseTuple(args, "Ois", &db, &docid, &stext))
        return NULL;
    idb = (TCIDB *) PyCObject_AsVoidPtr(db);
    result = tcidbput(idb,(int64_t)docid,stext);
    
    /* delete the object */
    return Py_BuildValue("b",result);
}

static PyObject *idbinfo(PyObject *self, PyObject *args)
{
  PyObject *db;
  TCIDB *idb;
  int ecode;
  uint64_t number, size;
  PyObject *tuple;
  if (!PyArg_ParseTuple(args, "O", &db))
        return NULL;
  idb = (TCIDB *) PyCObject_AsVoidPtr(db);
  number = tcidbrnum(idb);
  size = tcidbfsiz(idb);
  tuple = Py_BuildValue("ii", number, size);
  return tuple;
}
    



static PyObject *search(PyObject *self, PyObject *args){
    const char *stext;
    int ecode, rnum, i;
    uint64_t *result;
    PyObject* pList;
    PyObject *db;
    TCIDB *idb;
    
    if (!PyArg_ParseTuple(args, "Os", &db, &stext))
        return NULL;
    
    idb = (TCIDB *) PyCObject_AsVoidPtr(db);
    /* search records */
    result = tcidbsearch2(idb, stext, &rnum);
    pList = PyList_New(rnum);
    if(result){
        for(i = 0; i < rnum; i++){
            // printf("r[i]:%lld\n",result[i]);
            PyList_SetItem(pList, i, Py_BuildValue("i", (int)result[i]));
        }
        tcfree(result);
    } else {
        ecode = tcidbecode(idb);
        fprintf(stderr, "search error: %s\n", tcidberrmsg(ecode));
    }
    
    return Py_BuildValue("O",pList);
}

PyMethodDef methods[] = {
  {"open", open, METH_VARARGS},
  {"search", search, METH_VARARGS},
  {"put", put, METH_VARARGS},
  {"close", idbclose, METH_VARARGS},
  {"info", idbinfo, METH_VARARGS},
  {NULL, NULL},
};

void init_dystopia(){
    PyObject* m;
    m = Py_InitModule("_dystopia", methods);
}

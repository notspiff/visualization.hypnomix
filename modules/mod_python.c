#include <Python.h>

#include <stdio.h>

#include "hypnomix.h"
#include "module.h"

PyObject *pmodule, *pdict, *presult;
PyObject *pinit, *pdraw, *pdestroy, *phyp, *ppg, *pvalue, *psamples;


char modname[] = "python";

struct transform *tr;
struct program *pg;


static PyObject *wrapperTransformIdentity(PyObject *self)
{
	transformIdentity(tr);
	return Py_None;
}


static PyObject *wrapperTransformViewRotate(PyObject *self, PyObject *args)
{
	float ax, ay, az;

	if(!PyArg_ParseTuple(args, "fff", &ax, &ay, &az)) {
		return NULL;
	}
	transformViewRotate(tr, ax, ay, az);
	return Py_None;
}


static PyObject *wrapperTransformViewTranslate(PyObject *self, PyObject *args)
{
	float tx, ty, tz;

	if(!PyArg_ParseTuple(args, "fff", &tx, &ty, &tz)) {
		return NULL;
	}
	transformViewTranslate(tr, tx, ty, tz);
	return Py_None;
}


static PyObject *wrapperTransformModelRotate(PyObject *self, PyObject *args)
{
	float ax, ay, az;

	if(!PyArg_ParseTuple(args, "fff", &ax, &ay, &az)) {
		return NULL;	
	}
	transformModelRotate(tr, ax, ay, az);
	return Py_None;
}


static PyObject *wrapperTransformModelTranslate(PyObject *self, PyObject *args)
{
	float tx, ty, tz;

	if(!PyArg_ParseTuple(args, "fff", &tx, &ty, &tz)) {
		return NULL;
	}
	transformModelTranslate(tr, tx, ty, tz);
	return Py_None;
}


static PyObject *wrapperTransformModelScale(PyObject *self, PyObject *args)
{
	float sx, sy, sz;

	if(!PyArg_ParseTuple(args, "fff", &sx, &sy, &sz)) {
		return NULL;
	}
	transformModelScale(tr, sx, sy, sz);
	return Py_None;
}


static PyObject *wrapperTransformMVP(PyObject *self)
{
	transformMVP(tr);
	glUniformMatrix4fv(pg->mvp, 1, GL_FALSE, tr->mvp);
	glUniformMatrix3fv(pg->mnormal, 1, GL_FALSE, tr->mnormal);
	return Py_None;
}


static PyMethodDef methodsdef[] = {
	{"identity", (PyCFunction)wrapperTransformIdentity, METH_NOARGS, NULL},
	{"viewRotate", wrapperTransformViewRotate, METH_VARARGS, NULL},
	{"viewTranslate", wrapperTransformViewTranslate, METH_VARARGS, NULL},
	{"modelRotate", wrapperTransformModelRotate, METH_VARARGS, NULL},
	{"modelTranslate", wrapperTransformModelTranslate, METH_VARARGS, NULL},
	{"modelScale", wrapperTransformModelScale, METH_VARARGS, NULL},
	{"MVP", (PyCFunction)wrapperTransformMVP, METH_NOARGS, NULL},
	{NULL, NULL, 0, NULL}
};


static void buildProgram(const char *key, int value)
{
	pvalue = PyInt_FromLong(value);
	PyDict_SetItem(ppg, Py_BuildValue("s", key), pvalue);
}


static void loadPython(struct hypnomix *hyp)
{
	pdict = PyModule_GetDict(pmodule);
	pinit = PyDict_GetItemString(pdict, "init");
	pdraw = PyDict_GetItemString(pdict, "draw");
	pdestroy = PyDict_GetItemString(pdict, "destroy");
	if(!PyCallable_Check(pinit) || !PyCallable_Check(pdraw)
		|| !PyCallable_Check(pdestroy)) {
		fprintf(stderr, "PYTHON: init(), destroy() or draw() not found\n");
		exit(1);
	}

	/* build module program python object */
	ppg = PyDict_New();
	buildProgram("id", hyp->pg.id);
	buildProgram("mvp", hyp->pg.mvp);
	buildProgram("mnormal",	hyp->pg.mnormal);
	buildProgram("mmodel", hyp->pg.mmodel);
	buildProgram("pos", hyp->pg.pos);
	buildProgram("nrm", hyp->pg.nrm);
	buildProgram("clr", hyp->pg.clr);
	buildProgram("texpos", hyp->pg.texpos);
	buildProgram("texture0", hyp->pg.texture0);
	buildProgram("texture1", hyp->pg.texture1);
	/*buildProgram("lightpos", hyp->pg.lightpos);
	buildProgram("lightclr", hyp->pg.lightclr);
	buildProgram("lightdir", hyp->pg.lightdir);
	buildProgram("nblights", hyp->pg.nblights); */

	phyp = PyDict_New(); /* FIXME: decref this ? */
	PyDict_SetItem(phyp, Py_BuildValue("s", "w"), PyInt_FromLong(hyp->w));
	PyDict_SetItem(phyp, Py_BuildValue("s", "h"), PyInt_FromLong(hyp->h));
	PyDict_SetItem(phyp, Py_BuildValue("s", "home"), 
		Py_BuildValue("s", hyp->home));
	presult = PyObject_CallFunctionObjArgs(pinit, phyp, NULL);
	if(presult != NULL) {
		Py_DECREF(presult);
	}

	psamples = PyList_New(hyp->nbsamples);
}


void init(struct hypnomix *hyp)
{
	tr = &hyp->tr;
	pg = &hyp->pg;
	transformProjection(tr, hyp->pmatrix);
	hyp->var.nbbands = 3;

	setenv("PYTHONPATH", hyp->home, 1);
	fprintf(stderr, "INFO: PYTHONPATH=%s\n", getenv("PYTHONPATH"));
	Py_Initialize();
	if(Py_IsInitialized()) {
		fprintf(stderr, "PYTHON INIT\n");
	} else {
		fprintf(stderr, "PYTHON NOT INIT\n");
	}
	Py_InitModule("TRANSFORM", methodsdef);

	pmodule = PyImport_Import(PyString_FromString("mod_python"));
	if(!pmodule) {
		fprintf(stderr, "ERROR: python module not found\n");
		PyErr_Print();
		hyp->mod = NULL; /* FIXME: does it works? */
	} else {
		loadPython(hyp);
	} 
fprintf(stderr, "AFTER PYTHON INIT\n");
}


void destroy(struct hypnomix *hyp)
{
	if(pmodule) {
		presult = PyObject_CallObject(pdestroy, NULL);
		if(presult != NULL) {
			Py_DECREF(presult);
		}
		if(psamples != NULL) { /* FIXME: put this in a macro */
			Py_DECREF(psamples);
		}
		Py_DECREF(pmodule);
	}
	Py_Finalize();
}


void draw(struct hypnomix *hyp)
{
	int i;

	/* pass samples to python */
	/* FIXME: find a way to use pointer... */	
	for(i = 0; i < hyp->nbsamples; i++) {
		PyList_SET_ITEM(psamples, i, 
			PyFloat_FromDouble(hyp->samples[i]));
	} 

	glUseProgram(hyp->pg.id);
	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr); 
	if(hyp->pg.nrm != -1) { /* FIXME: shader must have nrm, or not? */
		glEnableVertexAttribArray(hyp->pg.nrm);  
	}

	glUniform3fv(hyp->pg.lightclr, hyp->lights.nb, hyp->lights.clr);
	glUniform1i(hyp->pg.nblights, hyp->lights.nb);

	/* call the python draw method */	
	if(pmodule) {
		presult = PyObject_CallFunctionObjArgs(pdraw, 
			ppg, psamples, NULL);
		// presult = PyObject_CallObject(pdraw, NULL);
		if(presult != NULL) {
			Py_DECREF(presult);
		}
	}

	if(hyp->pg.nrm != -1) {
		glDisableVertexAttribArray(hyp->pg.nrm); 
	}
	glDisableVertexAttribArray(hyp->pg.clr); 
	glDisableVertexAttribArray(hyp->pg.pos);   
}

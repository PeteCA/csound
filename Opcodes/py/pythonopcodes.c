/*
  * pythonopcodes.c
  *
  * Copyright (C) 2002 Maurizio Umberto Puxeddu
  *
  * This software is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This software is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this software; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#ifdef EMBEDDED_PYTHON

#include <Python.h>
#include "py/pythonopcodes.h"
#include "py/pythonhelper.h"

/* HELPERS */

static void
create_private_namespace_if_needed(OPDS *o)
{
  if (GETPYLOCAL(o->insdshead) == 0)
    {
      SETPYLOCAL(o->insdshead, PyDict_New());
#ifdef DEBUG_PY_NAMESPACES
      printf("Creating private namespace %x for %x\n", (void*)GETPYLOCAL(o->insdshead), (void*)o->insdshead);
#endif
    }
  else
    {
#ifdef DEBUG_PY_NAMESPACES
      printf("Private namespace for %x already allocated at %x\n", (void*)o->insdshead, (void*)GETPYLOCAL(o->insdshead));
#endif
    }
}

static void
format_call_statement(char *statement, char *callable, int argc, MYFLT *argv[], int skip)
{
  int i;

  statement[0] = '\0';
  if (argc > 0)
    {
      sprintf(statement, "%s(%0.6f", callable, *(argv[0]));
      for (i = 1; i < argc - skip; ++i)
	{
	  sprintf(statement + strlen(statement), ", %f", *(argv[i]));
	}
      strcat(statement, ")");
    }
  else
    {
      sprintf(statement, "%s()", callable);
    }
}

static PyObject *
run_statement_in_given_context(char *string, PyObject *private)
{
  PyObject *module, *public;
  module = PyImport_AddModule("__main__");
  if (module == NULL)
    {
      PyErr_SetString(PyExc_RuntimeError, "couldn't find module __main__");
      return NULL;
    }
  public = PyModule_GetDict(module);
  return PyRun_String(string, Py_file_input, public, private? private: public);
}

static PyObject *
eval_string_in_given_context(char *string, PyObject *private)
{
  PyObject *module, *public;
  module = PyImport_AddModule("__main__");
  if (module == NULL)
    {
      PyErr_SetString(PyExc_RuntimeError, "couldn't find module __main__");
      return NULL;
    }
  public = PyModule_GetDict(module);
  return PyRun_String(string, Py_eval_input, public, private? private: public);
}

static PyObject *
exec_file_in_given_context(char *filename, PyObject *private)
{
  FILE *file;
  PyObject *result, *module, *public;
  module = PyImport_AddModule("__main__");
  if (module == NULL)
    {
      PyErr_SetString(PyExc_RuntimeError, "couldn't find module __main__");
      return NULL;
    }
  public = PyModule_GetDict(module);
  file = fopen(filename, "r");
  if (file == 0)
    {
      PyErr_Format(PyExc_RuntimeError, "couldn't open script file %s", filename);
      return NULL;
    }
  result = PyRun_File(file, filename, Py_file_input, public, private? private: public);
  fclose(file);
  return result;
}

// ------ OPCODES

#include "pyx.c.auto"
#include "pycall.c.auto"

void
pycalln_krate(PYCALLN *p)
{
  int i;
  char command[1024];
  PyObject *result, *module, *namespace;

  if (*p->function != sstrcod)
    return;

  format_call_statement(command, unquote(p->STRARG), p->INOCOUNT, p->args, (int)*p->nresult + 1);

  result = eval_string_in_given_context(command, 0);
  if (result != NULL && PyTuple_Check(result) && PyTuple_Size(result) == (int)*p->nresult)
    {
      for (i = 0; i < *p->nresult; ++i)
	*p->args[i] = PyFloat_AsDouble(PyTuple_GET_ITEM(result, i));
      Py_DECREF(result);
    }
  else
    {
      err_printf("pycalln_krate: ERROR\n");
      PyErr_Print();
    }
}

void
pylcalln_irate(PYCALLN *p)
{
  if (*p->function != sstrcod)
    return;

  create_private_namespace_if_needed(&p->h);
}

void
pylcalln_krate(PYCALLN *p)
{
  int i;
  char command[1024];
  PyObject *result, *module, *namespace;

  if (*p->function != sstrcod)
    return;

  format_call_statement(command, unquote(p->STRARG), p->INOCOUNT, p->args, (int)*p->nresult + 1);

  result = eval_string_in_given_context(command, GETPYLOCAL(p->h.insdshead));
  if (result != NULL && PyTuple_Check(result) && PyTuple_Size(result) == (int)*p->nresult)
    {
      for (i = 0; i < *p->nresult; ++i)
	*p->args[i] = PyFloat_AsDouble(PyTuple_GET_ITEM(result, i));
      Py_DECREF(result);
    }
  else
    {
      err_printf("pycalln: ERROR\n");
      PyErr_Print();
    }
}

void
pylcallni_irate(PYCALLN *p)
{
  int i;
  char command[1024];
  PyObject *result, *module, *namespace;

  if (*p->function != sstrcod)
    return;

  create_private_namespace_if_needed(&p->h);

  format_call_statement(command, unquote(p->STRARG), p->INOCOUNT, p->args, (int)*p->nresult + 1);

  result = eval_string_in_given_context(command, GETPYLOCAL(p->h.insdshead));
  if (result != NULL && PyTuple_Check(result) && PyTuple_Size(result) == (int)*p->nresult)
    {
      for (i = 0; i < *p->nresult; ++i)
	*p->args[i] = PyFloat_AsDouble(PyTuple_GET_ITEM(result, i));
      Py_DECREF(result);
    }
  else
    {
      err_printf("pycalln: ERROR\n");
      PyErr_Print();
    }
}

  /* PYTHON OPCODES */

OENTRY oentries[] = {
  /* RUN GROUP */

{ "pyrun",     S(PYRUN),     2,  "",            "S",     NULL,                 (SUBR)pyrun_krate   },
{ "pyruni",    S(PYRUN),     1,  "",            "S",     (SUBR)pyruni_irate                        },
{ "pylrun",    S(PYRUN),     3,  "",            "S",     (SUBR)pylrun_irate,   (SUBR)pylrun_krate  },
{ "pylruni",   S(PYRUN),     1,  "",            "S",     (SUBR)pylruni_irate                       },

{ "pyrunt",    S(PYRUNT),    2,  "",            "kS",     NULL,                 (SUBR)pyrunt_krate  },
{ "pylrunt",   S(PYRUNT),    3,  "",            "kS",     (SUBR)pylrunt_irate,  (SUBR)pylrunt_krate },

  /* EXEC GROUP */

{ "pyexec",    S(PYEXEC),    2,  "",            "S",     NULL,                (SUBR)pyexec_krate    },
{ "pyexeci",   S(PYEXEC),    1,  "",            "S",     (SUBR)pyexec_krate                         },
{ "pylexec",   S(PYEXEC),    2,  "",            "S",     (SUBR)pylexec_irate,  (SUBR)pylexec_krate  },
{ "pylexeci",  S(PYEXEC),    1,  "",            "S",     (SUBR)pylexeci_irate                       },

{ "pyexect",    S(PYEXECT),    2,  "",            "kS",     NULL,                (SUBR)pyexect_krate },
{ "pylexect",    S(PYEXECT),   2,  "",            "kS",     (SUBR)pylexec_irate,  (SUBR)pylexect_krate },  
  /* CALL GROUP */

{ "pycall",    S(PYCALL0),   2,  "" ,           "Sz",    NULL,                (SUBR)pycall0_krate },
{ "pycall1",   S(PYCALL1),   2,  "k",           "Sz",    NULL,                (SUBR)pycall1_krate },
{ "pycall2",   S(PYCALL2),   2,  "kk",          "Sz",    NULL,                (SUBR)pycall2_krate },
{ "pycall3",   S(PYCALL3),   2,  "kkk",         "Sz",    NULL,                (SUBR)pycall3_krate },
{ "pycall4",   S(PYCALL4),   2,  "kkkk",        "Sz",    NULL,                (SUBR)pycall4_krate },
{ "pycall5",   S(PYCALL5),   2,  "kkkkk",       "Sz",    NULL,                (SUBR)pycall5_krate },
{ "pycall6",   S(PYCALL6),   2,  "kkkkkk",      "Sz",    NULL,                (SUBR)pycall6_krate },
{ "pycall7",   S(PYCALL7),   2,  "kkkkkkk",     "Sz",    NULL,                (SUBR)pycall7_krate },
{ "pycall8",   S(PYCALL8),   2,  "kkkkkkkk",    "Sz",    NULL,                (SUBR)pycall8_krate },

{ "pycalln",   S(PYCALLN),   2,  "",            "Siz",    NULL,                (SUBR)pycalln_krate },

{ "pycallt",    S(PYCALL0T),   2,  "" ,           "kSz",    NULL,                (SUBR)pycall0t_krate },
{ "pycall1t",   S(PYCALL1T),   2,  "k",           "kSz",    NULL,                (SUBR)pycall1t_krate },
{ "pycall2t",   S(PYCALL2T),   2,  "kk",          "kSz",    NULL,                (SUBR)pycall2t_krate },
{ "pycall3t",   S(PYCALL3T),   2,  "kkk",         "kSz",    NULL,                (SUBR)pycall3t_krate },
{ "pycall4t",   S(PYCALL4T),   2,  "kkkk",        "kSz",    NULL,                (SUBR)pycall4t_krate },
{ "pycall5t",   S(PYCALL5T),   2,  "kkkkk",       "kSz",    NULL,                (SUBR)pycall5t_krate },
{ "pycall6t",   S(PYCALL6T),   2,  "kkkkkk",      "kSz",    NULL,                (SUBR)pycall6t_krate },
{ "pycall7t",   S(PYCALL7T),   2,  "kkkkkkk",     "kSz",    NULL,                (SUBR)pycall7t_krate },
{ "pycall8t",   S(PYCALL8T),   2,  "kkkkkkkk",    "kSz",    NULL,                (SUBR)pycall8t_krate },

#if 0
{ "pycallnt",   S(PYCALLNT),   2,  "",            "Siz",    NULL,                (SUBR)pycallnt_krate },
#endif

{ "pycalli",   S(PYCALL0),   2,  "i",           "Sm",    (SUBR)pycall0_krate },
{ "pycall1i",  S(PYCALL1),   1,  "i",           "Sm",    (SUBR)pycall1_krate },
{ "pycall2i",  S(PYCALL2),   1,  "ii",          "Sm",    (SUBR)pycall2_krate },
{ "pycall3i",  S(PYCALL3),   1,  "iii",         "Sm",    (SUBR)pycall3_krate },
{ "pycall4i",  S(PYCALL4),   1,  "iiii",        "Sm",    (SUBR)pycall4_krate },
{ "pycall5i",  S(PYCALL5),   1,  "iiiii",       "Sm",    (SUBR)pycall5_krate },
{ "pycall6i",  S(PYCALL6),   1,  "iiiiii",      "Sm",    (SUBR)pycall6_krate },
{ "pycall7i",  S(PYCALL7),   1,  "iiiiiii",     "Sm",    (SUBR)pycall7_krate },
{ "pycall8i",  S(PYCALL8),   1,  "iiiiiiii",    "Sm",    (SUBR)pycall8_krate },

{ "pycallni",  S(PYCALL8),   1,  "",            "Sim",   (SUBR)pycalln_krate },

{ "pylcall",    S(PYCALL0),   2,  "" ,           "Sz",    (SUBR)pylcall0_irate, (SUBR)pylcall0_krate },
{ "pylcall1",   S(PYCALL1),   2,  "k",           "Sz",    (SUBR)pylcall1_irate, (SUBR)pylcall1_krate },
{ "pylcall2",   S(PYCALL2),   2,  "kk",          "Sz",    (SUBR)pylcall2_irate, (SUBR)pylcall2_krate },
{ "pylcall3",   S(PYCALL3),   2,  "kkk",         "Sz",    (SUBR)pylcall3_irate, (SUBR)pylcall3_krate },
{ "pylcall4",   S(PYCALL4),   2,  "kkkk",        "Sz",    (SUBR)pylcall4_irate, (SUBR)pylcall4_krate },
{ "pylcall5",   S(PYCALL5),   2,  "kkkkk",       "Sz",    (SUBR)pylcall5_irate, (SUBR)pylcall5_krate },
{ "pylcall6",   S(PYCALL6),   2,  "kkkkkk",      "Sz",    (SUBR)pylcall6_irate, (SUBR)pylcall6_krate },
{ "pylcall7",   S(PYCALL7),   2,  "kkkkkkk",     "Sz",    (SUBR)pylcall7_irate, (SUBR)pylcall7_krate },
{ "pylcall8",   S(PYCALL8),   2,  "kkkkkkkk",    "Sz",    (SUBR)pylcall8_irate, (SUBR)pylcall8_krate },

{ "pylcalln",   S(PYCALLN),   2,  "",            "Siz",    (SUBR)pylcalln_irate, (SUBR)pylcalln_krate },

{ "pylcallt",    S(PYCALL0T),   2,  "" ,           "kSz",    (SUBR)pylcall0t_irate, (SUBR)pylcall0t_krate },
{ "pylcall1t",   S(PYCALL1T),   2,  "k",           "kSz",    (SUBR)pylcall1t_irate, (SUBR)pylcall1t_krate },
{ "pylcall2t",   S(PYCALL2T),   2,  "kk",          "kSz",    (SUBR)pylcall2t_irate, (SUBR)pylcall2t_krate },
{ "pylcall3t",   S(PYCALL3T),   2,  "kkk",         "kSz",    (SUBR)pylcall3t_irate, (SUBR)pylcall3t_krate },
{ "pylcall4t",   S(PYCALL4T),   2,  "kkkk",        "kSz",    (SUBR)pylcall4t_irate, (SUBR)pylcall4t_krate },
{ "pylcall5t",   S(PYCALL5T),   2,  "kkkkk",       "kSz",    (SUBR)pylcall5t_irate, (SUBR)pylcall5t_krate },
{ "pylcall6t",   S(PYCALL6T),   2,  "kkkkkk",      "kSz",    (SUBR)pylcall6t_irate, (SUBR)pylcall6t_krate },
{ "pylcall7t",   S(PYCALL7T),   2,  "kkkkkkk",     "kSz",    (SUBR)pylcall7t_irate, (SUBR)pylcall7t_krate },
{ "pylcall8t",   S(PYCALL8T),   2,  "kkkkkkkk",    "kSz",    (SUBR)pylcall8t_irate, (SUBR)pylcall8t_krate },

#if 0
{ "pylcallnt",   S(PYCALLNT),   2,  "",            "Siz",    (SUBR)pylcalln_irate, (SUBR)pylcallnt_krate },
#endif

{ "pylcalli",   S(PYCALL0),   2,  "i",           "Sm",    (SUBR)pylcall0i_irate },
{ "pylcall1i",  S(PYCALL1),   1,  "i",           "Sm",    (SUBR)pylcall1i_irate },
{ "pylcall2i",  S(PYCALL2),   1,  "ii",          "Sm",    (SUBR)pylcall2i_irate },
{ "pylcall3i",  S(PYCALL3),   1,  "iii",         "Sm",    (SUBR)pylcall3i_irate },
{ "pylcall4i",  S(PYCALL4),   1,  "iiii",        "Sm",    (SUBR)pylcall4i_irate },
{ "pylcall5i",  S(PYCALL5),   1,  "iiiii",       "Sm",    (SUBR)pylcall5i_irate },
{ "pylcall6i",  S(PYCALL6),   1,  "iiiiii",      "Sm",    (SUBR)pylcall6i_irate },
{ "pylcall7i",  S(PYCALL7),   1,  "iiiiiii",     "Sm",    (SUBR)pylcall7i_irate },
{ "pylcall8i",  S(PYCALL8),   1,  "iiiiiiii",    "Sm",    (SUBR)pylcall8i_irate },

{ "pylcallni",  S(PYCALLN),   1,  "",            "Sim",   (SUBR)pylcallni_irate },

  /* EVAL GROUP */

{ "pyeval",    S(PYEVAL),    2,  "k",           "S",    NULL, (SUBR)pyeval_krate },
{ "pyevali",   S(PYEVAL),    1,  "i",           "S",    (SUBR)pyeval_krate },
{ "pyleval",    S(PYEVAL),    2,  "k",           "S",   (SUBR)pyleval_irate, (SUBR)pyleval_krate },
{ "pylevali",   S(PYEVAL),    1,  "i",           "S",    (SUBR)pylevali_irate },

{ "pyevalt",    S(PYEVALT),    2,  "k",           "S",    NULL, (SUBR)pyevalt_krate },
{ "pylevalt",    S(PYEVALT),    2,  "k",           "S",   (SUBR)pyleval_irate, (SUBR)pylevalt_krate },

  /* ASSIGN GROUP */

{ "pyassign",  S(PYASSIGN),  2,  "",            "Sz",    NULL, (SUBR)pyassign_krate },
{ "pyassigni", S(PYASSIGN),  1,  "",            "Sz",    (SUBR)pyassign_krate },
{ "pylassign",  S(PYASSIGN),  2,  "",            "Sz",   (SUBR)pylassign_krate, (SUBR)pylassign_krate },
{ "pylassigni", S(PYASSIGN),  1,  "",            "Sz",   (SUBR)pylassigni_irate },

{ "pyassignt",  S(PYASSIGNT),  2,  "",            "Sz",    NULL, (SUBR)pyassignt_krate },
{ "pylassignt",  S(PYASSIGNT),  2,  "",            "Sz",   (SUBR)pylassign_krate, (SUBR)pylassignt_krate },

};

/**
* Called by Csound to obtain the size of
* the table of OENTRY structures defined in this shared library.
*/
PUBLIC int opcode_size()
{
    return sizeof(oentries);
}

/**
* Called by Csound to obtain a pointer to
* the table of OENTRY structures defined in this shared library.
*/
PUBLIC OENTRY *opcode_init(ENVIRON *csound)
{
    return oentry;
}


#endif // EMBEDDED_PYTHON

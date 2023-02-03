# SPDX-FileCopyrightText: 2023 Manuel Weiss <manuel.weiss@bht-berlin.de>
#
# SPDX-License-Identifier: MIT

# distutils: language = c++
# cython: language_level=3
# cython: embedsignature=True
# distutils: sources = hwdaq/cpp/hwdaq.cpp
# distutils: undef_macros = NDEBUG

import numpy as np

from libcpp cimport bool
from libc.string cimport memcpy
cimport numpy as np
cimport cython


ctypedef double hwdaq_t


cdef extern from 'cpp/hwdaq.hpp':
    cdef cppclass C_HWDAQ 'HWDAQ':
        C_HWDAQ() except +
    
        void getADC(double* vin)
        void setDAC(double* vout)

cdef class HWDAQ:
    cdef C_HWDAQ* c_obj

    def __cinit__(self):
        self.c_obj = new C_HWDAQ()

    def __init__(self):
        pass

    def __dealloc__(self):
        del self.c_obj

    def getADC(self):
        cdef np.ndarray[double, ndim=1, mode='c'] vin = np.empty(shape=(8,), dtype=np.double)
        self.c_obj.getADC(<double*> np.PyArray_DATA(vin))
        return vin
    
    

    def setDAC(self, vout):
        self.c_obj.setDAC(<double*> np.PyArray_DATA(vout))

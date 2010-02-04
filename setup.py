# -*- coding:utf-8 -*-
import os, sys
from distutils.core import setup, Extension

ext_args = {
    'libraries': ['tokyodystopia', 'tokyocabinet'],
    'include_dirs': ['/usr/local/include'],
    'library_dirs': ['/usr/local/lib'],
}

ext_modules = [
    Extension("_qdb", ["_qdb.c"], **ext_args)
]

setup(
    name='tokyodystopia',
    version='0.1',
    autho='Shin Adachi', author_email='shn@glucose.jp',
    description='Python binding for Tokyo Dystopia',
    ext_modules=ext_modules,
    py_modules=['tokyodystopia']
)

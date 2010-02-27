# -*- coding:utf-8 -*-
import os
import sys

sys.path.insert(0, os.path.join('build',
    'lib.freebsd-7.2-RELEASE-p2-amd64-2.6'))


def test_qdb():
    from tokyodystopia import TCQDB
    qdbobj = TCQDB()
    
    # open
    qdbobj.tune(1000000, TCQDB.QDBTBZIP)
    qdbobj.open('test.qdb', TCQDB.QDBOWRITER | TCQDB.QDBOCREAT)
    qdbobj.put(1, u'あだちちん')
    qdbobj.put(2, u'あだちめぐみ')
    qdbobj.put(3, u'あだちえりこ')
    qdbobj.put(4, u'おかじまれな')
    
    print qdbobj.search(u'あだち')

test_qdb()

# -*- coding:utf-8 -*-
"""

Python binding for Tokyo Dystopia

http://1978th.net/tokyodystopia/spex.html

"""

__version__ = '0.1'

import sys
import _qdb

# extract hear
TCQDB = _qdb.TCQDB
class TCQDB(_qdb.TCQDB):
    for key in dir(_qdb):
        if key.startswith('QDB'):
            locals()[key] = getattr(_qdb, key)
del _qdb

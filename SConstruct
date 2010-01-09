import distutils.sysconfig
vars = distutils.sysconfig.get_config_vars('CC', 'CXX', 'OPT', 'BASECFLAGS',
                                          'CCSHARED', 'LDSHARED', 'SO')
PLIBS = distutils.sysconfig.get_config_vars('BLDLIBRARY')[0].split(' ')[1]
PINCLUDES = '-I' + distutils.sysconfig.get_python_inc()
opts = Options()

env = Environment(options=opts)
env.AppendUnique(CFLAGS=['-Wall', PINCLUDES], LIBS=[PLIBS, 'tokyodystopia',  'tokyocabinet',  'bz2', 'z', 'pthread', 'm' , 'c'])
env.AppendUnique()	  
pyre = env.SharedLibrary('pykhufu' , ['pykhufu.c'], SHLIBPREFIX='')

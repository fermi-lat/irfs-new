#$Id$
def generate(env, **kw):
	if not kw.get('depsOnly',0):
		env.Tool('addLibrary', library='_pyIrfLoader')
	env.Tool('irfLoaderLib')

def exists(env):
	return 1

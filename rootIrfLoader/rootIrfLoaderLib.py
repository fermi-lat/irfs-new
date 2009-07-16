#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['rootIrfLoader'])
    env.Tool('irfLoaderLib')

def exists(env):
    return 1

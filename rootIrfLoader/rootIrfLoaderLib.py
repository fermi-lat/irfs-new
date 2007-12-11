def generate(env, **kw):
    env.Tool('addLibrary', library = ['rootIrfLoader'], package = 'irfs/rootIrfLoader')
    env.Tool('irfLoaderLib')

def exists(env):
    return 1

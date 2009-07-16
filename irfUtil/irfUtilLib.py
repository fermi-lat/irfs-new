#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['irfUtil'])
    env.Tool('astroLib')
    env.Tool('tipLib')
    env.Tool('st_facilitiesLib')
    env.Tool('addLibrary', library = env['f2cLibs'])

def exists(env):
    return 1

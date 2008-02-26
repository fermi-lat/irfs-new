# 
# $Header$
#

def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library=['irfInterface'])
    env.Tool('astroLib')
    env.Tool('st_facilitiesLib')

def exists(env):
    return 1

# 
# $Header$
#

def generate(env, **kw):
    env.Tool('addLibrary', library=['irfInterface'], 
             package='irfs/irfInterface')
    env.Tool('astroLib')
    env.Tool('st_facilitiesLib')

def exists(env):
    return 1

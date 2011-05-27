#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['irfLoader'])
    env.Tool('st_streamLib')
    env.Tool('tipLib')
    env.Tool('facilitiesLib')
    env.Tool('st_facilitiesLib')
    env.Tool('irfInterfaceLib')
    env.Tool('dc1ResponseLib')
    env.Tool('dc1aResponseLib')
    env.Tool('dc2ResponseLib')
    env.Tool('g25ResponseLib')
    env.Tool('handoff_responseLib')
    env.Tool('testResponseLib')
    env.Tool('latResponseLib')

def exists(env):
    return 1

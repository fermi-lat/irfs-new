def generate(env, **kw):
    env.Tool('irfInterfaceLib')
    env.Tool('irfUtilLib')
    env.Tool('irfLoaderLib')
    env.Tool('g25ResponseLib')
    env.Tool('dc1ResponseLib')
    env.Tool('dc1aResponseLib')
    env.Tool('testResponseLib')
    env.Tool('dc2ResponseLib')
    env.Tool('handoff_responseLib')

def exists(env):
    return 1

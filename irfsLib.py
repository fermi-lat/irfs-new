def generate(env, **kw):
    env.Tool('irfInterfaceLib')
    env.Tool('irfUtilLib')
    env.Tool('irfLoaderLib')
    env.Tool('dc1aResponseLib')
    env.Tool('testResponseLib')
    env.Tool('handoff_responseLib')

def exists(env):
    return 1

def generate(env, **kw):
    env.Tool('addLibrary', library = ['irfLoader'], package = 'irfs/irfLoader')
    env.Tool('st_appLib')
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

def exists(env):
    return 1

def generate(env, **kw):
    env.Tool('addLibrary', library = ['irfUtil', 'g2c'], package = 'irfs/irfUtil')
    env.Tool('astroLib')
    env.Tool('tipLib')

def exists(env):
    return 1

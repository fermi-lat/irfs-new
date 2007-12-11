def generate(env, **kw):
    env.Tool('addLibrary', library = ['g25Response','g2c'], package = 'irfs/g25Response')
    env.Tool('irfInterfaceLib')
    env.Tool('irfUtilLib')
    env.Tool('st_facilitiesLib')
    env.Tool('astroLib')
    env.Tool('addLibrary', library = env['clhepLibs'])

def exists(env):
    return 1

def generate(env, **kw):
    env.Tool('addLibrary', library = ['testResponse', 'g2c'], package = 'irfs/testResponse')
    env.Tool('irfInterfaceLib')
    env.Tool('irfUtilLib')
    env.Tool('astroLib')
    env.Tool('tipLib')
    env.Tool('st_facilitiesLib')
    env.Tool('addLibrary', library = env['clhepLibs'])

def exists(env):
    return 1;

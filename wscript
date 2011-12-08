APPNAME = 'sydmine'
VERSION = '0.0.1'

top = '.'
out = 'build'

def options(opt):
    opt.tool_options('compiler_cxx')

def configure(conf):
    conf.check_tool('compiler_cxx')
    conf.sub_config('src')

def build(bld):
    bld.add_subdirs('src')

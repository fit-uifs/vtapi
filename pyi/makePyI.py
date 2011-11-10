
# See
# https://gitorious.org/vtapi/pages/Install 
# http://www.gccxml.org
# http://sourceforge.net/projects/pygccxml/
# http://www.boost.org/doc/libs/1_47_0/libs/python/doc/


# nm -C -D -g ../dist/libvtapi.so

import os
import subprocess
from pyplusplus import module_builder

cwd = os.getcwd() + os.sep;

# creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [cwd + '../include/vtapi_commons.h', cwd + '../include/vtapi.h']
                                      , gccxml_path=r"gccxml" 
                                      , working_directory= cwd + "../include/"
                                      , include_paths=[cwd + '../include/', cwd + '../include/postgresql']
                                      , define_symbols=[] )


# don't you want to see what is going on?
# mb.print_declarations()

# creating code creator. After this step you should not modify/customize declarations.
mb.build_code_creator( module_name='vtapi' )

# writing code to file.
mb.write_module( cwd + 'vtapi_ext.cpp' )


# build the library
subprocess.call("bjam")

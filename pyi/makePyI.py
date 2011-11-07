"""
See
# https://gitorious.org/vtapi/pages/Install 
# http://www.gccxml.org
# http://sourceforge.net/projects/pygccxml/

$ sudo apt-get install gccxml python-pygccxml
$ svn co 
$ cd pyplusplus && sudo python setup.py install
$ cd .. && python makePyI.py
"""

# nm -C -D -g ../dist/libvtapi.so

import os
from pyplusplus import module_builder

cwd = os.getcwd() + os.sep;

#Creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [cwd + '../include/vtapi_commons.h', cwd + '../include/vtapi.h']
                                      , gccxml_path=r"gccxml" 
                                      , working_directory= cwd + "../include/"
                                      , include_paths=[cwd + '../include/', cwd + '../include/postgresql']
                                      , define_symbols=[] )


#Well, don't you want to see what is going on?
mb.print_declarations()

#Creating code creator. After this step you should not modify/customize declarations.
mb.build_code_creator( module_name='vtapi' )

#Writing code to file.
mb.write_module( cwd + 'vtapi.py' )

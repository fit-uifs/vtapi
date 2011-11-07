"""
See
# https://gitorious.org/vtapi/pages/Install 
# http://www.gccxml.org
# http://sourceforge.net/projects/pygccxml/

$ sudo apt-get install gccxml python-pygccxml
$ svn co https://pygccxml.svn.sourceforge.net/viewvc/pygccxml/tags/pyplusplus_dev_1.0.0/ pyplusplus
$ cd pyplusplus && sudo python setup.py install
$ cd .. && python makaPyI.py
"""
import os
from pyplusplus import module_builder

#Creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [r"/home/chmelarp/Projects/VTApi/vtapi/include/vtapi_commons.h", r"/home/chmelarp/Projects/VTApi/vtapi/include/vtapi.h"]
                                      , gccxml_path=r"gccxml" 
                                      , working_directory=r"/home/chmelarp/Projects/VTApi/vtapi/include"
                                      , include_paths=['/home/chmelarp/Projects/VTApi/vtapi', '/home/chmelarp/Projects/VTApi/vtapi/include', '/home/chmelarp/Projects/VTApi/vtapi/include']
                                      , define_symbols=[] )


#Well, don't you want to see what is going on?
mb.print_declarations()

#Creating code creator. After this step you should not modify/customize declarations.
mb.build_code_creator( module_name='pyplusplus' )

#Writing code to file.
mb.write_module( './vtapi.py' )

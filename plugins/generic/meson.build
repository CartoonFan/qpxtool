libqscan_generic_sources = ['qscan_cmd_generic.cpp','qscan_plugin_generic.cpp']
libqscan_generic = library('libqscan_generic',
                            libqscan_generic_sources,
                            link_with : plugin_linked_libraries,
                            include_directories : [lib_inc,plugin_inc],
                            install : true)

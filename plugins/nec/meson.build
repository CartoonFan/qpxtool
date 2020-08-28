libqscan_nec_sources = ['qscan_cmd_nec.cpp','qscan_plugin_nec.cpp']
libqscan_nec = library('libqscan_nec',
                            libqscan_nec_sources,
                            link_with : plugin_linked_libraries,
                            include_directories : [lib_inc,plugin_inc],
                            install : true)

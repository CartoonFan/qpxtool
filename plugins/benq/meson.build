libqscan_benq_sources = ['qscan_cmd_benq.cpp','qscan_plugin_benq.cpp']
libqscan_benq = library('libqscan_benq',
                            libqscan_benq_sources,
                            link_with : plugin_linked_libraries,
                            include_directories : [lib_inc,plugin_inc],
                            install : true)

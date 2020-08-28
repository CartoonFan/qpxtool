libqscan_liteon_sources = ['qscan_cmd_liteon.cpp','qscan_plugin_liteon.cpp']
libqscan_liteon = library('libqscan_liteon',
                            libqscan_liteon_sources,
                            link_with : plugin_linked_libraries,
                            include_directories : [lib_inc,plugin_inc],
                            install : true)

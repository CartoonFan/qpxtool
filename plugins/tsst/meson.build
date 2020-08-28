libqscan_tsst_sources = ['qscan_cmd_tsst.cpp','qscan_plugin_tsst.cpp']
libqscan_tsst = library('libqscan_tsst',
                            libqscan_tsst_sources,
                            link_with : plugin_linked_libraries,
                            include_directories : [lib_inc,plugin_inc],
                            install : true)

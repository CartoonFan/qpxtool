qpxtransport_sources = ['common_functions.cpp','qpx_mmc.cpp','qpx_mmc_css.cpp','qpx_transport.cpp','sense.cpp','threads.cpp']
qpxtransport = library('qpxtransport',
                            qpxtransport_sources,
                            include_directories : lib_inc,
                            version : '0.7.0',
                            dependencies : thread_dep,
                            install : true)

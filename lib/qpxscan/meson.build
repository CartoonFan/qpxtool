qpxscan_sources = ['qpx_scan.cpp','qpx_scan_algo.cpp','qpx_writer.cpp']
qpxscan = library('qpxscan',
                            qpxscan_sources,
                            link_with : [qpxtransport,qpxplextor],
                            link_args : '-ldl',
                            dependencies: thread_dep,
                            version : '0.7.0',
                            include_directories : lib_inc,
                            install : true)

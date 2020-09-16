qpxtransport_sources = ['common_functions.cpp','qpx_mmc.cpp',
                        'qpx_mmc_css.cpp','qpx_transport.cpp',
                        'sense.cpp','threads.cpp']
qpxtransport = library('qpxtransport',
                            qpxtransport_sources,
                            link_args : '-lstdc++',
                            include_directories : inc,
                            version : '0.7.0',
                            dependencies : thread_dep,
                            install : true)

qpxpioneer_sources = ['pioneer_spdctl.cpp']
qpxpioneer = library('qpxpioneer',
                            qpxpioneer_sources,
                            link_with : qpxtransport,
                            version : '0.7.0',
                            include_directories : inc,
                            install : true)

qpxplextor_sources = ['plextor_features.cpp']
qpxplextor = library('qpxplextor',
                            qpxplextor_sources,
                            link_with : qpxtransport,
                            version : '0.7.0',
                            include_directories : inc,
                            install : true)

qpxscan_sources = ['qpx_scan.cpp','qpx_scan_algo.cpp','qpx_writer.cpp']
qpxscan = library('qpxscan',
                            qpxscan_sources,
                            link_with : [qpxtransport,qpxplextor],
                            link_args : ['-ldl', '-lstdc++'],
                            dependencies: thread_dep,
                            version : '0.7.0',
                            include_directories : inc,
                            install : true)

qpxyamaha_sources = ['yamaha_features.cpp']
qpxyamaha = library('qpxyamaha',
                            qpxyamaha_sources,
                            link_with : qpxtransport,
                            version : '0.7.0',
                            include_directories : inc,
                            install : true)

cdvdcontrol_linked_libraries = [qpxtransport,qpxplextor,
                                qpxyamaha,qpxpioneer]

cdvdcontrol = executable('cdvdcontrol', 'cdvdcontrol.cpp',
                         link_args : '-lstdc++',
                         include_directories : inc,
                         link_with : cdvdcontrol_linked_libraries)

f1tattoo_linked_libraries = [qpxtransport,qpxyamaha]

f1tattoo = executable('f1tattoo', 'f1tattoo.cpp',
                         link_args : '-lstdc++',
                         include_directories: inc,
                         dependencies: png_dep,
                         link_with: f1tattoo_linked_libraries)

pxfw_linked_libraries = [qpxtransport,qpxplextor]

pxfw = executable('pxfw', 'pxfw.cpp',
                  link_args : '-lstdc++',
                  include_directories: inc,
                  link_with: pxfw_linked_libraries)

qscan_linked_libraries = [qpxtransport,qpxscan,qpxplextor]

qscan = executable('qscan', 'qscan.cpp',
                         link_args : '-lstdc++',
                         include_directories: inc,
                         link_with: qscan_linked_libraries)

qscand_linked_libraries = [qpxtransport]
qscand_sources = ['qscand.cpp','child.cpp']

qscand = executable('qscand', qscand_sources,
                    link_args : '-lstdc++',
                    include_directories: inc,
                    dependencies: thread_dep,
                    link_with: qscand_linked_libraries)

readdvd_linked_libraries = [qpxtransport]
readdvd_sources = ['deadreader.cpp','dvd_udf.cpp','imgwriter.cpp','kbhit.cpp',
                   'reader.cpp','reader_disc.cpp','sectmap.cpp']

readdvd = executable('readdvd', readdvd_sources,
                     link_args : '-lstdc++',
                     include_directories: inc,
                     dependencies: thread_dep,
                     link_with: readdvd_linked_libraries)


plugin_linked_libraries = [qpxtransport, qpxscan]

libqscan_asus_sources = ['qscan_cmd_asus.cpp','qscan_plugin_asus.cpp']
libqscan_asus = library('libqscan_asus',
                            libqscan_asus_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_benq_sources = ['qscan_cmd_benq.cpp','qscan_plugin_benq.cpp']
libqscan_benq = library('libqscan_benq',
                            libqscan_benq_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_benq_dvdrom_sources = ['qscan_cmd_benq_dvdrom.cpp','qscan_plugin_benq_dvdrom.cpp']
libqscan_benq_dvdrom = library('libqscan_benq_dvdrom',
                            libqscan_benq_dvdrom_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_generic_sources = ['qscan_cmd_generic.cpp','qscan_plugin_generic.cpp']
libqscan_generic = library('libqscan_generic',
                            libqscan_generic_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_liteon_sources = ['qscan_cmd_liteon.cpp','qscan_plugin_liteon.cpp']
libqscan_liteon = library('libqscan_liteon',
                            libqscan_liteon_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_nec_sources = ['qscan_cmd_nec.cpp','qscan_plugin_nec.cpp']
libqscan_nec = library('libqscan_nec',
                            libqscan_nec_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_pioneer_sources = ['qscan_cmd_pioneer.cpp','qscan_plugin_pioneer.cpp']
libqscan_pioneer = library('libqscan_pioneer',
                            libqscan_pioneer_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_plextor_sources = ['qscan_cmd_plextor.cpp','qscan_plugin_plextor.cpp']
libqscan_plextor = library('libqscan_plextor',
                            libqscan_plextor_sources,
                            link_args : ['-lstdc++','-lm'],
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

libqscan_tsst_sources = ['qscan_cmd_tsst.cpp','qscan_plugin_tsst.cpp']
libqscan_tsst = library('libqscan_tsst',
                            libqscan_tsst_sources,
                            link_args : '-lstdc++',
                            link_with : plugin_linked_libraries,
                            include_directories : inc,
                            install : true)

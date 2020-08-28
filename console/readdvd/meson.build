readdvd_linked_libraries = [qpxtransport]
readdvd_sources = ['deadreader.cpp','dvd_udf.cpp','imgwriter.cpp','kbhit.cpp',
                   'reader.cpp','reader_disc.cpp','sectmap.cpp']

readdvd = executable('readdvd', readdvd_sources, 
                         include_directories: [console_inc,lib_inc],
                         dependencies: thread_dep,
                         link_with: readdvd_linked_libraries)

qscand_linked_libraries = [qpxtransport]
qscand_sources = ['qscand.cpp','child.cpp']

qscand = executable('qscand', qscand_sources, 
                         include_directories: [console_inc,lib_inc],
                         dependencies: thread_dep,
                         link_with: qscand_linked_libraries)

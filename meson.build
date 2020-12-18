project('qpxtool', 'cpp', version: '0.7.2', license: 'GPL',
        default_options: ['optimization=g',
                          'warning_level=3',
                          'cpp_std=c++20',
                          'b_coverage=true'])


inc = include_directories('include')


png_dep = dependency('libpng')
thread_dep = dependency('threads')

run_target('sonarqube_report',
           command: 'scripts/sonarqube_report.sh')


subdir('src')
subdir('src/gui')
subdir('man')
subdir('include')

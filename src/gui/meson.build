qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Network', 'PrintSupport', 'Sql', 'Widgets'])
qt5_moc_headers = ['../../include/about.hpp','../../include/abstractpreview.hpp',
                   '../../include/db_report_selection.hpp',
                   '../../include/device.hpp','../../include/devsettings.hpp',
                   '../../include/devsettings_widgets.hpp',
                   '../../include/errc_detailed.hpp','../../include/graphtab.hpp',
                   '../../include/hostedit_dialog.hpp','../../include/image_label.hpp',
                   '../../include/images_list.hpp','../../include/mainwidget.hpp',
                   '../../include/mainwindow.hpp','../../include/mcapwidget.hpp',
                   '../../include/mwatcher.hpp','../../include/pref_colors.hpp',
                   '../../include/pref_common.hpp','../../include/pref_devices.hpp',
                   '../../include/pref_reports.hpp','../../include/preferences.hpp',
                   '../../include/printpreview.hpp','../../include/progresswidget.hpp',
                   '../../include/qpxgraph.hpp','../../include/qpxiodevice.hpp',
                   '../../include/resultsio.hpp',
                   '../../include/splitbutton.hpp','../../include/tab_devinfo.hpp',
                   '../../include/tab_errc.hpp','../../include/tab_fete.hpp',
                   '../../include/tab_jb.hpp','../../include/tab_mediainfo.hpp',
                   '../../include/tab_ta.hpp','../../include/tab_transfer.hpp',
                   '../../include/tattoowidget.hpp','../../include/testdialog.hpp',
                   '../../include/textslider.hpp']
qt5_sources = ['src/colorlabel.cpp','src/db_connection.cpp','src/main.cpp','src/qpxsettings.cpp']
qt5_moc_sources = ['src/about.cpp','src/abstractpreview.cpp',
                   'src/db_report_selection.cpp','src/device.cpp',
                   'src/devsettings.cpp','src/devsettings_widgets.cpp',
                   'src/errc_detailed.cpp','src/graphtab.cpp',
                   'src/hostedit_dialog.cpp','src/image_label.cpp',
                   'src/images_list.cpp','src/mainwidget.cpp',
                   'src/mainwindow.cpp','src/mcapwidget.cpp',
                   'src/mwatcher.cpp','src/pref_colors.cpp',
                   'src/pref_common.cpp','src/pref_devices.cpp',
                   'src/pref_reports.cpp','src/preferences.cpp',
                   'src/printpreview.cpp','src/progresswidget.cpp',
                   'src/qpxgraph.cpp','src/qpxiodevice.cpp',
                   'src/resultsio.cpp',
                   'src/splitbutton.cpp','src/tab_devinfo.cpp',
                   'src/tab_errc.cpp','src/tab_fete.cpp',
                   'src/tab_jb.cpp','src/tab_mediainfo.cpp',
                   'src/tab_ta.cpp','src/tab_transfer.cpp',
                   'src/tattoowidget.cpp','src/testdialog.cpp',
                   'src/textslider.cpp']
qt5_qresources = ['images/media/cd.png','images/media/cd_r.png',
                  'images/media/cd_rw.png','images/media/dvd-rom.png',
                  'images/media/dvd-ram.png','images/media/dvd-r.png',
                  'images/media/dvd-r_dl.png','images/media/dvd-rw.png',
                  'images/media/dvd+r.png','images/media/dvd+r_dl.png',
                  'images/media/dvd+rw.png','images/media/hddvd-rom.png',
                  'images/media/hddvd-ram.png','images/media/hddvd-r.png',
                  'images/media/hddvd-rw.png','images/media/bd.png',
                  'images/q.png','images/x.png','images/ok.png',
                  'images/add.png','images/edit.png','images/edit-redo.png',
                  'images/edit-undo.png','images/edit-clear.png',
                  'images/logo.png','images/splash.png','images/info.png',
                  'images/pdf.png','images/html.png','images/document.png',
                  'images/save.png','images/directory.png',
                  'images/fileopen.png','images/printer.png',
                  'images/page-setup.png','images/zoom-in.png',
                  'images/zoom-out.png','images/zoom-orig.png',
                  'images/disc.png','images/device.png',
                  'images/cdwriter.png','images/colors.png',
                  'images/loej.png','images/lock.png',
                  'images/test_rt.png','images/test_wt.png',
                  'images/test_errc.png','images/test_jb.png',
                  'images/test_ft.png','images/test_ta.png',
                  'images/settings2.png','images/varirec.png',
                  'images/gigarec.png','images/password.png',
                  'images/sound.png','images/eraser.png',
                  'images/disc-eraser.png','images/tattoo.png',
                  'images/search.png','images/scan.png','images/stop.png',
                  'images/refresh.png','images/refresh-info.png',
                  'images/refresh-media.png','images/settings.png',
                  'images/exit.png','../COPYING','about.html']
moc_files = qt5.preprocess(moc_headers : qt5_moc_headers,
                           moc_sources : qt5_moc_sources,
                           qresources : 'qpxtool.qrc',
                           include_directories : inc,
                           dependencies : qt5_dep)
qpxtool = executable('qpxtool',
           sources : [qt5_sources,moc_files,qt5_moc_sources],
           link_args : ['-lstdc++','-lm'],
           include_directories: inc,
           link_with : [qpxtransport],
           dependencies : [qt5_dep,thread_dep])

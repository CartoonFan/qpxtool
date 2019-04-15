PREFIX    = /c/QPxTool
BINDIR    = /c/QPxTool
SBINDIR   = /c/QPxTool
LIBDIR    = /c/QPxTool
PLUGINDIR = /c/QPxTool/plugins
INCDIR    = /c/QPxTool/include
MANDIR    = /c/QPxTool/man

BINSUFF   = .exe
LIBS_HW   = 
LIBS_INET = -lws2_32
LIBS_DL   = 
LIBS_THREAD = 

LPNG_INC  = 
LPNG_LIB  = 


CXXFLAGS += -Wall -O2  -DOFFT_64BIT -DHAVE_FOPEN64 -DHAVE_FSEEKO   
CFLAGS   += -Wall -O2  -DOFFT_64BIT -DHAVE_FOPEN64 -DHAVE_FSEEKO   

all: cli gui
cli: lib console plugins man

lib:
	$(MAKE) -C lib

console: lib
	$(MAKE) -C console

plugins: lib
	$(MAKE) -C plugins

man:
	$(MAKE) -C man


gui: lib
	$(MAKE) -C gui

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C console clean
	$(MAKE) -C plugins clean
	$(MAKE) -C man clean
	$(MAKE) -C gui clean

install: all cli_install gui_install

cli_install:
	$(MAKE) -C lib install
	$(MAKE) -C console install
	$(MAKE) -C plugins install
	$(MAKE) -C man install

gui_install:
	$(MAKE) -C gui install

uninstall: cli_uninstall gui_uninstall

cli_uninstall:
	$(MAKE) -C lib uninstall
	$(MAKE) -C console uninstall
	$(MAKE) -C plugins uninstall
	$(MAKE) -C man uninstall

gui_uninstall:
	$(MAKE) -C gui uninstall

help:
	@echo ""
	@echo "Alailable targets:"
	@echo "all           - build all"
	@echo "cli           - build libs and console tools"
	@echo "gui           - build gui only"
	@echo "plugins       - build plugins only"
	@echo "install       - install all"
	@echo "cli_install   - install libs and console tools"
	@echo "gui_install   - install gui only"
	@echo ""
	@echo "clean         - clean source tree from object files"
	@echo "uninstall     - uninstall all"
	@echo "cli_uninstall - uninstall libs and console tools"
	@echo "gui_uninstall - uninstall gui only"

.PHONY: all clean cli lib console plugins man nogui gui install cli_install gui_install uninstall cli_uninstall gui_uninstall help

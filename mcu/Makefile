CODE_DIR = .
GIT = git
ECHO = echo

.PHONY: gitversion all clean reset upload do_upload

gitversion: 
	@echo "Get Git-Version"
	$(GIT) log -1 --abbrev=8 --format="format:#define GITHASH 0x%h" > include/vc.h
	$(ECHO) "\n" >> include/vc.h
	$(GIT) log -1 --date=short --format="format:#define GITDATE \"%ad\"" >> include/vc.h
	$(ECHO) "\n" >> include/vc.h
	$(GIT) log -1 --date=short --format="format:#define GITUNIX %ct" >> include/vc.h

all:
#	$(MAKE) gitversion
	$(MAKE) --makefile=Makefile_build -C $(CODE_DIR) all
	
clean:
	$(MAKE) --makefile=Makefile_build -C $(CODE_DIR) clean	

reset:
	$(MAKE) --makefile=Makefile_build -C $(CODE_DIR) reset	
	
upload:
	$(MAKE) --makefile=Makefile_build -C $(CODE_DIR) upload	

do_upload:
	$(MAKE) --makefile=Makefile_build -C $(CODE_DIR) do_upload	

CODE_DIR = mcu

.PHONY: gitversion all clean upload 

gitversion:
	$(MAKE) -C $(CODE_DIR) gitversion
	
all:
	$(MAKE) -C $(CODE_DIR) all
	
clean:
	$(MAKE) -C $(CODE_DIR) clean	

upload:
	$(MAKE) -C $(CODE_DIR) upload	
		
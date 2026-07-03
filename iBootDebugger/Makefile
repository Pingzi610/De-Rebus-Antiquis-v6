all: breakpoint_handlers genDebug	
	@gcc -o genDebug main.c -Itarget -I. -std=c23

breakpoint_handlers:
	@make -C breakpoint_handlers

clean:
	@rm -rf genDebug
	@make clean -C breakpoint_handlers


.PHONY: breakpoint_handlers genDebug

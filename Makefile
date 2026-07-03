all: iBootDebugger payloadGenerator

iBootDebugger:
	@make -C iBootDebugger

payloadGenerator:
	@make -C payloadGenerator


clean:
	@make clean -C iBootDebugger
	@make clean -C payloadGenerator

.PHONY: iBootDebugger payloadGenerator
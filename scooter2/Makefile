BOARD :=	uno
FIRMWARE :=	.pioenvs/$(BOARD)/firmware.bin
SOURCES :=					\
		src/main.cc
LOGFILE :=	serial.log

PIO :=	pio run -e $(BOARD)

$(FIRMWARE): $(SOURCES)
	$(PIO)

PHONY: all
all: $(FIRMWARE)

.PHONY: upload
upload: $(FIRMWARE)
	$(PIO) -t upload

.PHONY: monitor
monitor:
	$(PIO) -t monitor | tee $(LOGFILE)

.PHONY: deploy
deploy: $(FIRMWARE)
	$(PIO) -t upload && sleep 0.5 && $(PIO) -t monitor

.PHONY: clean
clean:
	$(PIO) -t clean

.PHONY: cloc
cloc:
	cloc include lib src

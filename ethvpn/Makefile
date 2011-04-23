
.PHONY: all
all: sim tap
	make -C util/tap


.PHONY: tap
tap:
	make -C util/tap

.PHONY: tap_clean
tap_clean:
	make -C util/tap clean


.PHONY: util
util:
	make -C util/polarssl

.PHONY: util_clean
util_clean:
	make -C util/polarssl clean


.PHONY: sim
sim: util
	make -C hw/sim

.PHONY: sim_clean
sim_clean: util_clean
	make -C hw/sim clean


.PHONY: lpc2148
lpc2148:
	make -C hw/lpc2148

.PHONY: lpc2148_clean
lpc2148_clean:
	make -C hw/lpc2148 clean

.PHONY: test_clean
test_clean:
	make -C test clean

clean: tap_clean sim_clean lpc2148_clean test_clean


# Delta CNC - Make-based build system

ifeq ($(wildcard config.mk),)
$(error config.mk not found. Run ./configure first)
endif

include config.mk

BUILD_DIR ?= build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
LIB_DIR := $(BUILD_DIR)/lib
DOC_DIR := $(BUILD_DIR)/docs

TARGET_OS_UPPER := $(shell echo $(TARGET_OS) | tr a-z A-Z)
CPPFLAGS := -I. -Iutils -Icore/include -DTARGET_OS_$(TARGET_OS_UPPER) $(EXTRA_CPPFLAGS)
CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic $(EXTRA_CFLAGS)
LDFLAGS := $(EXTRA_LDFLAGS)
LDLIBS := -lm -pthread $(EXTRA_LIBS)

ifeq ($(ENABLE_G5),1)
CPPFLAGS += -DENABLE_G5=1
else
CPPFLAGS += -DENABLE_G5=0
endif

ifeq ($(ENABLE_OPCUA),1)
CPPFLAGS += -DENABLE_OPCUA=1
else
CPPFLAGS += -DENABLE_OPCUA=0
endif

ifeq ($(ENABLE_BRACHISTO),1)
CPPFLAGS += -DENABLE_BRACHISTO=1
else
CPPFLAGS += -DENABLE_BRACHISTO=0
endif

ifeq ($(ENDIAN),le)
CPPFLAGS += -DBAGET_ENDIAN_LE
else ifeq ($(ENDIAN),be)
CPPFLAGS += -DBAGET_ENDIAN_BE
endif

COMMON_DIRS := board calib cia402 core drivers ethcat gcode ide kinematics ml motion opcua osal planner sim storage utils vision
COMMON_SRCS := $(shell find $(COMMON_DIRS) -name '*.c')
COMMON_SRCS := $(filter-out main.c ide/ide_app.c sim/simulator.c tests/test_suite.c tests/qemu_selftest.c,$(COMMON_SRCS))
COMMON_OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(COMMON_SRCS))

MAIN_OBJ := $(OBJ_DIR)/main.o
IDE_OBJ := $(OBJ_DIR)/ide/ide_app.o
SIM_OBJ := $(OBJ_DIR)/sim/simulator.o
TEST_OBJ := $(OBJ_DIR)/tests/test_suite.o
QEMU_OBJ := $(OBJ_DIR)/tests/qemu_selftest.o

LIBCNC := $(LIB_DIR)/libcnc.a
FIRMWARE := $(BIN_DIR)/cnc_firmware
IDE_BIN := $(BIN_DIR)/cnc_ide
SIM_BIN := $(BIN_DIR)/cnc_simulator
TEST_BIN := $(BIN_DIR)/tests_host
QEMU_BIN := $(BIN_DIR)/qemu_selftest

.PHONY: all firmware ide simulator tests docs docs_api clean distclean install uninstall check qemu-selftest

all: firmware ide simulator

firmware: $(FIRMWARE)

ide: $(IDE_BIN)

simulator: $(SIM_BIN)

tests: $(TEST_BIN)

check: $(TEST_BIN)
	$(TEST_BIN)

qemu-selftest:
	PROJECT_SOURCE_DIR=$(PWD) tests/run_qemu_selftest.sh

$(FIRMWARE): $(LIBCNC) $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $(MAIN_OBJ) $(LIBCNC) $(LDLIBS) -o $@

$(IDE_BIN): $(LIBCNC) $(IDE_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $(IDE_OBJ) $(LIBCNC) $(LDLIBS) -o $@

$(SIM_BIN): $(LIBCNC) $(SIM_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $(SIM_OBJ) $(LIBCNC) $(LDLIBS) -o $@

$(TEST_BIN): $(LIBCNC) $(TEST_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $(TEST_OBJ) $(LIBCNC) $(LDLIBS) -o $@

$(QEMU_BIN): $(QEMU_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $(QEMU_OBJ) $(LDLIBS) -o $@

$(LIBCNC): $(COMMON_OBJS)
	@mkdir -p $(LIB_DIR)
	$(AR) rcs $@ $(COMMON_OBJS)
	$(RANLIB) $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/tests/test_suite.o: CPPFLAGS += -DTEST_DATA_DIR=\"tests/data\"
$(OBJ_DIR)/tests/qemu_selftest.o: CPPFLAGS += -DTEST_DATA_DIR=\"tests/data\"

ifeq ($(BUILD_DOCS),1)
.PHONY: docs docs_api docs_html docs_pdf

docs: docs_api docs_html docs_pdf

docs_api:
	@mkdir -p $(DOC_DIR)/api
	DOXYGEN_OUTPUT_DIR=$(DOC_DIR)/api doxygen docs/Doxyfile

docs_html: docs_api
	@mkdir -p $(DOC_DIR)/html
	SPHINX_DOXYGEN_HTML=$(DOC_DIR)/api/html sphinx-build -b html docs/source $(DOC_DIR)/html

docs_pdf: docs_api
	@mkdir -p $(DOC_DIR)/latex
	SPHINX_DOXYGEN_HTML=$(DOC_DIR)/api/html sphinx-build -b latex docs/source $(DOC_DIR)/latex
	$(MAKE) -C $(DOC_DIR)/latex all-pdf
	@mkdir -p $(DOC_DIR)/pdf
	if ls $(DOC_DIR)/latex/*.pdf >/dev/null 2>&1; then \
	    cp $(DOC_DIR)/latex/*.pdf $(DOC_DIR)/pdf/; \
	else \
	    echo "LaTeX PDF output not generated; check TeX dependencies" >&2; \
	fi
else
.PHONY: docs docs_api docs_html docs_pdf

docs docs_api docs_html docs_pdf:
	@echo "Documentation build disabled (BUILD_DOCS=0)"
endif

clean:
	rm -rf $(BUILD_DIR)

DISTCLEAN_FILES := config.mk

distclean: clean
	rm -f $(DISTCLEAN_FILES)

HEADERS := $(shell find $(COMMON_DIRS) -name '*.h')

install: all
	install -d $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/share/delta-cnc $(DESTDIR)$(PREFIX)/include/delta_cnc
	install -m 755 $(FIRMWARE) $(DESTDIR)$(PREFIX)/bin/
	install -m 755 $(IDE_BIN) $(DESTDIR)$(PREFIX)/bin/
	install -m 755 $(SIM_BIN) $(DESTDIR)$(PREFIX)/bin/
	install -m 644 $(LIBCNC) $(DESTDIR)$(PREFIX)/lib/
	for header in $(HEADERS); do \
	    install -D -m 644 $$header $(DESTDIR)$(PREFIX)/include/delta_cnc/$$header; \
	done
	if [ -d $(DOC_DIR)/html ]; then \
	    cp -R $(DOC_DIR)/html $(DESTDIR)$(PREFIX)/share/delta-cnc/html; \
	fi
	if [ -d $(DOC_DIR)/pdf ]; then \
	    cp -R $(DOC_DIR)/pdf $(DESTDIR)$(PREFIX)/share/delta-cnc/pdf; \
	fi

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/cnc_firmware
	rm -f $(DESTDIR)$(PREFIX)/bin/cnc_ide
	rm -f $(DESTDIR)$(PREFIX)/bin/cnc_simulator
	rm -f $(DESTDIR)$(PREFIX)/lib/libcnc.a
	rm -rf $(DESTDIR)$(PREFIX)/include/delta_cnc
	rm -rf $(DESTDIR)$(PREFIX)/share/delta-cnc

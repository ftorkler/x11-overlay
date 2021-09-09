SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TEST_SRC_DIR := test

EXE := $(BIN_DIR)/overlay
SRC := $(filter-out $(SRC_DIR)/overlay.cpp, $(wildcard $(SRC_DIR)/*.cpp))
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_EXE := $(BIN_DIR)/run_tests
TEST_SRC := $(filter-out $(TEST_SRC_DIR)/test.cpp, $(wildcard $(TEST_SRC_DIR)/*.cpp))
TEST_OBJ := $(TEST_SRC:$(TEST_SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -Wall `pkg-config --cflags freetype2`
LDFLAGS  :=
LDLIBS   := -lX11 -lXext -lXfixes -lXrandr -lXft -lfreetype

.PHONY: all clean

all: overlay test

overlay: $(EXE)
test: $(TEST_EXE)

debug: CPPFLAGS += -DDEBUG -g
debug: $(EXE)

$(EXE): $(OBJ_DIR)/overlay.o $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_EXE): $(OBJ_DIR)/test.o ${TEST_OBJ} ${OBJ} | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) -fno-access-control -I3rdparty/acutest/include -DTEST_NO_MAIN $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test.o: $(TEST_SRC_DIR)/test.cpp | $(OBJ_DIR)
	$(CXX) -fno-access-control -I3rdparty/acutest/include $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

clean:
	@$(RM) -r $(BIN_DIR) $(OBJ_DIR)
	@echo "clean... OK"

-include $(OBJ:.o=.d)

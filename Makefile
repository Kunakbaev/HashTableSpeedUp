CC			 := g++
# WARNING: maybe try adding avx2 flag only to linked list file
CFLAGS		 := -g -O2
#CFLAGS 		 := -g -D _DEBUG -lm -ggdb3 -std=c++17 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla
#CFLAGS 		 += -fsanitize=address

MY_LOG_LIB_NAME    				:= my_loglib

LOGGER_EXT_LIB_DIR 				:= ./external/LoggerLib
LIB_RUN_NAME       				:= hashTableSpeedUp
BUILD_DIR_PATH					:= building
BUILD_DIR_TARGET_NAME          	:= createBuildDir
LINKED_LIST_DIR					:= LinkedList
HASH_TABLE_DIR					:= HashTable
SRC_DIR							:= ./

SRC 							:=  $(SRC_DIR)/main.cpp 			 				\
									$(LINKED_LIST_DIR)/linkedList.cpp				\
									$(LINKED_LIST_DIR)/common.cpp					\
									$(LINKED_LIST_DIR)/linkedListShortKeys.cpp		\
									$(LINKED_LIST_DIR)/errorsHandlerLinkedList.cpp	\
									$(HASH_TABLE_DIR)/hashTable.cpp					\
									$(HASH_TABLE_DIR)/errorsHandlerHashTable.cpp	\

OBJ								:= $(patsubst %.cpp, $(BUILD_DIR_PATH)/%.o, $(notdir ${SRC})) 

CFLAGS += -I $(LOGGER_EXT_LIB_DIR)/include

ifeq ($(DEBUG), 0)
	ASSERT_DEFINE = -DNDEBUG
endif

.PHONY: $(LIB_RUN_NAME) run $(BUILD_DIR) clean

# GET RID OF SANITIZER DEADLY SIGNAL:		sudo sysctl vm.mmap_rnd_bits=28

# -------------------------   HELPER TARGETS   ---------------------------

compile: $(OBJ)
	@$(CC) $^ -o $(BUILD_DIR_PATH)/${LIB_RUN_NAME} $(CFLAGS) -l$(MY_LOG_LIB_NAME) $(CFLAGS)

$(BUILD_DIR_PATH)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR_TARGET_NAME)
	@$(CC) -c $< -o $@

$(BUILD_DIR_PATH)/%.o: $(LINKED_LIST_DIR)/%.cpp | $(BUILD_DIR_TARGET_NAME)
	@$(CC) -c $< -o $@

$(BUILD_DIR_PATH)/%.o: $(HASH_TABLE_DIR)/%.cpp | $(BUILD_DIR_TARGET_NAME)
	@$(CC) -c $< -o $@

run:
	@$(BUILD_DIR_PATH)/$(LIB_RUN_NAME)

testPerfomance: clean compile
	hyperfine building/hashTableSpeedUp --warmup 5 -r 10

runProfiling: clean compile
	perf record building/hashTableSpeedUp
	perf report

# WARNING: cleans build dir everytime
compileAndRun: clean compile run

$(BUILD_DIR_TARGET_NAME):
	@mkdir -p $(BUILD_DIR_PATH)
clean:
	rm -rf $(BUILD_DIR_PATH)/*.o $(BUILD_DIR_PATH)/$(LIB_RUN_NAME)

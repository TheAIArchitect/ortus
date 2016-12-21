BASE_FILES = ExeTimer.cpp FileUtils.cpp GeneralUtils.cpp Logger.cpp NumUtils.cpp \
			 Parallel.cpp StrUtils.cpp

CL_FILES = ExeTimer.cpp FileUtils.cpp GeneralUtils.cpp Logger.cpp NumUtils.cpp \
		   Parallel.cpp StrUtils.cpp CLHelperClass.cpp

DB_FILES = ExeTimer.cpp FileUtils.cpp GeneralUtils.cpp Logger.cpp NumUtils.cpp \
		   Parallel.cpp StrUtils.cpp dbHelper.cpp

ALL_FILES = ExeTimer.cpp FileUtils.cpp GeneralUtils.cpp Logger.cpp NumUtils.cpp \
		    Parallel.cpp StrUtils.cpp dbHelper.cpp CLHelperClass.cpp

CC := clang++
DEBUG := -DLOG_DEBUG -g
OPT := -O2
CFLAGS := -std=c++11 -c
SRC := $(wildcard *.cpp)
OBJ :=$(addprefix obj/,$(notdir $(SRC:.cpp=.o)))
LIB := libutil.a
LIB_DIR := lib/
LIB_CMD := ar rvs
RM = -@\rm -f

.PHONY: all
all: $(LIB)

$(LIB): $(OBJ)
	$(LIB_CMD) $@ $^
	mv $(LIB) $(LIB_DIR)

obj/%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $< $(OPT)

clean:
	$(RM) obj/*.o
	$(RM) $(LIB_DIR)$(LIB)

#debug_base: $(BASE_FILES)
#	clang++ -std=c++11 -c $(BASE_FILES) -DLOG_DEBUG -DNOBUF

#debug_cl: $(CL_FILES)
#	clang++ -std=c++11 -c $(CL_FILES)  -DLOG_DEBUG

#debug_db: $(DB_FILES)
#	clang++ -std=c++11 -c $(DB_FILES) -DLOG_DEBUG

#debug_all: $(ALL_FILES)
#	clang++ -std=c++11 -c $(ALL_FILES) -DLOG_DEBUG

#release_base: $(BASE_FILES)
#	clang++ -std=c++11 -c $(BASE_FILES) -O2

#release_base_nobuffer: $(BASE_FILES)
#	clang++ -std=c++11 -c $(BASE_FILES) -DNOBUF -O2

#release_cl: $(CL_FILES)
#	clang++ -std=c++11 -c $(CL_FILES) -O2

#release_db: $(DB_FILES)
#	clang++ -std=c++11 -c $(DB_FILES) -O2

#release_all: $(ALL_FILES)
#	clang++ -std=c++11 -c $(ALL_FILES) -O2

#optimized_base_debug: $(BASE_FILES)
#	clang++ -std=c++11 -c $(BASE_FILES) -DLOG_DEBUG -O2

#lib: *.o
#	ar rvs libutil.a *.o
#	mv libutil.a lib/

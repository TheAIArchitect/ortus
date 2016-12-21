CC := clang++
CFLAGS := -std=c++11 -pthread 
INCLUDES := -I../bullet-build/ -I../spglib/include
LFLAGS := -L/opt/intel/opencl-1.2-5.0.0.43/lib64\
../spglib/lib/libutil.a \
../bullet-build/BulletDynamics/libBulletDynamics.a \
../bullet-build/BulletCollision/libBulletCollision.a \
../bullet-build/LinearMath/libLinearMath.a

LIBS := -lGL -lGLU -lglut -lOpenCL -lsqlite3
SRC := $(wildcard *.cpp)
OBJ := $(addprefix obj/,$(notdir $(SRC:.cpp=.o))) 
EXC := worm
RM := -@\rm -f
SPGLIB := cd ../spglib && ./external_make.sh

$(EXC): $(OBJ)
	$(SPGLIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS) $(LIBS)

obj/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES)  -c -o $@ $<

.PHONY: clean re

clean:
	$(RM) obj/*.o
	$(RM) $(EXC)

ex:
	$(RM) $(EXC)
    
run:
	./$(EXC)

#all: 
#	clang++ -pthread -std=c++11 -L/opt/intel/opencl-1.2-5.0.0.43/lib64 -o worm *.cpp \
	-lGL -lGLU -lglut -lOpenCL -lsqlite3 -I ../bullet-build/ -I ../spglib/include \
	../spglib/lib/libutil.a ../bullet-build/BulletDynamics/libBulletDynamics.a \
	../bullet-build/BulletCollision/libBulletCollision.a ../bullet-build/LinearMath/libLinearMath.a



#clean:
#	-@\rm -f worm 2>/dev/null || true




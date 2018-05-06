CC = g++
AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: dirs $(LIB_DIR)/libCTagMarks.a

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../lib ]; then mkdir ../lib; fi

SRC = \
CTagMarks.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++14 \
-I$(INC_DIR) \
-I../../CBookmarks/include \
-I../../CHtml/include \
-I../../CXML/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I../../COS/include \
-I.

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCTagMarks.a

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(LIB_DIR)/libCTagMarks.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCTagMarks.a $(OBJS)

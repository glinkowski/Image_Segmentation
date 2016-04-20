OBJS1 = test.cpp
OBJS2 = segment.h segment.cpp
OBJS3 = segment.h segThresh.cpp

EXE1 = test.exe
EXE2 = segment.exe
EXE3 = segThresh.exe

CC = g++

FLAGS = -w -lSDL2 -o

#OCV1 = `pkg-config --cflags opencv' 'pkg-config --libs opencv`

OCVFLAGS = -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib/ -g -o
OCVLIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_stitching

segment: $(OBJS2)
	$(CC) $(OCVFLAGS) $(EXE2) $(OBJS2) $(OCVLIBS)
#	$(CC) $(OCV) $(OBJS) $(FLAGS) $(EXE)

thresh: $(OBJS3)
	$(CC) $(OCVFLAGS) $(EXE3) $(OBJS3) $(OCVLIBS)

basic: $(OBJS1)
	$(CC) $(OCVFLAGS) $(EXE1) $(OBJS1) $(OCVLIBS)

# TODO: fix 'all'
all:  $(OBJS2)
	$(CC) $(OCVFLAGS) $(EXE2) $(OBJS2) $(OCVLIBS)
	  $(OBJS1)
	$(CC) $(OCVFLAGS) $(EXE1) $(OBJS1) $(OCVLIBS)

# TODO:
#clean: 
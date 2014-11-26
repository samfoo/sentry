INCLUDE = -I/usr/local/include/libfreenect
LIBS = -L/usr/local/lib
GLUTLIBS = -framework OpenGL -framework GLUT
KINECTLIBS = -lfreenect
CVLIBS = -lopencv_core -lopencv_features2d -lopencv_highgui -lopencv_imgproc -lopencv_nonfree -lopencv_objdetect -lopencv_flann -lopencv_calib3d -lopencv_video

HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

%.o: %.c $(HEADERS)
	$(CXX) $(INCLUDE) $(CFLAGS) -c $< -o $@ -Wall

cv: $(OBJECTS)
	$(CXX) $(OBJECTS) -Wall -o cv $(LIBS) $(CVLIBS) $(KINECTLIBS)

clean:
	rm -f *.o
	rm -rf cv

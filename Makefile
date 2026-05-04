CC := gcc

# 交叉编译
AARCH64 := aarch64-linux-gnu-gcc
INCLUDE_FILE := -I/usr/arm64-libjpeg/include -I/usr/arm64-ffmpeg/include


LIBS := -L/usr/arm64-libjpeg/lib \
		-L/usr/arm64-ffmpeg/lib \
		-ljpeg \
		-lswscale \
		-lavformat \
		-lavcodec \
		-lavutil \
		-lswresample \
		-pthread \
		-lm 

# video_frame: video_frame.c
# 	$(CC) -o video_frame video_frame.c

# clean:
# 	rm -f video_frame

main: main.c
	$(AARCH64) -o main main.c v4l2_app.c $(INCLUDE_FILE) $(LIBS)

clean:
	rm -f main



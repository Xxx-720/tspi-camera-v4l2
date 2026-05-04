#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
// V4L2的头文件
#include <linux/videodev2.h>


void v4l2_start();

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
// V4L2的头文件
#include <linux/videodev2.h>

#include "v4l2_app.h"

int main(int argc, char const *argv[])
{
    v4l2_start();
    
    return 0;
}


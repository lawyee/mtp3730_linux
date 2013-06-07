/*
 * saV4L2Rotation.c
 *
 * This is a V4L2 sample application to show the display functionality
 * The app puts a moving horizontal bar on the display device in various
 * shades of colors with 90 and 180 degree rotation.
 * This application is an extenson of saMmapDisplay application and it demos the
 * rotation on video pipelines using VRFB.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Author: Vaibhav Hiremath <hvaibhav@ti.com>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Header Files
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/videodev.h>
#include <linux/videodev2.h>

/*
 * Macros
 */
#define MAX_BUFFER	3
#define MAXLOOPCOUNT	300
#define DEGREE		90
#define DEGREE1		180

#ifdef CONFIG_OMAP3530
static char display_dev_name[30] = { "/dev/video7"};
#else
static char display_dev_name[30] = { "/dev/video1"};
#endif
static int display_fd = 0;
static unsigned int loop_count = MAXLOOPCOUNT;

struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

static struct buf_info display_buff_info[MAX_BUFFER];
static int numbuffers = MAX_BUFFER;
static int dispheight, dispwidth, sizeimage;
struct v4l2_buffer buf;

/*
 * Release display: unmap and close the device
*/
static int releaseDisplay(void)
{
	int i;
	for (i = 0; i < numbuffers; i++) {
		munmap(display_buff_info[i].start,
				display_buff_info[i].length);
		display_buff_info[i].start = NULL;
	}
	close(display_fd);
	display_fd = 0;
	return 0;
}
/*
 * Starts Streaming
 */
static void startDisplay(void)
{
	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT, ret;
	ret = ioctl(display_fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		close(display_fd);
		exit(1);
	}
}

/*
 * Stops Streaming
 */
static void stopDisplay(void)
{
	int ret, a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_STREAMOFF, &a);
	if(ret < 0) {
		perror("Error in stopping display\n");
		close(display_fd);
		exit(1);
	}
}

static short ycbcr[8] = {
	(0x1F << 11) | (0x3F << 5) | (0x1F),
	(0x00 << 11) | (0x00 << 5) | (0x00),
	(0x1F << 11) | (0x00 << 5) | (0x00),
	(0x00 << 11) | (0x3F << 5) | (0x00),
	(0x00 << 11) | (0x00 << 5) | (0x1F),
	(0x1F << 11) | (0x3F << 5) | (0x00),
	(0x1F << 11) | (0x00 << 5) | (0x1F),
	(0x00 << 11) | (0x3F << 5) | (0x1F),
};

static void color_bar(char *addr, int width, int height, int order)
{
	unsigned short *ptr = (unsigned short *)addr + order*width;
	int i, j, k;

	for(i = 0 ; i < 8 ; i ++) {
		for(j = 0 ; j < height / 8 ; j ++) {
			for(k = 0 ; k < width / 2 ; k ++, ptr++)
				*ptr = ycbcr[i];
			if((unsigned int)ptr >= (unsigned int)addr +
					width*height)
				ptr = (unsigned short *)addr;
		}
	}
}

static void set_rotation(int degree)
{
	struct v4l2_control control;
	int ret = 0;

	control.id = V4L2_CID_ROTATE;
	control.value = degree;

	ret = ioctl(display_fd, VIDIOC_S_CTRL, &control);
	if (ret < 0) {
		perror("VIDIOC_S_CTRL\n");
		close(display_fd);
		exit(0);
	}
}

static void set_format(int degree)
{

	struct v4l2_format fmt;
	int ret = 0;

	/* Get the parameters before setting and
	 * set only required parameters */
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_G_FMT, &fmt);
	if(ret<0) {
		perror("Get Format failed\n");
		exit(1);
	}
	/* Set the image size to VGA and pixel format to RGB565 */
	if(degree == 0 || degree == 180) {
		fmt.fmt.pix.width = WIDTH;
		fmt.fmt.pix.height = HEIGHT;
	} else {
		fmt.fmt.pix.width = HEIGHT;
		fmt.fmt.pix.height = WIDTH;
	}
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	ret = ioctl(display_fd, VIDIOC_S_FMT, &fmt);
	if(ret<0) {
		perror("Set Format failed\n");
		exit(1);
	}
}

static void get_format(int *dispheight, int *dispwidth, int *sizeimage)
{
	struct v4l2_format fmt;
	int ret = 0;
	/* It is necessary for applications to know about the
	 * buffer chacteristics that are set by the driver for
	 * proper handling of buffers These are : width,height,
	 * pitch and image size
	 * */
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(display_fd, VIDIOC_G_FMT, &fmt);
	if(ret<0){
		perror("Get Format failed\n");
		close(display_fd);
		exit(1);
	}
	*dispheight = fmt.fmt.pix.height;
	*dispwidth = fmt.fmt.pix.bytesperline;
	*sizeimage = fmt.fmt.pix.sizeimage;

}

static void setup_buffers(void)
{
	struct v4l2_requestbuffers reqbuf;
	int ret = 0, i;
	/* Now for the buffers.Request the number of buffers needed
	 * and the kind of buffers(User buffers or kernel buffers
	 * for memory mapping). Please note that the return value
	 * in the reqbuf.count might be lesser than numbuffers under
	 * some low memory circumstances */
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.count = numbuffers;
	reqbuf.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(display_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("Could not allocate the buffers\n");
		close(display_fd);
		exit(1);
	}

	numbuffers = reqbuf.count;
	for(i = 0 ; i < reqbuf.count ; i ++) {
		/* query */
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(display_fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("quering for buffer info failed\n");
			close(display_fd);
			exit(1);
		}
		/* mmap */
		display_buff_info[i].length = buf.length;
		display_buff_info[i].index = i;
		display_buff_info[i].start =
			mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
					MAP_SHARED, display_fd, buf.m.offset);

		if ((unsigned int) display_buff_info[i].
				start == MAP_SHARED) {
			printf("Cannot mmap = %d buffer\n", i);
			close(display_fd);
			exit(1);
		}
		/*
		   After mapping each buffer, it is a good
		   idea to first "zero" them out.
		   Here it is being set to a mid grey-scale
		   Y=0x80, Cb=0x80, Cr=0x80
		 */
		memset(display_buff_info[i].start, 0x80, buf.length);

		/* Fill up the buffers with the values.*/
		color_bar(display_buff_info[i].start, dispwidth, dispheight,
				0);
	}

	for(i = 0 ; i < reqbuf.count ; i ++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		ret = ioctl(display_fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			close(display_fd);
			exit(1);
		}
	}
}

static void open_display(void)
{
	struct v4l2_capability capability;
	int mode = O_RDWR;
	display_fd = open((const char *)display_dev_name, mode);
	if(display_fd == -1) {
		perror("failed to open display device\n");
		exit(1);
	}
	if (ioctl(display_fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		exit(1);
	}

}

static int app_main(void)
{

	void *displaybuffer;
	int counter = 0;
	int ret = 0;

	/* Setting parameters for 90 degree rotation */
	/* open display channel */
	open_display();

	/* Set the rotation angle */
	set_rotation(DEGREE);

	/* set the format according to the rotation value selected */
	set_format(DEGREE);

	/* Get the negotiatied format */
	get_format(&dispheight, &dispwidth, &sizeimage);

	/*
	  1. Request the buffers
	   2. Query for the buffer info like the phys address
	   3. mmap the buffer to user space.
	   This information anout the buffers is currently stored in a user level
	   data structue
	 */
	setup_buffers();

	printf("Image rotated by 90 degree\n");
	/* Start Displaying */
	startDisplay();

	/*
	   This is a running loop where the buffer is
	   DEQUEUED  <-----|  PROCESSED	|  & QUEUED -------|
	 */
	while(counter < loop_count) {
		/* Get display buffer using DQBUF ioctl */
		ret = ioctl(display_fd, VIDIOC_DQBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_DQBUF\n");
			return -1;
		}

		displaybuffer = display_buff_info[buf.index].start;
		/* Process it
		   In this example, the "processing" is putting a horizontally
		   moving color bars with changing starting line of display.
		 */
		color_bar(displaybuffer, dispwidth, dispheight,
				counter%(dispheight/2));

		/* Now queue it back to display it */
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = buf.index;
		ret = ioctl(display_fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			return -1;
		}

		counter++;
	}

	/*
	   Once the streaming is done  stop the display  hardware  */
	stopDisplay();
	/* close  display channel */
	releaseDisplay();

	/* open display channel */
	open_display();

	/* Set the rotation angle */
	set_rotation(DEGREE1);

	/* set the format according to the rotation value selected */
	set_format(DEGREE1);

	/* Get the negotiatied format */
	get_format(&dispheight, &dispwidth, &sizeimage);

	/*
	  1. Request the buffers
	   2. Query for the buffer info like the phys address
	   3. mmap the buffer to user space.
	   This information anout the buffers is currently stored in a user level
	   data structue
	 */
	setup_buffers();

	printf("Image rotated by 180 degree\n");
	/* Start Displaying */
	startDisplay();

	/*
	   This is a running loop where the buffer is
	   DEQUEUED  <-----|  PROCESSED	|  & QUEUED -------|
	 */
	counter = 0;
	while(counter < loop_count) {
		/* Get display buffer using DQBUF ioctl */
		ret = ioctl(display_fd, VIDIOC_DQBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_DQBUF\n");
			return -1;
		}

		displaybuffer = display_buff_info[buf.index].start;
		/* Process it
		   In this example, the "processing" is putting a horizontally
		   moving color bars with changing starting line of display.
		 */
		color_bar(displaybuffer, dispwidth, dispheight,
				counter%(dispheight/2));

		/* Now queue it back to display it */
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = buf.index;
		ret = ioctl(display_fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			return -1;
		}

		counter++;
	}

	/*
	   Once the streaming is done  stop the display  hardware  */
	stopDisplay();
	/* close  display channel */
	releaseDisplay();

	/* open display channel */
        open_display();

        /* Set the rotation to -1 angle */
        set_rotation(0);

	close(display_fd);
	return 0;

}

static void usage(void)
{
	printf("Usage:saV4L2Rotation [-d <device node>][-l <loop coult>][-h help]");
	printf("\t[-d <device node>]	: Device node as a string," \
			" default to /dev/video7\n" \
		"[-l <loop count>]	: Loop Count, default to 500\n" \
		"[-h <help>]		: Help\n");
}
int main(int argc, char *argv[])
{
	char shortoptions[] = "h:d:l:";
	int c, index;

	for (;;) {
		c = getopt_long(argc, argv, shortoptions, (void *) NULL,
				&index);
		if (-1 == c)
			break;
		switch (c) {
		case 0:
			break;
		case 'd':
		case 'D':
			strcpy(display_dev_name, optarg);
			break;
		case 'l':
		case 'L':
			loop_count = atoi(optarg);
			break;
		default:
		case 'h':
		case 'H':
			usage();
			exit(1);
		}
	}
	if(app_main())
		releaseDisplay();
	return 0;
}



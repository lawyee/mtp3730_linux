/*
 * saSrcColorkey.c
 *
 * This application demos the source color keying functionality
 * of the vidoe and graphics plane of the DSS on LCD overlay
 * manager.
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

 /******************************************************************************
  Header File Inclusion
 ******************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <pthread.h>

#include <sys/mman.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <linux/omapfb.h>

/*
 * Macros
 */
#define MAXLOOPCOUNT            500
#define MAX_BUFFER              2
#define NUMBUFFERS		3

#ifdef CONFIG_OMAP3530
static char dev_name[30] = { "/dev/video7"};
#else
static char dev_name[30] = { "/dev/video1"};
#endif
static char display_dev_name[30] = {"/dev/fb0"};
static unsigned int loop_count = MAXLOOPCOUNT;

struct buf_info {
	unsigned int length;
	char *start;
};

/*
 * Color bars
 */
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

static void fill(void *start, unsigned int pixfmt, unsigned int w,
			unsigned int h, int c)
{
	int i, j, k;
	int startcolor = c;

	switch(pixfmt) {
		case V4L2_PIX_FMT_RGB565:
			{
				unsigned short *ptr = (unsigned short *)start;
				unsigned short val;
				for(i = 0 ; i < h ; i ++) {
					startcolor = c;
					for (j = 0 ; j < 8 ; j ++) {
						for( k = 0 ; k < w / 8 ; k ++) {
							val = ycbcr[startcolor];
							*ptr = val;
							ptr ++;
						}
						startcolor ++;
						if(startcolor == 8)
							startcolor = 0;
					}
				}
			}
			break;
	}
}

static void fill_graphics_buffer(unsigned char *addr, int width,
					int height, int pitch)
{

	unsigned short *start = (unsigned short *)addr;
	int i, j, k;

	for(i = 0 ; i < 8 ; i ++){
		for(j = 0 ; j < height / 8 ; j ++){
			for(k = 0 ; k < width; k ++){
				*(start + k) = ycbcr[i];
			}
			start += (pitch/2);
		}
	}

}

/*
 * TODO: Take a valid integer here and write to the SYFS entry
 */
static void set_gbobal_alpha(void)
{
	int ret;

	ret = system("echo 255 > /sys/devices/platform/omapdss/overlay0/global_alpha");
	if (ret)
		printf("Failed to configure Global Alpha\n");

}

/* Thread for graphics  streaming */
static void * graphics_threadfun(void *ptr)
{
	int display_fd;
	struct fb_fix_screeninfo fixinfo;
	struct fb_var_screeninfo var, org_var;
	int buffersize, ret;
	unsigned char *buffer_addr[MAX_BUFFER];
	int i;
	/* Open the display device */

	display_fd = open(display_dev_name, O_RDWR);
	if (display_fd <= 0) {
		perror("Could not open device\n");
		exit (1);
	}

	/* Get fix screen information. Fix screen information gives
	 * fix information like panning step for horizontal and vertical
	 * direction, line length, memory mapped start address and length etc.
	 */
	ret = ioctl(display_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if (ret < 0) {
		perror("Error reading fixed information.\n");
		exit(2);
	}

	/* Get variable screen information. Variable screen information
	 * gives informtion like size of the image, bites per pixel,
	 * virtual size of the image etc. */
	ret = ioctl(display_fd, FBIOGET_VSCREENINFO, &var);
	if (ret < 0) {
		perror("Error reading variable information.\n");
		exit(3);
	}

	memcpy(&org_var, &var, sizeof(var));

	/* Set the resolution virtual and actual */
	var.xres_virtual = var.xres;
	var.yres_virtual = var.yres * MAX_BUFFER ;
	/* Set bits per pixel */
	var.bits_per_pixel=16;

	set_gbobal_alpha();

	if (ioctl(display_fd, FBIOPUT_VSCREENINFO, &var)<0) {
		perror("Error:FBIOPUT_VSCREENINFO\n");
		exit(4);
	}

	/* Get fix screen information. Fix screen information gives
	 * fix information like panning step for horizontal and vertical
	 * direction, line length, memory mapped start address and length etc.
	 */
	ret = ioctl(display_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if (ret < 0) {
		perror("Error reading fixed information.\n");
		exit(2);
	}

	/* Mmap the driver buffers in application space so that application
	 * can write on to them. Driver allocates contiguous memory for
	 * three buffers. These buffers can be displayed one by one. */
	buffersize = fixinfo.line_length * var.yres;
	buffer_addr[0] = (unsigned char *)mmap (0, buffersize*MAX_BUFFER,
			(PROT_READ|PROT_WRITE),
			MAP_SHARED, display_fd, 0) ;
	if ((int)buffer_addr[0] == -1) {
		printf("MMap failed for %d x %d  of %d\n", fixinfo.line_length,
				var.yres*MAX_BUFFER, fixinfo.smem_len);
	}

	/* Store each buffer addresses */
	for(i = 1 ; i < MAX_BUFFER ; i ++) {
		buffer_addr[i] = buffer_addr[i-1] + buffersize;
	}
	/* Fill the buffers with the color bars */
	for(i = 0 ; i < MAX_BUFFER ; i ++)
		/* Fill the buffers with image*/
		fill_graphics_buffer(buffer_addr[i], var.xres,
				var.yres, fixinfo.line_length);

	/* Main loop Foever true*/
	for (i = 0 ; i < MAXLOOPCOUNT ; i ++) {
		/* Get the variable screeninfo */
		ret = ioctl(display_fd, FBIOGET_VSCREENINFO, &var);
		if(ret < 0) {
			perror("Cannot get variable screen info\n");
			munmap(buffer_addr[0], buffersize*MAX_BUFFER);
			exit(0);
		}
		/* Pan the display to the next frame*/
		var.yoffset = i % var.yres;
		ret = ioctl(display_fd, FBIOPAN_DISPLAY, &var);
		if(ret < 0) {
			perror("Cannot pan display\n");
			munmap(buffer_addr[0], buffersize*MAX_BUFFER);
			exit(0);
		}

		ret = ioctl(display_fd, OMAPFB_WAITFORVSYNC, 0);
		if(ret < 0) {
			perror("FBIO_WAITFORVSYNC\n");
			munmap(buffer_addr[0], buffersize*MAX_BUFFER);
			exit(0);
		}
	}

	/* It is better to revert back to original configuration of
	 * the driver */
	if (ioctl(display_fd, FBIOPUT_VSCREENINFO, &org_var)<0) {
		perror("Error:FBIOPUT_VSCREENINFO\n");
		exit(4);
	}
	return 0;
}

/* Thread for video1 streaming */
static void *video1_threadfun(void *ptr)
{
	int mode = O_RDWR, ret, j, i;
	int fd, a, numbuffers = NUMBUFFERS;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct buf_info *buff_info;
	struct v4l2_format fmt;
	struct v4l2_framebuffer framebuffer;
	struct v4l2_capability capability;

	/* Open the video1 device */
	fd = open((const char *)dev_name, mode);
	if (fd <= 0) {
		printf("Cannot open = %s device\n", dev_name);
		exit(0);
	}

	/* Check if the device is capable of streaming */
	if (ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0) {
		perror("VIDIOC_QUERYCAP");
		exit (1);
	}

	/* set the buffer parameters */
	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT\n");
		close(fd);
		exit(0);
	}

	/* Set the image size pixel format and device type*/
	fmt.fmt.pix.width = WIDTH;
	fmt.fmt.pix.height = HEIGHT;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT\n");
		close(fd);
		exit(0);
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT 2\n");
		close(fd);
		exit(0);
	}

	/* Set the window size  and chromakey*/

	fmt.fmt.win.w.left = 0;
	fmt.fmt.win.w.top = 0;
	fmt.fmt.win.w.width = WIDTH;
	fmt.fmt.win.w.height = HEIGHT;
	/* Make red color transperent */
	fmt.fmt.win.chromakey = 63488;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT 2\n");
		close(fd);
		exit(0);
	}

	/* Get the flags related to overlay parameters like alpha blending, color keying. */
	/* Enabled color keying after setting the chroma key as chormakey set is used
	while enabling of the color keying */
	ret = ioctl (fd, VIDIOC_G_FBUF, &framebuffer);
	if (ret < 0) {
		perror ("VIDIOC_G_FBUF");
		exit(1);

	}
	/* Set SRC_COLOR_KEYING if device supports that */
	if(framebuffer.capability & V4L2_FBUF_CAP_SRC_CHROMAKEY) {

		framebuffer.flags |= V4L2_FBUF_FLAG_SRC_CHROMAKEY;
		framebuffer.flags &= ~V4L2_FBUF_FLAG_CHROMAKEY;

		ret = ioctl (fd, VIDIOC_S_FBUF, &framebuffer);
		if (ret < 0) {
			perror ("VIDIOC_S_FBUF");
			exit(1);

		}
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT\n");
		close(fd);
		exit(0);
	}

	/* Request buffers from driver
	 * Buffer can be driver allocated type MMAP
	 * or userptr
	 */
	req.count = numbuffers;
	req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	req.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(fd, VIDIOC_REQBUFS, &req);
	if (ret < 0) {
		perror("cannot allocate memory\n");
		close(fd);
		exit(0);
	}

	buff_info =
		(struct buf_info *) malloc(sizeof(struct buf_info) *
					   req.count);
	if (!buff_info) {
		printf("cannot allocate memory for buff_info\n");
		close(fd);
		exit(0);
	}
	/* Query the buffer to get the physical address of the allocated
	 * buffers. Get the virtual  address of the buffer by
	 * passing the physical address to mmap call of the driver
	 */
	for (i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.index = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYCAP\n");
			for (j = 0; j < i; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			close(fd);
			exit(0);
		}
		buff_info[i].length = buf.length;
		buff_info[i].start =
			mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, buf.m.offset);

		if ((unsigned int) buff_info[i].start ==
				MAP_SHARED) {
			printf("Cannot mmap = %d buffer\n", i);
			for (j = 0; j < i; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			close(fd);
			exit(0);
		}
		memset(buff_info[i].start, 0x80, buff_info[i].length);
		/* Initially fill the buffer */
		fill(buff_info[i].start, fmt.fmt.pix.pixelformat, fmt.fmt.pix.width,
				fmt.fmt.pix.height, 0);
	}
	memset(&buf, 0, sizeof(buf));

	/* Enqueue buffers */
	for (i = 0; i < req.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.index = i;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start,
						buff_info[j].length);
			exit(0);
		}
	}
	/* Start streamon */
	a = 0;
	ret = ioctl(fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		for (i = 0; i < req.count; i++)
			munmap(buff_info[i].start, buff_info[i].length);
		exit(0);
	}

	/* Forever looop for streaming */
	for(i = 0 ; i < MAXLOOPCOUNT ; i ++) {
		ret = ioctl(fd, VIDIOC_DQBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_DQBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start, buff_info[j].length);
			close(fd);
			exit(0);
		}
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_QBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start, buff_info[j].length);
			close(fd);
			exit(0);
		}
	}

	/* Streaming off */
	ret = ioctl(fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF\n");
		for (i = 0; i < req.count; i++)
			munmap(buff_info[i].start, buff_info[i].length);
		close(fd);
		exit(0);
	}
	/* Unmap all the buffers */
	for (i = 0; i < req.count; i++)
		munmap(buff_info[i].start, buff_info[i].length);

	ret = ioctl (fd, VIDIOC_G_FBUF, &framebuffer);
	if (ret < 0) {
		perror ("VIDIOC_G_FBUF");
		exit(1);

	}
	if(framebuffer.capability & V4L2_FBUF_CAP_SRC_CHROMAKEY) {

		framebuffer.flags &= ~V4L2_FBUF_FLAG_SRC_CHROMAKEY;

		ret = ioctl (fd, VIDIOC_S_FBUF, &framebuffer);
		if (ret < 0) {
			perror ("VIDIOC_S_FBUF");
			exit(1);

		}
	}

	close(fd);
	return 0;
}

static void usage(void)
{
	printf("Usage:saSrcColorkey [-d <device node>][-l <loop coult>][-h help]");
	printf("\t[-d <device node>]	: Device node as a string," \
			" default to /dev/video7\n" \
		"[-l <loop count>]	: Loop Count, default to 500\n" \
		"[-h <help>]		: Help\n");
}

int main(int argc, char *argv[])
{
	char shortoptions[] = "h:d:l:";
	int ret1, ret2, c, index;
	pthread_t t1, t2;

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

	printf("Source color keying applied\n");
	printf("Video1 layer is on top of graphics layer\n");
	printf("Red color from the video1 layer is transperent\n");
	printf("Graphics image is seen below video1 where red color is displayed on video1\n");
	/* Create threads for Video1 and Video2 streaming*/
	ret1 = pthread_create(&t1, NULL, video1_threadfun, NULL);
	ret2 = pthread_create(&t2, NULL, graphics_threadfun, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}

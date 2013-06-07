/*
 * saGraphicsARGB.c
 *
 * This is a V4L2 sample application to show ARGB buffer format functionality
 * in Graphics with the alpha blending of the video and graphics plane with different
 * shades of alpha blending.
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
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
 * Header File Inclusion
 */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/omapfb.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <linux/fb.h>

/*
 * Macros
 */
#define MAXLOOPCOUNT		30
#define MAX_FB_BUFFERS		2
#define IMG_WIDTH		400
#define IMG_HEIGHT		272
#define NUMBUFFERS		3
#define OMAP_TI_BMP		"omap_ti.rgb"
#define OMAP_TI_BLANK_BMP	"omap_ti_blank.rgb"

#ifdef CONFIG_OMAP3530
static char vid_dev_name[30] = { "/dev/video7"};
#else
static char vid_dev_name[30] = { "/dev/video1"};
#endif
static char fb_dev_name[30] = {"/dev/fb0"};
static unsigned int loop_count = MAXLOOPCOUNT;
static unsigned int  video1[IMG_WIDTH][IMG_HEIGHT];
static unsigned int  graphics[IMG_WIDTH][IMG_HEIGHT];

struct buf_info {
	unsigned int length;
	char *start;
};

void fill(void *start, unsigned int w, unsigned int h,
		unsigned int output_device)
{
	int i, j;
	static int count = 0, direction = 0;
	static char count1=0;

	if(output_device == 1) {
		if(count >= 250 || (count <= 0 && direction)) {
			direction = ~direction;
		}
		if(!direction)
			count+=25;
		else
			count-=25;
		count1 = 255 - count;
		if(count1 < 25 )
			count1 = 0;

	}

	unsigned int *ptr = (unsigned int *)start;
	ptr += w* (h-1) ;
	for(i = 0 ; i < h ; i ++) {
		for (j = 0 ; j < w ; j ++) {
			if(output_device == 1) {
				if(i < 200)
					*ptr  = ((graphics[i][j] & 0xFFFFFF00)
							| count);
				else
					*ptr = ((graphics[i][j] & 0xFFFFFF00)
							| count1);
			} else if (output_device == 0) {
				*ptr = video1[i][j];
			}
			ptr ++;
		}
		ptr -= 2*w;
	}

}

/* Thread for graphics  streaming */
void * graphics_threadfun(void *ptr)
{
	int display_fd;
	struct fb_fix_screeninfo fixinfo;
	struct fb_var_screeninfo var, org_var;
	int buffersize, ret;
	unsigned char *buffer_addr[MAX_FB_BUFFERS];
	int i;
	unsigned int index;
	/* Open the display device */

	display_fd = open(fb_dev_name, O_RDWR);
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
	var.xres = IMG_WIDTH;
	var.yres = IMG_HEIGHT;
	var.xres_virtual = IMG_WIDTH;
	var.yres_virtual = IMG_HEIGHT * MAX_FB_BUFFERS;

	/* Set bits per pixel */
	var.red.length= 8;
	var.green.length = 8;
	var.blue.length = 8;
	var.transp.length= 8;
	var.red.offset = 24;
	var.green.offset =16;
	var.blue.offset = 8;
	var.transp.offset = 0;
	var.bits_per_pixel = 32;
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
	buffer_addr[0] = (unsigned char *)mmap (0, buffersize * MAX_FB_BUFFERS,
			(PROT_READ|PROT_WRITE),
			MAP_SHARED, display_fd, 0) ;
	if ((int)buffer_addr[0] == -1) {
		printf("MMap failed for %d x %d  of %d\n", fixinfo.line_length,
				var.yres * MAX_FB_BUFFERS, fixinfo.smem_len);
	}

	/* Store each buffer addresses */
	for(i = 1 ; i < MAX_FB_BUFFERS; i ++) {
		buffer_addr[i] = buffer_addr[i-1] + buffersize;
	}

	/* Fill the buffers with image*/
	fill(buffer_addr[0], fixinfo.line_length/4,
			var.yres,1);
	fill(buffer_addr[1], fixinfo.line_length/4,
			var.yres,1);

	/* Main loop Foever true*/
	for (i = 0 ; i < loop_count; i ++) {
		/* Get the variable screeninfo */
		ret = ioctl(display_fd, FBIOGET_VSCREENINFO, &var);
		if(ret < 0) {
			perror("Cannot get variable screen info\n");
			munmap(buffer_addr[0], buffersize * MAX_FB_BUFFERS);
			exit(0);
		}
		/* Pan the display to the next frame*/
		var.yoffset = (i%2) * var.yres;

		ret = ioctl(display_fd, FBIOPAN_DISPLAY, &var);
		if(ret < 0) {
			perror("Cannot pan display\n");
			munmap(buffer_addr[0], buffersize * MAX_FB_BUFFERS);
			exit(0);
		}
		index = (~(i % 2)) & 0x1;
		/* Fill the frame other than what is being displayed */
		fill(buffer_addr[index], fixinfo.line_length/4,
				var.yres,1);

		ret = ioctl(display_fd, OMAPFB_WAITFORVSYNC, 0);
		if(ret < 0) {
			perror("FBIO_WAITFORVSYNC\n");
			munmap(buffer_addr[0], buffersize * MAX_FB_BUFFERS);
			exit(0);
		}
		sleep(1);
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
void *video1_threadfun(void *ptr)
{
	int mode = O_RDWR, ret, j, i;
	int fd, a, numbuffers = NUMBUFFERS;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	struct buf_info *buff_info;
	struct v4l2_format fmt;
	struct v4l2_crop crop;
	struct v4l2_framebuffer framebuffer;

	/* Open the video1 device */
	fd = open((const char *)vid_dev_name, mode);
	if (fd <= 0) {
		printf("Cannot open = %s device\n", vid_dev_name);
		exit(0);
	}

	ret = ioctl (fd, VIDIOC_G_FBUF, &framebuffer);
	if (ret < 0) {
		perror ("VIDIOC_G_FBUF");
		close(fd);
		exit(1);

	}
	if(framebuffer.capability & V4L2_FBUF_CAP_LOCAL_ALPHA) {

		framebuffer.flags |= V4L2_FBUF_FLAG_LOCAL_ALPHA;
		framebuffer.flags &= ~(V4L2_FBUF_FLAG_CHROMAKEY |
				V4L2_FBUF_FLAG_SRC_CHROMAKEY);

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
	/* Set the image size pixel format and device type*/
	fmt.fmt.pix.width = IMG_WIDTH;
	fmt.fmt.pix.height = IMG_HEIGHT;

	/* Video1 does not support ARGB so it takes
	 * RGB32 as unpack RGB24
	 */
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT\n");
		close(fd);
		exit(0);
	}

	crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(fd, VIDIOC_G_CROP, &crop);
	if (ret < 0) {
		perror("VIDIOC_G_CROP\n");
		close(fd);
		exit(0);
	}
	/* Set the Cropping parameters */
	crop.c.left = 0;
	crop.c.top = 0;
	crop.c.width = IMG_WIDTH;
	crop.c.height = IMG_HEIGHT;
	ret = ioctl(fd, VIDIOC_S_CROP, &crop);
	if (ret < 0) {
		perror("VIDIOC_S_CROP\n");
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
	/* Set the window size */
	fmt.fmt.win.w.left = 0;
	fmt.fmt.win.w.top = 0;
	fmt.fmt.win.w.width = IMG_WIDTH;
	fmt.fmt.win.w.height = IMG_HEIGHT;
	ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT 2\n");
		close(fd);
		exit(0);
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
		fill(buff_info[i].start, fmt.fmt.pix.width,
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
	for(i = 0 ; i < loop_count; i ++) {
		ret = ioctl(fd, VIDIOC_DQBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_DQBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start, buff_info[j].length);
			close(fd);
			exit(0);
		}
		/* Filling the buffer without alpha value since video1 does not
		 * support ARGB
		 */
		fill(buff_info[buf.index].start, fmt.fmt.pix.width,
				fmt.fmt.pix.height, 0);
		ret = ioctl(fd, VIDIOC_QBUF, &buf);
		if(ret < 0){
			perror("VIDIOC_QBUF\n");
			for (j = 0; j < req.count; j++)
				munmap(buff_info[j].start, buff_info[j].length);
			close(fd);
			exit(0);
		}
		sleep(1);
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
	if(framebuffer.capability & V4L2_FBUF_CAP_LOCAL_ALPHA) {

		framebuffer.flags &= ~V4L2_FBUF_FLAG_LOCAL_ALPHA;

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
	printf("Usage: saGraphicsARGB [-d <device node>][-l <loop coult>][-h help]");
	printf("\t[-d <device node>]	: Device node as a string," \
			" default to /dev/video7\n" \
		"[-l <loop count>]	: Loop Count, default to 500\n" \
		"[-h <help>]		: Help\n");
}
int main(int argc, char *argv[])
{
	char shortoptions[] = "h:d:l:";
	int ret1, ret2, i, j, c, index;
	char red, green, blue;
	unsigned int val;
	pthread_t t1, t2;
	FILE * file_r;

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
			strcpy(vid_dev_name, optarg);
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

	/* Open the file for the video1 and store the
	 * RGB value
	 */
	file_r = fopen(OMAP_TI_BMP,"r");
	if(file_r == NULL) {
		printf("Unable to open video1 file\n");
		return -1;
	}
	for(i = 0 ; i < IMG_HEIGHT ; i++) {
		for(j = 0 ; j < IMG_WIDTH ; j++) {
			blue = fgetc (file_r);
			green = fgetc (file_r);
			red = fgetc (file_r);
			val = (0 << 24) | (red << 16) | (green << 8) |
				(blue << 0);
			video1[i][j] = val;
		}

	}
	fclose(file_r);

	/* Open the file for graphics and store the RGB
	 * Value
	 */
	file_r = fopen(OMAP_TI_BLANK_BMP, "r");
	if(file_r == NULL){
		printf("Unable to open graphics file\n");
		return -1;
	}
	for(i = 0 ; i < IMG_HEIGHT ; i++) {
		for(j = 0 ; j < IMG_WIDTH ; j++) {
			blue = fgetc (file_r);
			green = fgetc (file_r);
			red = fgetc (file_r);
			val = (red << 24) |(green << 16) | (blue << 8) |
				(0 << 0);
			graphics[i][j] = val;
		}
	}
	fclose(file_r);

	/* Create threads for Video1 and Video2 streaming*/
	ret1 = pthread_create(&t1, NULL, video1_threadfun, NULL);
	ret2 = pthread_create(&t2, NULL, graphics_threadfun, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}

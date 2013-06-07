/*
 * saFbdevDisplayPan.c
 *
 * Panning application on top of Fbdev
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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <linux/fb.h>
#include <linux/omapfb.h>

/*
 * Macros Definations
 */
#define MAX_LOOPCOUNT		1000
#define MAX_FB_BUFFERS		2

/*
 * Default fbdev device node
 */
static char dev_name[30] = {"/dev/fb0"};
static unsigned int loop_count = MAX_LOOPCOUNT;

/*
 * Color bar
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

/* This function is used to fill up buffer with color bars. */
void fill_color_bar(unsigned char *addr, int width, int height)
{
	unsigned short *start = (unsigned short *)addr;
	unsigned int size = width * (height / 8);
	int i, j;

	for(i = 0 ; i < 8 ; i ++) {
		for(j = 0 ; j < size / 2 ; j ++) {
			*start = ycbcr[i];
			start ++;
		}
	}
}

static int timeval_subtract(struct timeval *result, struct timeval *x,
		                     struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 *	nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait, tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

static int app_main(void)
{
	struct fb_fix_screeninfo fixinfo;
	int buffersize, ret, display_fd, i;
	struct timeval before, after, result;
	unsigned char *buffer_addr[MAX_FB_BUFFERS];
	struct fb_var_screeninfo varinfo, org_varinfo;

	/* Open the display device */
	display_fd = open(dev_name, O_RDWR);
	if (display_fd <= 0) {
		perror("Could not open device\n");
		return -1;
	}

	/* Get fix screen information. Fix screen information gives
	 * fix information like panning step for horizontal and vertical
	 * direction, line length, memory mapped start address and length etc.
	 */
	ret = ioctl(display_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if (ret < 0) {
		perror("Error reading fixed information.\n");
		goto exit1;
	}
	printf("\nFix Screen Info:\n");
	printf("----------------\n");
	printf("Line Length - %d\n", fixinfo.line_length);
	printf("Physical Address = %lx\n",fixinfo.smem_start);
	printf("Buffer Length = %d\n",fixinfo.smem_len);

	/* Get variable screen information. Variable screen information
	 * gives informtion like size of the image, bites per pixel,
	 * virtual size of the image etc. */
	ret = ioctl(display_fd, FBIOGET_VSCREENINFO, &varinfo);
	if (ret < 0) {
		perror("Error reading variable information.\n");
		goto exit1;
	}
	printf("\nVar Screen Info:\n");
	printf("----------------\n");
	printf("Xres - %d\n", varinfo.xres);
	printf("Yres - %d\n", varinfo.yres);
	printf("Xres Virtual - %d\n", varinfo.xres_virtual);
	printf("Yres Virtual - %d\n", varinfo.yres_virtual);
	printf("Bits Per Pixel - %d\n", varinfo.bits_per_pixel);
	printf("Pixel Clk - %d\n", varinfo.pixclock);
	printf("Rotation - %d\n", varinfo.rotate);

	memcpy(&org_varinfo, &varinfo, sizeof(varinfo));

	/*
	 * Set the resolution which read before again to prove the
	 * FBIOPUT_VSCREENINFO ioctl, except virtual part which is required for
	 * panning.
	 */
	varinfo.xres_virtual = varinfo.xres;
	varinfo.yres_virtual = varinfo.yres * 2;

	ret = ioctl(display_fd, FBIOPUT_VSCREENINFO, &varinfo);
	if (ret < 0) {
		perror("Error writing variable information.\n");
		goto exit1;
	}

	/* It is better to get fix screen information again. its because
	 * changing variable screen info may also change fix screen info. */
	ret = ioctl(display_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if (ret < 0) {
		perror("Error reading fixed information.\n");
		goto exit2;
	}

	/* Mmap the driver buffers in application space so that application
	 * can write on to them. Driver allocates contiguous memory for
	 * three buffers. These buffers can be displayed one by one. */
	buffersize = fixinfo.line_length * varinfo.yres;
	buffer_addr[0] = (unsigned char *)mmap (0, buffersize * MAX_FB_BUFFERS,
			(PROT_READ|PROT_WRITE), MAP_SHARED, display_fd, 0);

	if ((int)buffer_addr[0] == -1) {
		printf("MMap failed\n");
		ret = -ENOMEM;
		goto exit2;
	}

	/* Store each buffer addresses in the local variable. These buffer
	 * addresses can be used to fill the image. */
	for(i = 1 ; i < MAX_FB_BUFFERS ; i ++) {
		buffer_addr[i] = buffer_addr[i-1] + buffersize;
	}

	/* Fill the buffers with the color bars */
	for(i = 0 ; i < MAX_FB_BUFFERS ; i ++) {
		fill_color_bar(buffer_addr[i], fixinfo.line_length,
				varinfo.yres);
	}

	gettimeofday(&before, NULL);

	/* Panning loop */
	for (i = 0 ; i < loop_count; i ++) {
		/* Get the variable screeninfo just to get the resolution. */
		ret = ioctl(display_fd, FBIOGET_VSCREENINFO, &varinfo);
		if(ret < 0) {
			perror("Cannot get variable screen info\n");
			goto exit3;
		}

		/* Pan the display to the next line. As all the buffers are
		 * filled with the same color bar, moving to next line gives
		 * effect of moving color color bar.
		 * Application should provide y-offset in terms of number of
		 * lines to have panning effect. To entirely change to next
		 * buffer, yoffset needs to be changed to yres field. */
		varinfo.yoffset = i % varinfo.yres;

		/* Change the buffer address */
		ret = ioctl(display_fd, FBIOPAN_DISPLAY, &varinfo);
		if(ret < 0) {
			perror("Cannot pan display\n");
			goto exit3;
		}

		/* Wait for the currect frame buffer to get displayed. */
		ret = ioctl(display_fd, OMAPFB_WAITFORGO, 0);
		if(ret < 0) {
			perror("FBIO_WAITFORVSYNC\n");
			goto exit3;
		}
	}

	gettimeofday(&after, NULL);
	printf("\nThis time for displaying %d frames\n", loop_count);
	printf("Before Time %lu %lu\n",before.tv_sec, before.tv_usec);
	printf("After Time %lu %lu\n",after.tv_sec, after.tv_usec);

	timeval_subtract(&result, &after, &before);
	printf("Result Time:\t%ld %ld\n",result.tv_sec, result.tv_usec);
	printf("Calculated Frame Rate:\t%ld Fps\n\n", loop_count/result.tv_sec);
	ret = 0;

exit3:
	munmap(buffer_addr[0], buffersize * MAX_FB_BUFFERS);
exit2:
	/* It is better to revert back to original configuration */
	ret = ioctl(display_fd, FBIOPUT_VSCREENINFO, &org_varinfo);
	if (ret < 0) {
		perror("Error setting variable information.\n");
	}
exit1:
	close(display_fd);
	return ret;
}

static void usage(void)
{
	printf("Usage: saFbdevDisplayPan [-d <device node>][-l <loop coult>][-h help]");
	printf("\t[-d <device node>]	: Device node as a string," \
			" default to /dev/fb0\n" \
		"[-l <loop count>]	: Loop Count, default to 20\n" \
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
			strcpy(dev_name, optarg);
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

	return app_main();
}

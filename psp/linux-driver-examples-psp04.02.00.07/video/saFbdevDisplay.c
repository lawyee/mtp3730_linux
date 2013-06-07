/*
 * saFbdevDisplay.c
 *
 * This is a Fbdev sample application to show the display functionality
 * The app puts a swapping horizontal bar on the display device in various
 * shades of colors. This application runs RGB565 format with size VGA.
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

#include <linux/fb.h>
#include <linux/omapfb.h>

/*
 * Macros Definations
 */
#define MAX_LOOPCOUNT		20

/*
 * Default fbdev device node
 */
static char dev_name[30] = {"/dev/fb0"};
static unsigned int loop_count = MAX_LOOPCOUNT;

/*
 * Color bars
 */
static short ycbcr[2][8] = {
	{
		(0x1F << 11) | (0x3F << 5) | (0x1F),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x1F),
	}, {
		(0x00 << 11) | (0x3F << 5) | (0x1F),
		(0x1F << 11) | (0x00 << 5) | (0x1F),
		(0x1F << 11) | (0x3F << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x1F),
		(0x00 << 11) | (0x3F << 5) | (0x00),
		(0x1F << 11) | (0x00 << 5) | (0x00),
		(0x00 << 11) | (0x00 << 5) | (0x00),
		(0x1F << 11) | (0x3F << 5) | (0x1F),
	}
};

/*
 * This function is used to fill up buffer with color bars.
 */
void fill_color_bar(unsigned char *addr, int width, int line_len, int height, int index)
{
	unsigned short *start = (unsigned short *)addr;
	int i, j, k;

	if (index) {
		for(i = 0 ; i < 8 ; i ++) {
			for(k=0; k < (height/8); k++) {
				for(j = 0 ; j < width ; j ++) {
					start[j] = ycbcr[1][i];
				}
				start = start + line_len;
			}
		}
	} else {
		for(i = 0 ; i < 8 ; i ++) {
			for(k=0; k < (height/8); k++) {
				for(j = 0 ; j < width ; j ++) {
					start[j] = ycbcr[0][i];
				}
				start = start + line_len;
			}
		}
	}
}

static int app_main(void)
{
	unsigned char *buffer_addr;
	struct fb_fix_screeninfo fixinfo;
	struct fb_var_screeninfo varinfo, org_varinfo;
	int buffersize, ret, display_fd, i;

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
	 * FBIOPUT_VSCREENINFO ioctl.
	 */

	ret = ioctl(display_fd, FBIOPUT_VSCREENINFO, &org_varinfo);
	if (ret < 0) {
		perror("Error writing variable information.\n");
		goto exit1;
	}

	/* It is better to get fix screen information again. its because
	 * changing variable screen info may also change fix screen info. */
	ret = ioctl(display_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if (ret < 0) {
		perror("Error reading fixed information.\n");
		goto exit1;
	}

	/* Mmap the driver buffers in application space so that application
	 * can write on to them. Driver allocates contiguous memory for
	 * three buffers. These buffers can be displayed one by one. */
	buffersize = fixinfo.line_length * varinfo.yres;
	buffer_addr = (unsigned char *)mmap (0, buffersize,
			(PROT_READ|PROT_WRITE), MAP_SHARED, display_fd, 0);

	if (buffer_addr == MAP_FAILED) {
		printf("MMap failed\n");
		ret = -ENOMEM;
		goto exit1;
	}

	/* Color bar display loop */
	for (i = 0 ; i < loop_count; i ++) {
		/* Fill the buffers with the color bars */
		fill_color_bar(buffer_addr, varinfo.xres, fixinfo.line_length/2,
				varinfo.yres, i%2);
		sleep(1);
	}
	ret = 0;
	munmap(buffer_addr, buffersize);

exit1:
	close(display_fd);
	return ret;
}

static void usage(void)
{
	printf("Usage: saFbdevDisplay [-d <device node>][-l <loop coult>][-h help]");
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

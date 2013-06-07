/*
 * minimal_playback.c
 *
 * ALSA test file to demonstrate minimal playback functionality.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
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
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define BUFF_SIZE 4096

int main (int argc, char *argv[])
{
	int err;
	short buf[BUFF_SIZE];
	int rate = 44100; /* Sample rate */
  unsigned int exact_rate;   /* Sample rate returned by */

	/* Handle for the PCM device */
	snd_pcm_t *playback_handle;

	/* Playback stream */
	snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

	/* This structure contains information about */
	/* the hardware and can be used to specify the */
	/* configuration to be used for the PCM stream. */
	snd_pcm_hw_params_t *hw_params;

	/* Name of the PCM device, like hw:0,0 */
	/* The first number is the number of the soundcard, */
	/* the second number is the number of the device. */
	static char *device = "default"; /* playback device */

	/* Open PCM. The last parameter of this function is the mode. */
	if ((err = snd_pcm_open (&playback_handle, device, stream, 0)) < 0) {
		fprintf (stderr, "cannot open audio device (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	/* Allocate the snd_pcm_hw_params_t structure on the stack. */
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	/* Init hwparams with full configuration space */
	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	/* Set access type. */
	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	/* Set sample format */
	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	/* Set sample rate. If the exact rate is not supported */
	/* by the hardware, use nearest possible rate.         */
	exact_rate = rate;
	if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &exact_rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		exit (1);
	}
	if (rate != exact_rate) {
    	fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n"
        		"==> Using %d Hz instead.\n", rate, exact_rate);
	}

	/* Set number of channels */
	if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 2)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	/* Apply HW parameter settings to PCM device and prepare device  */
	if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (playback_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	/* Write some junk data to produce sound. */
	if ((err = snd_pcm_writei (playback_handle, buf, BUFF_SIZE/2)) != BUFF_SIZE/2) {
		fprintf (stderr, "write to audio interface failed (%s)\n",
			 snd_strerror (err));
		exit (1);
	} else {
		fprintf (stdout, "snd_pcm_writei successful\n");
	}

	snd_pcm_close (playback_handle);
	exit (0);
}


/**
 * @file   test-snd-mixing2.c
 * @author Bram Vlerick <bravl@linux-y9rj>
 * @date   Wed Oct 23 14:49:37 2019
 * 
 * @brief Mixing Algorithm test
 *
 * Test of 3 mixing algorithms designed by:
 * - Dr. Catteau Benoit <benoit.catteau@openpixelsystems.org>
 * - Bram Vlerick <bram.vlerick@openpixelsystems.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>

#include "wave.h"
#include "mixer_lut.h"

#define MAX(a, b) ((a) > (b) ? a : b)
#define MIN(a, b) ((a) < (b) ? a : b)

#define NUMBER_OF_STREAMS 16u

/* static uint8_t volumes_l[NUMBER_OF_STREAMS] = { */
/*                                                25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, */
/* }; */

static uint8_t volumes_l[NUMBER_OF_STREAMS] = {
                                               255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static char filenames[NUMBER_OF_STREAMS][200] = {
                                                 "samples/auA01",
                                                 "samples/auA02",
                                                 "samples/auA03",
                                                 "samples/auA04",
                                                 "samples/auA05",
                                                 "samples/auA06",
                                                 "samples/auA07",
                                                 "samples/auA08",
                                                 "samples/auA09",
                                                 "samples/auA10",
                                                 "samples/auA11",
                                                 "samples/auA12",
                                                 "samples/auA13",
                                                 "samples/auA14",
                                                 "samples/auA15",
                                                 "samples/auA16",
};

int _get_min_chunk_size(struct wave_header_t *hdrs, int nr)
{
        int min = INT_MAX;
        for (int i = 0; i < nr; ++i) {
                if (hdrs[i].subchunk2_size < min)
                        min = hdrs[i].subchunk2_size;
        }

        return min;
}

int main(void)
{
        /* This will be the best code ever... */
        int error = 0, retval = 0;
        struct wave_header_t hdr[NUMBER_OF_STREAMS];
        FILE *fp[NUMBER_OF_STREAMS] = { 0 };
        char *mixed_buf = NULL;

        for (int i = 0; i < NUMBER_OF_STREAMS; ++i) {
                fp[i] = fopen(filenames[i], "rb");
                if (!fp[i]) {
                        printf("Failed to open %s\n", filenames[i]);
                        for (int j = i - 1; j >= 0; j++) {
                                fclose(fp[j]);
                        }

                        return -1;
                }
        }
        FILE *dump = fopen("dump-linear.hex", "wb");
        if (!dump) {
                printf("Dump.hex open failed\n");
                retval = -1;
                goto _close;
        }

        for (int i = 0; i < NUMBER_OF_STREAMS; ++i) {
                error = wave_header_parse(fp[i], &hdr[i]);
                if (error < 0) {
                        printf("Parse of %s failed\n", filenames[i]);
                        retval = error;
                        goto _close;
                }
        }

        char *audio_data[NUMBER_OF_STREAMS] = { 0 };

        for (int i = 0; i < NUMBER_OF_STREAMS; ++i) {
                audio_data[i] = (char *)malloc(hdr[i].subchunk2_size);
                if (!audio_data[i]) {
                        retval = -1;
                        goto _close;
                }

                int n = fread(audio_data[i], sizeof(char), hdr[i].subchunk2_size, fp[i]);
                if (n < 0) {
                        printf("Read of %s failed\n", filenames[i]);
                        retval = -1;
                        goto _close;
                }
        }

        int min_chunk_size = _get_min_chunk_size(hdr, NUMBER_OF_STREAMS);
        mixed_buf = (char *)malloc(min_chunk_size);
        if (!mixed_buf) {
                retval = -1;
                goto _close;
        }

        /* Time this part */
        /* NO DIVIDE FIX LUTLUTLUT */
        clock_t begin = clock();
        double mean_time = 0;
        int count = 0;

        for (int x = 0; x < 10; x++) {
        for (int i = 0; i < min_chunk_size; i += 2) {
                int64_t sample = 0;
                int16_t a = 0;

                for (int j = 0; j < NUMBER_OF_STREAMS; ++j) {
                        /* Char array to S16_LE */
                        int16_t sx = audio_data[j][i + 1] << 8;
                        sx += audio_data[j][i];

                        /* Calculate the volume controlled sample */
                        int32_t sxv = (sx * volumes_l[j]) >> 8;

                        /* Apply mixing factor (Now same as volume) */
                        sample += (sxv * volumes_l[j]);

                        /* Calculate the mixing alfa */
                        a += volumes_l[j];
                }

                /* Perform mixing using LUT for final mixing factor using fixed point */
                /* To remove the fixed point comma (Fucking fixed point shit) */
                /* Division should be >> 16 but due to (a >> 6) addressing we change the */
                /* fixed point comma resulting in a division >> 18 */
                /* (Damn fixed point rubbish) */
                int inter = a & 63;
                int addr = a >> 6;

                int64_t shift_val = ((64 - inter) * interpol_mixer_lut[addr]) + (inter * interpol_mixer_lut[addr+1]);

                sample = (sample * shift_val) >> 24;
                /* Reconvert to character array */
                mixed_buf[i + 1] = (char)((sample & 0xff00) >> 8);
                mixed_buf[i] = (char)(sample & 0x00ff);

                if (i % 512 == 0) {
                        clock_t end = clock();
                        double time = (double)(end - begin) / CLOCKS_PER_SEC;
                        mean_time += time;
                        count++;
                        begin = clock();
                }
        }
        }
        mean_time *= 1000; // miliseconds
        mean_time *= 1000; // microseconds
        printf("LUT: Execution time: %lf usec\n", mean_time / count);
        fwrite(mixed_buf, sizeof(char), min_chunk_size, dump);

 _close:
        for (int i = 0; i < NUMBER_OF_STREAMS; ++i) {
                if (fp[i])
                        fclose(fp[i]);
                if (audio_data[i])
                        free(audio_data[i]);

        }

        if (dump)
                fclose(dump);

        if (mixed_buf)
                free(mixed_buf);

        return retval;
}

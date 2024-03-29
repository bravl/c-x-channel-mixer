#ifndef _MIXER_LUT_H_
#define _MIXER_LUT_H_

static int interpol_mixer_lut[65] = {
    262144, 4096, 2048, 1365, 1024, 819, 683, 585, 512, 455, 410, 372, 341,
    315,    293,  273,  256,  241,  228, 216, 205, 195, 186, 178, 171, 164,
    158,    152,  146,  141,  137,  132, 128, 124, 120, 117, 114, 111, 108,
    105,    102,  100,  98,   95,   93,  91,  89,  87,  85,  84,  82,  80,
    79,     77,   76,   74,   73,   72,  71,  69,  68,  67,  66,  65,  64
};

static int mixer_lut[64] = {
    4096, 2048, 1365, 1024, 819, 683, 585, 512, 455, 410, 372, 341, 315,
    293,  273,  256,  241,  228, 216, 205, 195, 186, 178, 171, 164, 158,
    152,  146,  141,  137,  132, 128, 124, 120, 117, 114, 111, 108, 105,
    102,  100,  98,   95,   93,  91,  89,  87,  85,  84,  82,  80,  79,
    77,   76,   74,   73,   72,  71,  69,  68,  67,  66,  65,  64
};

#endif /*_MIXER_LUT_H_ */

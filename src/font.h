#include "stm32f10x_conf.h"

#ifndef FONTS_H
#define FONTS_H 120
typedef struct {
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	uint8_t CharBytes;    /*!< Count of bytes for one character */
	const uint8_t *data; /*!< Pointer to data font data array */
} FontDef_t;

/**
 * @brief  String length and height
 */
typedef struct {
	uint16_t Length;      /*!< String length in units of pixels */
	uint16_t Height;      /*!< String height in units of pixels */
} FONTS_SIZE_t;

extern FontDef_t Font_7x10;


#endif

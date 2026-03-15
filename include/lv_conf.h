#ifndef LV_CONF_H
#define LV_CONF_H

// Use system malloc/free instead of LVGL's 48KB static pool.
// Our dashboard uses only ~5 labels so actual runtime usage is ~3-5KB.
#define LV_MEM_CUSTOM 1
#define LV_MEM_CUSTOM_INCLUDE <stdlib.h>
#define LV_MEM_CUSTOM_ALLOC   malloc
#define LV_MEM_CUSTOM_FREE    free
#define LV_MEM_CUSTOM_REALLOC realloc

#endif  // LV_CONF_H

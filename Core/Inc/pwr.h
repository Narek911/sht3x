#ifndef __PWR_H__
#define __PWR_H__

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

enum power_save_t
{
    POWER_SAVE_LPSLEEP,
    POWER_SAVE_STOP
};

extern __IO bool is_coming_from_power_save;

int power_save(enum power_save_t t);

#endif /* __PWR_H__ */

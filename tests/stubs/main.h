#ifndef STUB_MAIN_H
#define STUB_MAIN_H

#include <stdint.h>

/* Simulation des ports GPIO */
typedef struct
{
    int dummy;
} GPIO_TypeDef;

/* Simulation des niveaux logiques */
typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET = 1
} GPIO_PinState;

/* Faux ports GPIO */
extern GPIO_TypeDef fake_gpiob;
extern GPIO_TypeDef fake_gpioc;

/* Correspondance avec notre STM32 */
#define LED_STATUS_GPIO_Port  (&fake_gpiob)
#define LED_STATUS_Pin        ((uint16_t)0x0001U)

#define BUTTON_USER_GPIO_Port (&fake_gpioc)
#define BUTTON_USER_Pin       ((uint16_t)0x2000U)

/* Déclarations des fausses fonctions HAL */
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *port,
                                uint16_t pin);

void HAL_GPIO_WritePin(GPIO_TypeDef *port,
                       uint16_t pin,
                       GPIO_PinState state);

void HAL_GPIO_TogglePin(GPIO_TypeDef *port,
                        uint16_t pin);

#endif

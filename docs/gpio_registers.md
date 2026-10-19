# GPIO par registres — STM32H753ZI

## Objectif

Comprendre le fonctionnement des registres matériels du STM32H753ZI et développer un pilote GPIO sans appeler les fonctions HAL GPIO.

## Matériel et environnement

- MCU : STM32H753ZI (Arm Cortex-M7)
- Plateforme de validation : Renode NUCLEO-H753ZI
- Compilateur : ARM GCC avec STM32CubeIDE
- Tests unitaires : GCC sous WSL

## Registres utilisés

| Registre | Rôle |
|---|---|
| RCC_AHB4ENR | Activation des horloges GPIOB et GPIOC |
| GPIOx_MODER | Configuration des modes entrée/sortie |
| GPIOx_OTYPER | Type de sortie |
| GPIOx_PUPDR | Configuration pull-up/pull-down |
| GPIOx_IDR | Lecture des entrées |
| GPIOx_ODR | État de sortie |
| GPIOx_BSRR | Mise à 1 ou à 0 d'une sortie |

## Configuration réalisée

- PB0 : LED configurée en sortie numérique.
- PC13 : bouton configuré en entrée numérique.
- Anti-rebond logiciel : 30 ms dans `main.c`.

## API développée

- `gpio_init()` : initialise les GPIO.
- `gpio_set()` : met une sortie à 1.
- `gpio_clear()` : met une sortie à 0.
- `gpio_toggle()` : inverse une sortie.
- `gpio_read()` : lit une entrée.

Les fonctions applicatives existantes ont été conservées pour assurer la compatibilité avec `main.c`.

## Validation

- Compilation ARM avec STM32CubeIDE : réussie.
- Vérification des registres RCC, MODER et ODR dans Renode : réussie.
- Fonctionnement LED/bouton dans Renode : validé.
- Tests unitaires GPIO sous GCC : 6 tests réussis.

Les tests sur PC utilisent des registres simulés. Ils ne remplacent pas la validation sur matériel physique.

## Conclusion

Cette étape a permis de comprendre comment le Cortex-M7 accède aux registres des périphériques à travers l'espace d'adressage mémoire.

Le pilote GPIO est désormais développé à l'aide des définitions CMSIS, sans appels HAL GPIO dans `gpio.c`.

Cette expérience constitue une base pour comprendre les périphériques ADC, timers/PWM et FDCAN du Mini-ECU.

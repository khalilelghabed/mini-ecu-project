
# Jour 14 - Timer TIM7 et LED non bloquante

## Objectif

Developper une base de temps de 1 ms avec TIM7 et faire
clignoter la LED bleue LD2 a 2 Hz, sans HAL_Delay().

## Configuration materielle

- Carte : NUCLEO-L4R5ZI-P
- Microcontroleur : STM32L4R5ZI
- Timer : TIM7
- Frequence timer nominale : 16 MHz
- Prescaler (PSC) : 15
- Auto-reload (ARR) : 999
- Periode de mise a jour : 1 ms
- LED bleue LD2 : PB7, GPIO Output

Formule :

T = (PSC + 1) * (ARR + 1) / f_TIM

T = 16 * 1000 / 16000000 = 0.001 s

## Architecture logicielle

src/hal/timer.h
- Declaration de l'interface du timer.

src/hal/timer.c
- Demarrage de TIM7 en mode interruption.
- Incrementation du compteur s_timer_ms.
- Lecture du temps et verification des echeances.

src/app/led_blink.h
- Declaration de l'interface applicative.

src/app/led_blink.c
- Lecture du compteur de millisecondes.
- Inversion de LD2 toutes les 250 ms.

main.c
- Initialise les peripheriques CubeMX.
- Demarre le module timer.
- Initialise et execute le module LED.
- Conserve les anciennes fonctions UART et EXTI.

## Fonctionnement

TIM7 genere un evenement de mise a jour toutes les 1 ms.

TIM7_IRQHandler()
    -> HAL_TIM_IRQHandler()
    -> HAL_TIM_PeriodElapsedCallback()
    -> s_timer_ms++

Dans la boucle principale, led_blink_process()
consulte le temps et inverse LD2 toutes les 250 ms.

Un cycle ON/OFF dure donc 500 ms, soit 2 Hz.

## Correspondance AUTOSAR Classic

- timer.c : BSW / MCAL, equivalent pedagogique GPT.
- Acces GPIO : BSW / MCAL, equivalent DIO/PORT.
- led_blink.c : Application Layer, analogue a un SWC.

Le projet utilise STM32 HAL et n'est pas une
implementation AUTOSAR conforme.

## Tests realises

- Compilation du firmware : validee.
- Programmation STM32 via ST-LINK : validee.
- Interruption TIM7 observee avec SWD : validee.
- Compteur environ 5000 apres 5 secondes : valide.
- Clignotement LD2 a 2 Hz : observe visuellement.
- Mesure par analyseur logique : non realisee.

## Conclusion

Le module TIM7 fournit une base de temps applicative
utilisable sans temporisation bloquante.

La logique de clignotement est separee du driver timer.

Prochaine etape : Jour 15 - Ordonnancement cooperatif.

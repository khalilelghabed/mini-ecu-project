# mini-ecu

Mini-ECU de régulation de vitesse : deux nœuds STM32 reliés par un bus CAN,
avec diagnostic UDS simplifié et banc de test Python/Simulink.

## Objectif
Un nœud capteur lit une consigne de vitesse (potentiomètre) et l'envoie sur CAN.
Un nœud de contrôle la reçoit, régule un moteur en PWM avec un PID,
et répond à des requêtes de diagnostic UDS.

## Architecture (principe)
Potentiomètre -> [Nœud capteur] --CAN--> [Nœud contrôle] -> PWM -> Moteur
                                              ^
                                       Testeur Python (UDS)

## Structure
- `src/hal/` : pilotes (GPIO, UART, timer, ADC, PWM, CAN)
- `src/os/` : ordonnanceur, FreeRTOS
- `src/app/` : capteur, contrôle, PID
- `src/diag/` : ISO-TP, UDS, DTC
- `tests/` : tests unitaires et testeur Python
- `simulink/` : modèle véhicule et PID
- `docs/` : schémas, messages CAN, exigences

## Matériel prévu
2 cartes STM32 Nucleo (FDCAN), 2 transceivers CAN, potentiomètre, moteur DC + MOSFET.

## Avancement
- [ ] Semaine 1 : dépôt et structure
- [ ] Semaine 2 : GPIO, UART, console
- [ ] Semaine 3 : timers, PWM, ADC
- [ ] Semaine 4 : bus série, FreeRTOS
- [ ] Semaine 5 : CAN
- [ ] Semaine 6 : UDS
- [ ] Semaine 7 : Simulink et PID
- [ ] Semaine 8 : architecture AUTOSAR, tests, documentation

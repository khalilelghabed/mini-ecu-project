# Mini-ECU — Calculateur embarqué automobile à deux nœuds STM32
 
Projet de développement progressif d'un Mini-ECU de régulation de vitesse, avec communication CAN, commande moteur PWM/PID, diagnostic UDS simplifié et validation par tests unitaires, Renode et, à terme, Python/Simulink.
 
**État actuel :** les briques logicielles de base sont développées et validées (PC et simulation Renode). Le fonctionnement à deux STM32 reliés par CAN, la régulation PID/PWM et le diagnostic UDS sont des **objectifs** du projet, pas encore des fonctionnalités validées de bout en bout. Aucun essai n'a encore été fait sur une carte physique : tout est validé sur PC ou en simulation.
 
---
 
## 1. À quoi sert ce projet ?
 
Un ECU (Electronic Control Unit) est un calculateur qui reçoit des informations, exécute une logique de contrôle et agit sur des périphériques ou communique avec d'autres calculateurs. Ce projet reproduit, à une échelle pédagogique, cette chaîne de fonctionnement :
 
1. Acquérir une consigne de vitesse à l'aide d'un potentiomètre (ADC).
2. Transmettre cette consigne sur un réseau CAN depuis un nœud capteur.
3. Recevoir et traiter la consigne dans un nœud de contrôle.
4. Réguler un moteur DC au moyen d'une sortie PWM et d'un correcteur PID, à partir d'une mesure de vitesse (ou d'une grandeur de retour issue d'un modèle de simulation) à définir.
5. Diagnostiquer certaines informations et certains défauts depuis un testeur Python, avec des services UDS simplifiés.
6. Tester séparément les fonctions C, puis leur intégration sur la cible STM32.
Le projet sert à apprendre non seulement à programmer un STM32, mais aussi à organiser, tester et intégrer le logiciel d'un calculateur automobile.
 
---
 
## 2. Architecture cible : deux nœuds STM32 sur CAN
 
```
   Potentiomètre
       │
       ▼
┌───────────────────────┐
│ NŒUD CAPTEUR          │
│ STM32 Nucleo / FDCAN  │
│ ADC → consigne        │
│ Encodage des signaux  │
└───────────┬───────────┘
            │ CAN
            ▼
┌─────────────────────────────┐
│ NŒUD CONTRÔLE               │
│ STM32 Nucleo / FDCAN        │
│ Réception CAN → traitement  │
│ PID → PWM → étage MOSFET    │────► Moteur DC
│ Gestion des défauts         │
└─────────────┬───────────────┘
              ▲
              │ Diagnostic UDS / ISO-TP (à développer)
       Testeur Python (PC)
```
 
- **Version physique (prévue) :** les deux nœuds sont reliés par un bus CAN avec un transceiver par nœud.
- **Version simulée (actuelle) :** les nœuds sont des machines virtuelles Renode reliées par un bus virtuel (`CANHub`), sans transceiver.
Le premier cas d'utilisation visé : le potentiomètre représente la vitesse demandée ; le nœud capteur convertit la tension en consigne, l'encode puis l'envoie par CAN ; le nœud de contrôle décode la consigne, calcule une commande moteur et ajuste le PWM ; le testeur peut ensuite demander des données de fonctionnement ou consulter les défauts.
 
> Ce scénario décrit la finalité du Mini-ECU et non l'état d'avancement actuel.
 
---
 
## 3. Ce qui fonctionne déjà
 
Nous avons commencé par les composants fondamentaux, testables sans réseau CAN ni moteur.
 
| Composant | Fonction dans le futur Mini-ECU | État constaté |
|---|---|---|
| `src/app/signals.c` | Encoder et décoder les signaux de vitesse, consigne, température et états | Développé ; 11 tests unitaires réussis sur PC |
| `src/app/ringbuf.c` | Stocker temporairement des données dans un tampon circulaire (par exemple des messages reçus) | Module développé et testé sur PC ; intégration CAN à venir |
| `src/hal/gpio.c` | Lire un bouton et piloter une LED sans mettre les appels HAL partout dans l'application | 3 tests unitaires GPIO réussis sur PC |
| `main.c` STM32 | Orchestrer la lecture du bouton et le changement d'état de la LED | Compilation ARM et scénario GPIO validés dans Renode |
| `tests/stubs/main.h` | Simuler les éléments de la HAL STM32 pour tester `gpio.c` sur ordinateur | Utilisé pour les tests GPIO |
| `Makefile` | Automatiser la compilation et l'exécution des tests sur PC | Cible `make test_gpio` vérifiée |
 
### Validation actuelle dans Renode
 
La plateforme simulée est une **NUCLEO-H753ZI** (STM32H753ZI, Cortex-M7) :
 
- PB0 : LED LD1.
- PC13 : bouton utilisateur.
- Lecture du bouton par polling (lecture régulière de l'entrée).
- Anti-rebond logiciel : temporisation de 30 ms, actuellement dans `main.c` (à extraire dans un module testé sur PC, voir la feuille de route).
| Action | État attendu et observé dans Renode |
|---|---|
| Démarrage | LED OFF (`False`) |
| Premier appui | LED ON (`True`) |
| Relâchement | LED reste ON (`True`) |
| Deuxième appui | LED OFF (`False`) |
 
> Ce test valide l'intégration logicielle GPIO **en simulation**. Il ne valide pas l'anti-rebond sur un bouton physique (un bouton simulé ne rebondit pas), ni la communication CAN.
 
---
 
## 4. Pourquoi ces modules sont utiles ensemble
 
Dans l'application finale, chaque module aura une responsabilité précise :
 
```
ENTRÉES                       TRAITEMENT                     SORTIES
────────                       ──────────                     ───────
Bouton ─► gpio.c ────────────► Logique d'état ──────────────► gpio.c ─► LED
ADC / potentiomètre ─────────► Consigne ─► signals.c ───────► Pilote CAN
Bus CAN ─► pilote CAN ───────► ringbuf.c ─► décodage ───────► PID ─► PWM
Requêtes de diagnostic ──────► ISO-TP / UDS ────────────────► Réponses CAN
```
 
Cette architecture sépare :
 
- **La logique applicative** : décisions, états du calculateur, consignes, PID.
- **Les services réutilisables** : encodage des signaux, tampon circulaire, diagnostic.
- **La couche matérielle (HAL)** : GPIO, ADC, timer, PWM et CAN.
---
 
## 5. Structure du dépôt
 
Certains éléments sont encore prévus (indiqués en commentaire).
 
```
mini-ecu-project/
├── src/
│   ├── app/
│   │   ├── signals.c / signals.h    # Encodage des signaux (existant)
│   │   ├── ringbuf.c / ringbuf.h    # Tampon circulaire (existant)
│   │   └── ...                      # debounce, capteur, PID, logique ECU (prévus)
│   ├── hal/
│   │   ├── gpio.c / gpio.h          # Interface GPIO (existant)
│   │   ├── can_types.h              # Types CAN (existant)
│   │   └── ...                      # ADC, PWM, CAN, UART (prévus)
│   ├── os/                          # Ordonnanceur / FreeRTOS (prévu)
│   └── diag/                        # ISO-TP, UDS, DTC (prévus)
├── tests/
│   ├── test_signals.c
│   ├── test_ringbuf.c
│   ├── test_gpio.c
│   └── stubs/main.h                 # Fausse HAL pour les tests GCC
├── firmware/
│   └── sensor_node/                 # Projet STM32CubeIDE MiniECU_SensorNode_H753 (à ajouter au dépôt, sans Debug/)
├── sim/
│   └── gpio_scenario.resc           # Scénario Renode reproductible (à ajouter)
├── simulink/                        # Modèle moteur / véhicule / PID (prévu)
├── docs/                            # Schémas, messages CAN, exigences, fiche registres
├── build/                           # Fichiers générés (ignorés par Git)
├── Makefile
└── README.md
```
 
Le projet STM32CubeIDE utilisé pour la simulation s'appelle `MiniECU_SensorNode_H753`. Il compile les sources GPIO partagées avec le dépôt grâce à un dossier lié vers `src/hal`.
 
---
 
## 6. Format des données CAN prévu — 8 octets
 
L'encodage défini par `signals.c` représente la charge utile d'un message CAN classique (DLC = 8). Les champs ne constituent pas, à eux seuls, une trame CAN complète sur le bus.
 
| Octet(s) | Information | Encodage | Conversion |
|---|---|---|---|
| 0–1 | Vitesse | `uint16_t` little-endian | Vitesse [km/h] = brut × 0,1 |
| 2–3 | Consigne | `uint16_t` little-endian | Consigne [km/h] = brut × 0,1 |
| 4 | Température | `uint8_t` | Température [°C] = brut − 40 |
| 5, bit 0 | Régulation active | Drapeau 0 ou 1 | 1 = active |
| 5, bit 1 | Défaut | Drapeau 0 ou 1 | 1 = défaut signalé |
| 5, bit 2 | Frein | Drapeau 0 ou 1 | 1 = frein signalé |
| 5, bits 3–7 | Réservés | Bits | Initialisés à 0 |
| 6–7 | Réservés | Octets | Initialisés à 0 |
 
- Les champs `uint16_t` vont théoriquement de 0 à 6553,5 km/h avec cette échelle ; ce n'est pas la plage réaliste d'une voiture.
- La température représentable va de −40 à 215 °C.
- Le little-endian signifie que l'octet de poids faible vient en premier.
- Le DLC = 8 indique huit octets de données, et non huit octets pour l'ensemble de la trame physique CAN.
- L'identifiant CAN, la périodicité d'émission et les règles de traitement des défauts restent à définir dans `docs/`.
---
 
## 7. Règles de codage
 
1. **Pas d'allocation dynamique** (`malloc`, `calloc`, `realloc`, `free`) dans `src/`.
   - Pourquoi : fragmentation du tas, échec d'allocation imprévisible, temps d'exécution non déterministe, interdit ou restreint par MISRA C et ISO 26262.
   - À la place : tableaux statiques de taille fixe, buffers fournis par l'appelant (voir `ringbuf`).
   - Vérification automatique : `make check`.
2. Compilation de `src/` avec `-Wall -Wextra -Werror -std=c11` : zéro avertissement accepté.
3. `volatile` pour les registres matériels et les variables partagées avec une interruption.
4. `const` partout où une donnée n'est pas modifiée ; `static` pour tout ce qui est privé à un fichier.
5. Types à taille fixe (`uint8_t`, `uint16_t`, …) pour tout ce qui est échangé (trames, registres).
---
 
## 8. Comment compiler et tester
 
### Pré-requis sur PC
 
- Ubuntu sous WSL (ou un environnement Linux équivalent).
- GCC et GNU Make.
- Sources du dépôt dans `~/mini-ecu-project`.
### Cibles du Makefile
 
| Commande | Rôle |
|---|---|
| `make` | Compiler les tests |
| `make test` | Exécuter les tests unitaires |
| `make test_gpio` | Compiler et exécuter les tests du module GPIO avec la fausse HAL |
| `make map` | Générer le fichier `.map` (Flash et RAM utilisées) |
| `make check` | Vérifier qu'il n'y a pas d'allocation dynamique dans `src/` |
| `make clean` | Supprimer `build/` |
| `make help` | Afficher l'aide |
 
### Tests unitaires du module GPIO
 
```bash
cd ~/mini-ecu-project
make test_gpio
```
 
Résultat constaté :
 
```
=== TESTS UNITAIRES GPIO ===
 
[TEST] led_write        OK
[TEST] led_toggle       OK
[TEST] button_pressed   OK
 
Tous les tests GPIO ont réussi !
```
 
Principe : sur le PC, `tests/stubs/main.h` remplace les types et constantes matériels fournis habituellement par STM32CubeIDE. Des fonctions HAL simulées vérifient les appels réalisés par `gpio.c`. Cela permet de tester la logique du module sans carte STM32.
 
### Tests unitaires des signaux
 
```bash
make test
```
 
Résultat constaté : 11 tests réussis (initialisation, vitesse, consigne, température, drapeaux, pointeurs invalides et valeurs limites).
 
### Firmware STM32 et simulation Renode
 
Le firmware pour le STM32H753ZI est compilé dans STM32CubeIDE. Les fichiers `src/hal/gpio.c` et `src/hal/gpio.h` sont partagés avec ce projet via un dossier lié. Sur STM32, `gpio.c` utilise la vraie HAL STM32 ; lors des tests PC, il utilise la HAL simulée.
 
Pour refaire le scénario dans Renode, après avoir compilé le firmware et généré le fichier `.elf`, ouvrir le Monitor Renode :
 
```
mach create "miniEcu_test"
machine LoadPlatformDescription @platforms/boards/nucleo_h753zi.repl
sysbus LoadELF @<chemin_du_projet>/Debug/MiniECU_SensorNode_H753.elf
start
sysbus.gpioPortB.GreenLED State
sysbus.gpioPortC.UserButton1 Press
sysbus.gpioPortB.GreenLED State
sysbus.gpioPortC.UserButton1 Release
sysbus.gpioPortB.GreenLED State
sysbus.gpioPortC.UserButton1 Press
sysbus.gpioPortB.GreenLED State
```
 
Remplacer `<chemin_du_projet>` par l'emplacement réel du projet CubeIDE. Laisser Renode exécuter le firmware suffisamment longtemps entre les actions pour dépasser les 30 ms d'anti-rebond. Les valeurs attendues sont `False`, `True`, `True`, `False`.
 
---
 
## 9. Matériel prévu pour la version physique
 
Cette partie est une **prévision** : le projet est actuellement validé uniquement sur PC et en simulation.
 
- 2 cartes STM32 Nucleo compatibles FDCAN (modèles exacts à confirmer ; la NUCLEO-H753ZI est la carte simulée).
- 2 transceivers CAN compatibles avec les niveaux logiques des cartes (CAN-FD si le mode FD est utilisé).
- 2 résistances de terminaison de 120 Ω.
- Potentiomètre pour la consigne de vitesse.
- Moteur DC et circuit de puissance adapté (MOSFET, protection, alimentation dédiée selon les caractéristiques du moteur).
- Dispositif de mesure de vitesse du moteur, ou mesure simulée lors des premières validations du PID.
- Interface CAN adaptée pour le testeur Python.
La validation matérielle des deux nœuds, du moteur et du bus CAN reste à faire. Un transceiver CAN est nécessaire entre un contrôleur FDCAN du microcontrôleur et les lignes physiques CAN_H/CAN_L.
 
---
 
## 10. Feuille de route
 
Le planning initial de huit semaines est conservé comme découpage des travaux, sans présenter les semaines futures comme terminées.
 
| Semaine | Contenu | Avancement |
|---|---|---|
| 1 | Dépôt, structure, bases C, Makefile, règles de codage, GPIO (LED, bouton), anti-rebond | Quasi terminée : anti-rebond à extraire en module testé, tag `semaine-1` à créer |
| 2 | Registres GPIO, horloge, UART, console, débogage | À réaliser (prochaine étape : GPIO par registres) |
| 3 | Interruptions, timers, PWM, ADC | À réaliser |
| 4 | SPI, I2C, DMA, watchdog, FreeRTOS | À réaliser |
| 5 | Communication CAN entre les deux nœuds, fichier DBC | À réaliser |
| 6 | CAN-FD, transport ISO-TP, services UDS, DTC | À réaliser |
| 7 | Modèle Simulink, régulation PID | À réaliser |
| 8 | Architecture inspirée d'AUTOSAR, normes (ISO 26262, ASPICE), tests, documentation, portfolio | À réaliser |
 
Les étapes effectives pourront être réordonnées selon les résultats des tests et le matériel disponible.
 
### Prochaines étapes
 
1. Extraire l'anti-rebond de `main.c` dans `src/app/debounce.c` avec ses tests sur PC, puis créer le tag `semaine-1`.
2. **Jour 7 :** réécrire `src/hal/gpio.c` avec des accès directs aux registres (RCC, MODER, ODR, BSRR), sans HAL, avec une API `gpio_set`, `gpio_clear`, `gpio_toggle`, `gpio_read`. L'interface `mini_ecu_*` ne change pas.
### Intégration prévue ensuite
 
Relier les modules déjà développés : intégrer `signals.c` dans le firmware, inverser un état applicatif `regulation_active` à chaque nouvel appui confirmé, mettre à jour le bit 0 du champ « drapeaux » dans les huit octets de données, et refléter cet état sur la LED. Ce scénario fera passer le projet d'un test GPIO isolé à une petite fonctionnalité intégrée de calculateur.
 
---
 
## 11. Documentation et suivi
 
Pour chaque nouvelle fonction, conserver :
 
- Le besoin et le rôle dans le Mini-ECU.
- L'interface logicielle (`.h`) et son implémentation (`.c`).
- Les commandes de compilation et les tests reproductibles.
- Les résultats réels : PC, simulation ou carte physique.
- Les limites connues et les travaux restant à effectuer.
Les fichiers générés (`build/`, objets `.o`, exécutables de test, `Debug/` de CubeIDE) ne doivent pas être ajoutés par erreur à Git. Les sources, les tests, la documentation et le Makefile sont les éléments à versionner.
 
**Statut global :** bases logicielles et chaîne GPIO validées sur PC et en simulation ; application Mini-ECU à deux nœuds CAN en cours de développement.
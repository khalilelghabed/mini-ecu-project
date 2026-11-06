# Interruptions EXTI / NVIC et anti-rebond

**Projet :** Mini-ECU automobile  
**Date :** 06/11/2026  
**Microcontrôleur :** STM32H753ZI — ARM Cortex-M7  
**Environnement :** STM32CubeIDE, Embedded C, STM32 HAL, Renode, Ubuntu WSL  
**Statut :** Développement terminé, validation fonctionnelle réalisée dans Renode.

---

## 1. Objectif de la séance

L'objectif du Jour 13 est de remplacer la détection du bouton par interrogation périodique (*polling*) par une détection utilisant une interruption matérielle EXTI.

Nous souhaitons :

- Configurer le bouton PC13 comme source d'interruption externe.
- Comprendre le rôle du NVIC et des fonctions ISR.
- Détecter un front montant sur PC13.
- Enregistrer une fonction callback avec STM32 HAL.
- Implémenter un anti-rebond logiciel de 30 ms.
- Détecter un seul événement par appui valide.
- Faire changer l'état de la LED PB0.
- Compiler et tester le firmware dans Renode.

Cette réalisation constitue une première étape vers la gestion des événements externes du Mini-ECU.

## 2. Notions théoriques étudiées

### 2.1 Polling et interruption

**Polling :** le microcontrôleur lit régulièrement l'état d'une entrée pour détecter un changement.

**Interruption :** un périphérique matériel signale un événement au processeur, qui suspend temporairement son exécution normale pour appeler une fonction de traitement.

Pour notre application, nous utilisons une interruption externe afin de signaler rapidement un appui sur PC13.

L'état du bouton reste ensuite lu dans la boucle principale pour vérifier l'anti-rebond.

### 2.2 GPIO et ligne EXTI

- **PC13** : broche physique numéro 13 du port GPIO C.
- **EXTI13** : ligne matérielle capable de détecter une transition sur la broche GPIO qui lui est associée.

Une ligne EXTI n'est donc pas une broche physique : c'est un canal de détection interne au microcontrôleur.

Dans notre configuration :

`PC13 → EXTI13`

### 2.3 NVIC

Le NVIC (*Nested Vectored Interrupt Controller*) est le contrôleur d'interruptions du processeur ARM Cortex-M.

Il permet notamment :

- D'activer ou désactiver une interruption.
- De gérer sa priorité.
- De sélectionner la routine d'interruption correspondante.
- De gérer les interruptions prioritaires.

Nous utilisons :

`EXTI15_10_IRQn`

Cette IRQ regroupe les demandes des lignes EXTI10 à EXTI15.

La priorité configurée dans notre projet est :

- Priorité de préemption : `5`
- Sous-priorité : `0`

### 2.4 ISR — Interrupt Service Routine

L'ISR est une fonction exécutée lorsqu'une interruption est prise en charge par le processeur.

Dans notre projet :

```c
void EXTI15_10_IRQHandler(void)
{
    HAL_EXTI_IRQHandler(&H_EXTI_13);
}
```

La fonction `HAL_EXTI_IRQHandler()` :

1. Identifie la ligne EXTI à traiter.
2. Vérifie son drapeau d'interruption en attente.
3. Acquitte ce drapeau matériel.
4. Appelle le callback enregistré si l'interruption correspondante est présente.

### 2.5 Callback et pointeur de fonction

Un callback est une fonction dont l'adresse est enregistrée afin de permettre son exécution ultérieure.

Dans notre projet :

```c
HAL_EXTI_RegisterCallback(
    &H_EXTI_13,
    HAL_EXTI_COMMON_CB_ID,
    Exti_OnInterrupt
);
```

Cette fonction associe `Exti_OnInterrupt` au handle `H_EXTI_13`.

Lorsque `HAL_EXTI_IRQHandler()` traite une interruption EXTI13 valide, elle peut retrouver et appeler ce callback.

**Important :** `MiniECU_EXTI_Init()` est exécutée pendant l'initialisation du système, et non à chaque interruption.

## 3. Configuration matérielle dans STM32CubeIDE

| Paramètre | Configuration |
|---|---|
| Microcontrôleur | STM32H753ZI |
| Entrée | PC13 |
| Mode GPIO | External Interrupt — Rising Edge |
| Pull-up / Pull-down | Pull-down |
| Ligne EXTI | EXTI13 |
| IRQ | EXTI15_10_IRQn |
| Priorité NVIC | 5 |
| Sous-priorité | 0 |
| Sortie LED | PB0 |
| Anti-rebond | 30 ms |

Lorsqu'un front montant est détecté sur PC13, une demande d'interruption est générée.

Le comportement dépend de la configuration GPIO et du modèle matériel utilisé.

## 4. Architecture logicielle

Les fichiers concernés sont :

| Fichier | Rôle |
|---|---|
| `src/hal/exti.h` | Interface publique du module |
| `src/hal/exti.c` | Callback, gestion des événements et anti-rebond |
| `Core/Src/main.c` | Initialisation et traitement applicatif |
| `Core/Src/stm32h7xx_it.c` | Gestionnaire d'interruption |
| Fichier `.ioc` | Configuration des périphériques STM32 |

### 4.1 Chaîne d'exécution

```text
Appui bouton PC13
       |
       v
Front montant 0 → 1
       |
       v
Ligne EXTI13
       |
       v
Contrôleur NVIC
       |
       v
EXTI15_10_IRQHandler()
       |
       v
HAL_EXTI_IRQHandler(&H_EXTI_13)
       |
       v
Exti_OnInterrupt()
       |
       v
s_irq_pending = 1U
       |
       v
Retour à la boucle principale
       |
       v
Exti_Process()
       |
       v
Anti-rebond de 30 ms
       |
       v
Exti_TakeEmergencyEvent()
       |
       v
mini_ecu_led_toggle()
       |
       v
Changement d'état de PB0
```

## 5. Initialisation EXTI

La fonction d'initialisation associe la ligne EXTI13 à un handle, puis enregistre le callback.

```c
HAL_StatusTypeDef MiniECU_EXTI_Init(void)
{
    HAL_StatusTypeDef status;

    status = HAL_EXTI_GetHandle(
        &H_EXTI_13,
        EXTI_LINE_13
    );

    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_EXTI_RegisterCallback(
        &H_EXTI_13,
        HAL_EXTI_COMMON_CB_ID,
        Exti_OnInterrupt
    );

    return status;
}
```

Le handle `H_EXTI_13` conserve les informations nécessaires au traitement de la ligne EXTI13.

Il contient notamment la référence à la ligne et le pointeur vers la fonction callback.

Cette initialisation est réalisée au démarrage, après la configuration GPIO.

## 6. Développement du module EXTI

### 6.1 Callback d'interruption

Dans `src/hal/exti.c` :

```c
static volatile uint8_t s_irq_pending = 0U;

void Exti_OnInterrupt(void)
{
    s_irq_pending = 1U;
}
```

Lorsqu'une interruption est reconnue, la variable `s_irq_pending` passe à `1`.

Cette variable permet de signaler qu'un événement doit être traité dans le programme principal.

Le mot-clé `volatile` indique au compilateur que cette variable peut être modifiée en dehors du flux normal d'exécution.

Le callback ne réalise ni temporisation ni traitement complexe.

### 6.2 Machine à états anti-rebond

Pour éviter de détecter plusieurs appuis à cause des rebonds mécaniques, nous avons développé une machine à quatre états :

```c
typedef enum
{
    EXTI_WAIT_PRESS = 0,
    EXTI_VERIFY_PRESS,
    EXTI_WAIT_RELEASE,
    EXTI_VERIFY_RELEASE
} Exti_State_t;
```

**EXTI_WAIT_PRESS**

Attend la réception d'une interruption signalant un nouvel appui.

Lorsqu'une interruption est détectée, la fonction mémorise le temps courant et passe à l'état de vérification.

**EXTI_VERIFY_PRESS**

Attend au moins 30 ms avant de vérifier que le bouton est encore à l'état haut.

Si le niveau est haut, l'appui est accepté et un événement logique est généré.

Sinon, le système retourne à `EXTI_WAIT_PRESS`.

**EXTI_WAIT_RELEASE**

Après validation d'un appui, le système attend que le bouton revienne à l'état bas.

Aucun nouvel événement d'appui ne doit être généré pendant le maintien du bouton.

**EXTI_VERIFY_RELEASE**

La fonction vérifie le relâchement sur une période de 30 ms.

Si le bouton revient à l'état haut, elle retourne à `EXTI_WAIT_RELEASE`.

Après validation du relâchement, le système retourne à `EXTI_WAIT_PRESS`.

Cette machine à états vise à garantir un seul événement logique par cycle appui-relâchement.

La validation repose toutefois sur des lectures échantillonnées de l'entrée ; elle ne garantit pas qu'aucun rebond très bref n'a eu lieu entre deux lectures.

### 6.3 Gestion de l'événement confirmé

Après confirmation de l'appui :

```c
s_emergency_event = true;
```

La boucle principale récupère cet événement avec :

```c
Exti_TakeEmergencyEvent();
```

Cette fonction renvoie `true` lorsqu'un événement confirmé est disponible, puis remet le drapeau à `false`.

Il s'agit d'un événement logiciel consommable, et non d'un état de défaut permanent de l'ECU.

### 6.4 Intégration dans la boucle principale

Dans `main.c`, le traitement est effectué régulièrement :

```c
uint32_t now_ms = HAL_GetTick();

bool button_high =
    (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)
        == GPIO_PIN_SET);

Exti_Process(now_ms, button_high);

if (Exti_TakeEmergencyEvent())
{
    mini_ecu_led_toggle();
}
```

La boucle principale continue également de gérer la communication UART et les commandes de la console.

L'interruption sert à signaler l'événement ; la validation anti-rebond reste hors de l'ISR.

## 7. Compilation et validation STM32CubeIDE

Après intégration des fichiers et de la configuration EXTI, le projet a été compilé.

Résultat obtenu :

```text
Build Finished.
0 errors, 0 warnings.
```

Le firmware généré est :

`MiniECU_SensorNode_H753.elf`

La compilation valide la cohérence du code C et des dépendances utilisées, mais ne prouve pas à elle seule le comportement des interruptions.

## 8. Validation fonctionnelle dans Renode

### 8.1 Plateforme simulée

Nous avons réutilisé la plateforme Renode :

```text
platforms/boards/nucleo_h753zi.repl
```

Périphériques utilisés :

- Bouton virtuel : `sysbus.gpioPortC.UserButton1`
- LED virtuelle : `sysbus.gpioPortB.GreenLED`
- Console de communication : `sysbus.usart3`

### 8.2 Console UART

La console UART est accessible avec :

```text
showAnalyzer sysbus.usart3
```

Les commandes de contrôle de LED ont été testées :

```text
LED ON
LED OFF
```

Les commandes sont traitées par le gestionnaire UART de notre Mini-ECU.

Ce test valide la communication UART et le contrôle de PB0 indépendamment du bouton EXTI.

### 8.3 Test de l'interruption EXTI13

Dans le Monitor Renode :

```text
sysbus.gpioPortB.GreenLED State

sysbus.gpioPortC.UserButton1 Press

sysbus.gpioPortB.GreenLED State

sysbus.gpioPortC.UserButton1 Release

sysbus.gpioPortB.GreenLED State

sysbus.gpioPortC.UserButton1 Press

sysbus.gpioPortB.GreenLED State
```

Nous avons laissé la simulation fonctionner suffisamment longtemps entre les actions pour permettre la validation des temporisations logicielles.

### 8.4 Résultats obtenus

| Test | Comportement attendu | Résultat |
|---|---|---|
| Commande UART LED ON | Allumage PB0 | PASS |
| Commande UART LED OFF | Extinction PB0 | PASS |
| Premier appui PC13 | Inversion de la LED | PASS |
| Relâchement PC13 | LED inchangée | PASS |
| Deuxième appui PC13 | Nouvelle inversion | PASS |
| Bouton maintenu | Aucune répétition | PASS |

Les résultats de comportement ont été confirmés lors de nos tests Renode.

Ils démontrent le fonctionnement du scénario testé dans l'environnement virtuel.

## 9. Résultats et compétences acquises

Cette séance a permis de comprendre et de mettre en pratique :

- L'architecture des interruptions ARM Cortex-M.
- La relation entre GPIO, EXTI et NVIC.
- La différence entre une ISR et un callback.
- L'utilisation de pointeurs de fonction avec STM32 HAL.
- L'initialisation et la gestion d'un handle EXTI.
- Les variables partagées entre interruption et application.
- L'utilité de `volatile` en Embedded C.
- Les machines à états pour l'anti-rebond.
- La séparation entre traitement matériel et logique applicative.
- La validation du firmware embarqué avec Renode.

## 10. Limites et améliorations futures

Les résultats sont positifs pour notre démonstrateur, mais plusieurs points restent à valider :

1. Réaliser des tests avec injection de rebonds rapides et mesures temporelles.
2. Tester les cas limites, notamment les appuis très courts.
3. Étudier les accès concurrents aux drapeaux partagés entre ISR et boucle principale.
4. Valider le comportement sur une carte STM32 physique.
5. Intégrer ultérieurement l'événement d'urgence à une véritable machine d'états du Mini-ECU.
6. Définir les exigences de sécurité avant toute utilisation comme dispositif d'arrêt d'urgence réel.

Le changement d'état de la LED constitue uniquement une indication de test.

## 11. Conclusion 

Nous avons remplacé la détection du bouton par polling par une architecture fondée sur une interruption EXTI.

Le signal d'interruption est traité rapidement dans un callback, tandis que l'anti-rebond et la décision applicative sont exécutés dans la boucle principale.

La compilation et les tests Renode ont permis de valider le fonctionnement du scénario d'appui et de relâchement.

Cette étape constitue une base pour les prochaines fonctions temps réel du Mini-ECU.


**Mini-ECU Automobile.**

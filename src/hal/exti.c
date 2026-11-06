
#include "hal/exti.h"

/* Les quatre etats de notre bouton */
typedef enum
{
    EXTI_WAIT_PRESS = 0,
    EXTI_VERIFY_PRESS,
    EXTI_WAIT_RELEASE,
    EXTI_VERIFY_RELEASE
} Exti_State_t;

/* Modifiee dans l'interruption */
static volatile uint8_t s_irq_pending = 0U;

/* Variables utilisees dans la boucle principale */
static Exti_State_t s_state = EXTI_WAIT_PRESS;
static uint32_t s_start_ms = 0U;
static bool s_emergency_event = false;

/*
 * Callback appele lorsqu'une interruption arrive.
 * Aucun traitement long dans cette fonction.
 */
void Exti_OnInterrupt(void)
{
    s_irq_pending = 1U;
}

/*
 * Machine a etats d'anti-rebond.
 * now_ms : temps courant en millisecondes.
 * button_high : niveau logique du bouton.
 */
void Exti_Process(uint32_t now_ms, bool button_high)
{
    switch (s_state)
    {
        case EXTI_WAIT_PRESS:

            if (s_irq_pending != 0U)
            {
                s_irq_pending = 0U;
                s_start_ms = now_ms;
                s_state = EXTI_VERIFY_PRESS;
            }
            break;

        case EXTI_VERIFY_PRESS:

            if ((uint32_t)(now_ms - s_start_ms)
                >= EXTI_DEBOUNCE_MS)
            {
                if (button_high)
                {
                    s_emergency_event = true;
                    s_state = EXTI_WAIT_RELEASE;
                }
                else
                {
                    s_state = EXTI_WAIT_PRESS;
                }
            }
            break;

        case EXTI_WAIT_RELEASE:

            if (!button_high)
            {
                s_start_ms = now_ms;
                s_state = EXTI_VERIFY_RELEASE;
            }
            break;

        case EXTI_VERIFY_RELEASE:

            if (button_high)
            {
                s_state = EXTI_WAIT_RELEASE;
            }
            else if ((uint32_t)(now_ms - s_start_ms)
                     >= EXTI_DEBOUNCE_MS)
            {
                s_irq_pending = 0U;
                s_state = EXTI_WAIT_PRESS;
            }
            break;

        default:

            s_state = EXTI_WAIT_PRESS;
            break;
    }
}

/*
 * Retourne true une seule fois par evenement.
 * Puis remet l'indicateur a false.
 */
bool Exti_TakeEmergencyEvent(void)
{
    bool event = s_emergency_event;
    s_emergency_event = false;

    return event;
}



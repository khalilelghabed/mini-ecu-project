
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "app/signals.h"

/* ============================================
   MACRO DE VERIFICATION
   ============================================ */

#define CHECK(c) do {                         \
    if (!(c)) {                               \
        printf("FAIL %s:%d: %s\n",             \
               __FILE__, __LINE__, #c);       \
        return 1;                             \
    }                                         \
} while (0)

/* ============================================
   TEST 1 : INITIALISATION
   ============================================ */

static int test_init(void)
{
    can_frame_t f;

    signals_init(&f);

    CHECK(f.id == 0x000);
    CHECK(f.dlc == 8);

    for (int i = 0; i < 8; i++) {
        CHECK(f.data[i] == 0);
    }

    return 0;
}

/* ============================================
   TEST 2 : VITESSE
   ============================================ */

static int test_speed(void)
{
    can_frame_t f;
    signals_init(&f);

    /* 1234 = 0x04D2 */
    signals_set_speed(&f, 1234);

    CHECK(f.data[0] == 0xD2);
    CHECK(f.data[1] == 0x04);

    CHECK(signals_get_speed(&f) == 1234);

    return 0;
}

/* ============================================
   TEST 3 : CONSIGNE
   ============================================ */

static int test_setpoint(void)
{
    can_frame_t f;
    signals_init(&f);

    /* 900 = 0x0384 */
    signals_set_setpoint(&f, 900);

    CHECK(f.data[2] == 0x84);
    CHECK(f.data[3] == 0x03);

    CHECK(signals_get_setpoint(&f) == 900);

    return 0;
}

/* ============================================
   TEST 4 : INDEPENDANCE DES SIGNAUX
   ============================================ */

static int test_independance(void)
{
    can_frame_t f;
    signals_init(&f);

    signals_set_speed(&f, 1234);
    signals_set_setpoint(&f, 900);

    CHECK(signals_get_speed(&f) == 1234);
    CHECK(signals_get_setpoint(&f) == 900);

    CHECK(f.data[0] == 0xD2);
    CHECK(f.data[1] == 0x04);
    CHECK(f.data[2] == 0x84);
    CHECK(f.data[3] == 0x03);

    /* Les autres octets restent inchanges */
    for (int i = 4; i < 8; i++) {
        CHECK(f.data[i] == 0);
    }

    return 0;
}

/* ============================================
   TEST 5 : TEMPERATURE NORMALE
   ============================================ */

static int test_temp_normale(void)
{
    can_frame_t f;
    signals_init(&f);

    /* 25 + 40 = 65 = 0x41 */
    bool ok = signals_set_temp(&f, 25);

    CHECK(ok == true);
    CHECK(f.data[4] == 0x41);
    CHECK(signals_get_temp(&f) == 25);

    return 0;
}

/* ============================================
   TEST 6 : LIMITES DE TEMPERATURE
   ============================================ */

static int test_temp_limites(void)
{
    can_frame_t f;
    signals_init(&f);

    /* Temperature minimale : -40 degres */
    CHECK(signals_set_temp(&f, -40) == true);
    CHECK(f.data[4] == 0x00);
    CHECK(signals_get_temp(&f) == -40);

    /* Temperature maximale : 215 degres */
    CHECK(signals_set_temp(&f, 215) == true);
    CHECK(f.data[4] == 0xFF);
    CHECK(signals_get_temp(&f) == 215);

    return 0;
}

/* ============================================
   TEST 7 : REFUS DES TEMPERATURES INVALIDES
   ============================================ */

static int test_temp_refus(void)
{
    can_frame_t f;
    signals_init(&f);

    CHECK(signals_set_temp(&f, 25) == true);

    uint8_t avant = f.data[4];

    CHECK(signals_set_temp(&f, -41) == false);
    CHECK(f.data[4] == avant);

    CHECK(signals_set_temp(&f, 216) == false);
    CHECK(f.data[4] == avant);

    CHECK(signals_set_temp(&f, 300) == false);
    CHECK(f.data[4] == avant);

    return 0;
}

/* ============================================
   TEST 8 : SET, CLEAR ET TOGGLE DES FLAGS
   ============================================ */

static int test_flags(void)
{
    can_frame_t f;
    signals_init(&f);

    /* Activation de la regulation : bit 0 */
    signals_set_flag(&f, SIG_FLAG_CRUISE);
    CHECK(f.data[5] == 0x01);

    /* Activation du defaut : bit 1 */
    signals_set_flag(&f, SIG_FLAG_FAULT);
    CHECK(f.data[5] == 0x03);

    /* Desactivation de la regulation */
    signals_clear_flag(&f, SIG_FLAG_CRUISE);
    CHECK(f.data[5] == 0x02);

    /* Activation du frein par toggle */
    signals_toggle_flag(&f, SIG_FLAG_BRAKE);
    CHECK(f.data[5] == 0x06);

    /* Deuxieme toggle : frein desactive */
    signals_toggle_flag(&f, SIG_FLAG_BRAKE);
    CHECK(f.data[5] == 0x02);

    return 0;
}

/* ============================================
   TEST 9 : LECTURE DES FLAGS
   ============================================ */

static int test_get_flag(void)
{
    can_frame_t f;
    signals_init(&f);

    signals_set_flag(&f, SIG_FLAG_FAULT);
    signals_set_flag(&f, SIG_FLAG_BRAKE);

    CHECK(f.data[5] == 0x06);

    CHECK(signals_get_flag(&f, SIG_FLAG_BRAKE) == true);
    CHECK(signals_get_flag(&f, SIG_FLAG_FAULT) == true);
    CHECK(signals_get_flag(&f, SIG_FLAG_CRUISE) == false);

    signals_toggle_flag(&f, SIG_FLAG_BRAKE);

    CHECK(signals_get_flag(&f, SIG_FLAG_BRAKE) == false);
    CHECK(signals_get_flag(&f, SIG_FLAG_FAULT) == true);

    return 0;
}

/* ============================================
   TEST 10 : POINTEURS NULL
   ============================================ */

static int test_null(void)
{
    /* Les fonctions void ne doivent pas planter */
    signals_init(NULL);
    signals_set_speed(NULL, 100);
    signals_set_setpoint(NULL, 200);

    signals_set_flag(NULL, SIG_FLAG_CRUISE);
    signals_clear_flag(NULL, SIG_FLAG_FAULT);
    signals_toggle_flag(NULL, SIG_FLAG_BRAKE);

    /* Les getters doivent retourner 0 */
    CHECK(signals_get_speed(NULL) == 0);
    CHECK(signals_get_setpoint(NULL) == 0);

    /* Convention actuelle : -40 si NULL */
    CHECK(signals_get_temp(NULL) == -40);

    CHECK(signals_set_temp(NULL, 25) == false);
    CHECK(signals_get_flag(NULL, SIG_FLAG_BRAKE) == false);

    return 0;
}

/* ============================================
   TEST 11 : VALEURS LIMITES DE VITESSE
   ============================================ */

static int test_speed_limites(void)
{
    can_frame_t f;
    signals_init(&f);

    /* Valeur minimale */
    signals_set_speed(&f, 0);

    CHECK(f.data[0] == 0x00);
    CHECK(f.data[1] == 0x00);
    CHECK(signals_get_speed(&f) == 0);

    /* Valeur maximale : 65535 = 0xFFFF */
    signals_set_speed(&f, 65535);

    CHECK(f.data[0] == 0xFF);
    CHECK(f.data[1] == 0xFF);
    CHECK(signals_get_speed(&f) == 65535);

    return 0;
}

/* ============================================
   EXECUTION DES TESTS
   ============================================ */

#define RUN_TEST(test) do {                    \
    printf("[TEST] %-25s ", #test);            \
    int result = test();                       \
    if (result == 0) {                         \
        printf("OK\n");                        \
    } else {                                   \
        printf("ECHEC\n");                     \
    }                                          \
    fails += result;                           \
} while (0)

int main(void)
{
    int fails = 0;

    printf("\n=== TESTS UNITAIRES MINI-ECU ===\n\n");

    RUN_TEST(test_init);
    RUN_TEST(test_speed);
    RUN_TEST(test_setpoint);
    RUN_TEST(test_independance);
    RUN_TEST(test_temp_normale);
    RUN_TEST(test_temp_limites);
    RUN_TEST(test_temp_refus);
    RUN_TEST(test_flags);
    RUN_TEST(test_get_flag);
    RUN_TEST(test_null);
    RUN_TEST(test_speed_limites);

    printf("\n===============================\n");

    if (fails == 0) {
        printf("SUCCES : tous les tests sont passes\n");
    } else {
        printf("ECHEC : %d test(s) en erreur\n", fails);
    }

    return fails;
}

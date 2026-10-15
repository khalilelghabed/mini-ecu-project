#include <stdio.h>
#include <stdint.h>

/* Variables globales */
int global_init = 100;
int global_zero;

/* Variables avec liaison interne */
static int static_init = 200;
static int static_zero;

/* Constante globale */
const uint32_t max_value = 500U;

void afficher(void)
{
    int local = 10;
    static int compteur = 0;

    compteur++;

    printf("local = %d\n", local);
    printf("compteur = %d\n", compteur);
}

int main(void)
{
    afficher();

    printf("global_init = %d\n", global_init);
    printf("global_zero = %d\n", global_zero);
    printf("static_init = %d\n", static_init);
    printf("static_zero = %d\n", static_zero);
    printf("max_value = %u\n", (unsigned int)max_value);

    return 0;
}

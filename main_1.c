/*
 *
 * C骴igo que conmuta los leds en tres frecuencias diferentes
 * usando interrupciones con prioridades y el systick.
 *
 */

#include <stdint.h>

// Definir las macros de punteros
#define SYSCTL_RCGCGPIO_R           (*((volatile uint32_t *)0x400FE608))            /*  pp382   GPIO Run Mode Clock Gating Control */
#define SYSCTL_PRGPIO_R             (*((volatile uint32_t *)0x400FEA08))            /*  pp499   GPIO Peripheral Ready */

//Entradas
// GPIO Port P (PortP)
// Registros puerto P (0x4006.5000)
#define GPIO_PORTP_DATA_R           (*((volatile uint32_t *)0x40065020))            //  PP3
#define GPIO_PORTP_DIR_R            (*((volatile uint32_t *)0x40065400))
#define GPIO_PORTP_AHB_IS_R         (*((volatile uint32_t *)0x40065404))            /*  pp761   GPIO Interrupt Sense */
#define GPIO_PORTP_AHB_IBE_R        (*((volatile uint32_t *)0x40065408))            /*  pp762   GPIO Interrupt Both Edges */
#define GPIO_PORTP_AHB_IEV_R        (*((volatile uint32_t *)0x4006540C))            /*  pp763   GPIO Interrupt Event */
#define GPIO_PORTP_AHB_IM_R         (*((volatile uint32_t *)0x40065410))            /*  pp764   GPIO Interrupt Mask */
#define GPIO_PORTP_AHB_ICR_R        (*((volatile uint32_t *)0x4006541C))            /*  pp769   GPIO Interrupt Clear */
#define GPIO_PORTP_AHB_PUR_R        (*((volatile uint32_t *)0x40065510))            /*  pp776   GPIO Pull-Up Select */
#define GPIO_PORTP_DEN_R            (*((volatile uint32_t *)0x4006551C))

// GPIO Port Q (PortQ)
// Registros puerto Q (0x4006.6000)
#define GPIO_PORTQ_DATA_R           (*((volatile uint32_t *)0x40066008))            // PQ1
#define GPIO_PORTQ_DIR_R            (*((volatile uint32_t *)0x40066400))
#define GPIO_PORTQ_AHB_IS_R         (*((volatile uint32_t *)0x40066404))            /*  pp761   GPIO Interrupt Sense */
#define GPIO_PORTQ_AHB_IBE_R        (*((volatile uint32_t *)0x40066408))            /*  pp762   GPIO Interrupt Both Edges */
#define GPIO_PORTQ_AHB_IEV_R        (*((volatile uint32_t *)0x4006640C))            /*  pp763   GPIO Interrupt Event */
#define GPIO_PORTQ_AHB_IM_R         (*((volatile uint32_t *)0x40066410))            /*  pp764   GPIO Interrupt Mask */
#define GPIO_PORTQ_AHB_ICR_R        (*((volatile uint32_t *)0x4006641C))            /*  pp769   GPIO Interrupt Clear */
#define GPIO_PORTQ_AHB_PUR_R        (*((volatile uint32_t *)0x40066510))            /*  pp776   GPIO Pull-Up Select */
#define GPIO_PORTQ_DEN_R            (*((volatile uint32_t *)0x4006651C))

// GPIO Port M (PortM)
// Registros puerto M (0x4006.3000)
#define GPIO_PORTM_DATA_R           (*((volatile uint32_t *)0x40063100))            // PM6
#define GPIO_PORTM_DIR_R            (*((volatile uint32_t *)0x40063400))
#define GPIO_PORTM_AHB_IS_R         (*((volatile uint32_t *)0x40063404))            /*  pp761   GPIO Interrupt Sense */
#define GPIO_PORTM_AHB_IBE_R        (*((volatile uint32_t *)0x40063408))            /*  pp762   GPIO Interrupt Both Edges */
#define GPIO_PORTM_AHB_IEV_R        (*((volatile uint32_t *)0x4006340C))            /*  pp763   GPIO Interrupt Event */
#define GPIO_PORTM_AHB_IM_R         (*((volatile uint32_t *)0x40063410))            /*  pp764   GPIO Interrupt Mask */
#define GPIO_PORTM_AHB_ICR_R        (*((volatile uint32_t *)0x4006341C))            /*  pp769   GPIO Interrupt Clear */
#define GPIO_PORTM_AHB_PUR_R        (*((volatile uint32_t *)0x40063510))            /*  pp776   GPIO Pull-Up Select */
#define GPIO_PORTM_DEN_R            (*((volatile uint32_t *)0x4006351C))

//Salidas
// GPIO Port N (PortN)
#define GPIO_PORTN_DATA_R           (*((volatile uint32_t *)0x4006400C))            /* pp759   GPIO Data */
#define GPIO_PORTN_DIR_R            (*((volatile uint32_t *)0x40064400))            /* pp760   GPIO Direction */
#define GPIO_PORTN_DEN_R            (*((volatile uint32_t *)0x4006451C))            /* pp781   GPIO Digital Enable */

// GPIO Port F (PortF)
#define GPIO_PORTF_AHB_DATA_R       (*((volatile uint32_t *)0x4005D044))            /*  pp759   GPIO Data >> PortF[4, 0] unmasked */
#define GPIO_PORTF_AHB_DIR_R        (*((volatile uint32_t *)0x4005D400))            /*  pp760   GPIO Direction */
#define GPIO_PORTF_AHB_DEN_R        (*((volatile uint32_t *)0x4005D51C))            /*  pp781   GPIO Digital Enable */

//Interrupciones
//Puerto P (Interrupci髇 79) --pp161
#define NVIC_EN2_R                  (*((volatile uint32_t *)0xE000E108))     //Habilita la interrupci髇 (64-95) -- pp154
//Esta habilitaci髇 sirve para todas nuestras interrupciones
#define NVIC_PRI19_R                (*((volatile uint32_t *)0xE000E44C))     //Interrupt 79 Priority -- pp161
//Puerto Q (Interrupci髇 85) --pp161
#define NVIC_PRI21_R                (*((volatile uint32_t *)0xE000E454))     //Interrupt 85 Priority -- pp161
//Puerto M (Interrupci髇 72) --pp161
#define NVIC_PRI18_R                (*((volatile uint32_t *)0xE000E448))     //Interrupt 72- Priority -- pp161

/*********************************************************************************
 * Macros auxiliares
 */

    // Bit fields in the NVIC_PRIn register                                            pp159
#define NVIC_PRI18_INT72_M          0x000000E0                                      /*  Interrupt 72 Priority mask */
#define NVIC_PRI19_INT79_M          0xE0000000                                      /*  Interrupt 79 Priority mask */
#define NVIC_PRI21_INT85_M          0x0000E000                                      /*  Interrupt 85 Priority mask */
#define NVIC_PRI18_INT72_S          5                                               /*  Interrupt 72 Priority shift */
#define NVIC_PRI19_INT79_S          29                                              /*  Interrupt 79 Priority shift */
#define NVIC_PRI21_INT85_S          13                                              /*  Interrupt 85 Priority shift */


    // Bit fields in the SYSCTL_RCGCGPIO register                                       pp382
#define SYSCTL_RCGCGPIO_R14         0x00004000                                      /*  GPIO Port Q Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R13         0x00002000                                      /*  GPIO Port P Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R12         0x00001000                                      /*  GPIO Port N Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R11         0x00000800                                      /*  GPIO Port M Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R10         0x00000400                                      /*  GPIO Port L Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R9          0x00000200                                      /*  GPIO Port K Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R8          0x00000100                                      /*  GPIO Port J Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R7          0x00000080                                      /*  GPIO Port H Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R6          0x00000040                                      /*  GPIO Port G Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R5          0x00000020                                      /*  GPIO Port F Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R4          0x00000010                                      /*  GPIO Port E Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R3          0x00000008                                      /*  GPIO Port D Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R2          0x00000004                                      /*  GPIO Port C Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R1          0x00000002                                      /*  GPIO Port B Run Mode Clock Gating Control */
#define SYSCTL_RCGCGPIO_R0          0x00000001                                      /*  GPIO Port A Run Mode Clock Gating Control */

    // Bit fields in the SYSCTL_PRGPIO register                                         pp499
#define SYSCTL_PRGPIO_R14           0x00004000                                      /*  GPIO Port Q Peripheral Ready */
#define SYSCTL_PRGPIO_R13           0x00002000                                      /*  GPIO Port P Peripheral Ready */
#define SYSCTL_PRGPIO_R12           0x00001000                                      /*  GPIO Port N Peripheral Ready */
#define SYSCTL_PRGPIO_R11           0x00000800                                      /*  GPIO Port M Peripheral Ready */
#define SYSCTL_PRGPIO_R10           0x00000400                                      /*  GPIO Port L Peripheral Ready */
#define SYSCTL_PRGPIO_R9            0x00000200                                      /*  GPIO Port K Peripheral Ready */
#define SYSCTL_PRGPIO_R8            0x00000100                                      /*  GPIO Port J Peripheral Ready */
#define SYSCTL_PRGPIO_R7            0x00000080                                      /*  GPIO Port H Peripheral Ready */
#define SYSCTL_PRGPIO_R6            0x00000040                                      /*  GPIO Port G Peripheral Ready */
#define SYSCTL_PRGPIO_R5            0x00000020                                      /*  GPIO Port F Peripheral Ready */
#define SYSCTL_PRGPIO_R4            0x00000010                                      /*  GPIO Port E Peripheral Ready */
#define SYSCTL_PRGPIO_R3            0x00000008                                      /*  GPIO Port D Peripheral Ready */
#define SYSCTL_PRGPIO_R2            0x00000004                                      /*  GPIO Port C Peripheral Ready */
#define SYSCTL_PRGPIO_R1            0x00000002                                      /*  GPIO Port B Peripheral Ready */
#define SYSCTL_PRGPIO_R0            0x00000001                                      /*  GPIO Port A Peripheral Ready */

    // Bit fields for the GPIO pin
#define GPIO_PIN_7                  0x00000080                                      /*  GPIO pin 7 */
#define GPIO_PIN_6                  0x00000040                                      /*  GPIO pin 6 */
#define GPIO_PIN_5                  0x00000020                                      /*  GPIO pin 5 */
#define GPIO_PIN_4                  0x00000010                                      /*  GPIO pin 4 */
#define GPIO_PIN_3                  0x00000008                                      /*  GPIO pin 3 */
#define GPIO_PIN_2                  0x00000004                                      /*  GPIO pin 2 */
#define GPIO_PIN_1                  0x00000002                                      /*  GPIO pin 1 */
#define GPIO_PIN_0                  0x00000001                                      /*  GPIO pin 0 */

void GPIO_PortP_Init(void) {
    /*  Paso 1: Habilitar la se帽al de reloj del GPIO (RCGCGPIO) y esperar a que se estabilice (PRGPIO) */
        SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R13;   /*  PortP => Habilitar y proveer de se帽al de reloj */
        while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R13)) {} /*  PortP => Esperar a que se estabilice la se帽al de reloj */

        /*  Paso 2: Configurar la direcci贸n del GPIO (GPIODIR) */
        GPIO_PORTP_DIR_R &= ~(GPIO_PIN_3); /*  PortP[3] => Data direction -> Input */

        /*  Paso 8: Configurar como Open Drain (GPIOODR) o las resistencias de Pull-Up (GPIOPUR) / Pull-Down (GPIOPDR) */
        GPIO_PORTP_AHB_PUR_R |= (GPIO_PIN_3); /*  PortP[3] => Pull-Up resistors -> Enabled */

        /*  Paso 9: Configurar las funciones digitales del GPIO (GPIODEN) */
        GPIO_PORTP_DEN_R |= (GPIO_PIN_3); /*  PortP[3] => Digital functions -> Enabled */

        /*  Paso 10: Para uso de interrupci贸n, configurar la sensibilidad (GPIOIS), el evento (GPIOIBE y GPIOIEV), limpiar la bandera de interrupci贸n (GPIOICR) y desenmascarar la interrupci贸n (GPIOIM) */
        GPIO_PORTP_AHB_IS_R &= ~(GPIO_PIN_3);        /*  PortP[3] => Interrupt sense -> Edge-sensitive */
        GPIO_PORTP_AHB_IBE_R &= ~(GPIO_PIN_3);       /*  PortP[3] => Interrupt both edges -> Controlled by the GPIOIEV register */
        GPIO_PORTP_AHB_IEV_R &= ~(GPIO_PIN_3);       /*  PortP[3] => Interrupt event -> Falling edge triggers an interrupt */
        GPIO_PORTP_AHB_ICR_R |= (GPIO_PIN_3);        /*  PortP[3] => Interrupt is cleared (recomendado) */
        GPIO_PORTP_AHB_IM_R |= (GPIO_PIN_3);         /*  PortP[3] => Interrupt mask -> Unmasked */

        //Configuraci髇 de la interrupci髇
        /*  Configurar el nivel de prioridad de la interrupci贸n (PRIn) */
        NVIC_PRI19_R = (NVIC_PRI19_R & ~NVIC_PRI19_INT79_M) | (2 << NVIC_PRI19_INT79_S);    /*  PortP => Interrupt priority -> 2 */

        /*  Habilitar la interrupci贸n (ENn) */
        NVIC_EN2_R |= (1 << (79 - 64));
}
 void GPIO_PortQ_Init(void) {
     /*  Paso 1: Habilitar la se帽al de reloj del GPIO (RCGCGPIO) y esperar a que se estabilice (PRGPIO) */
         SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R14;               /*  PortQ => Habilitar y proveer de se帽al de reloj */
         while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R14)) {}       /*  PortQ => Esperar a que se estabilice la se帽al de reloj */

         /*  Paso 2: Configurar la direcci贸n del GPIO (GPIODIR) */
         GPIO_PORTQ_DIR_R &= ~(GPIO_PIN_1);     /*  PortQ[1] => Data direction -> Input */

         /*  Paso 8: Configurar como Open Drain (GPIOODR) o las resistencias de Pull-Up (GPIOPUR) / Pull-Down (GPIOPDR) */
         GPIO_PORTQ_AHB_PUR_R |= (GPIO_PIN_1);  /*  PortQ[1] => Pull-Up resistors -> Enabled */

         /*  Paso 9: Configurar las funciones digitales del GPIO (GPIODEN) */
         GPIO_PORTQ_DEN_R |= (GPIO_PIN_1);  /*  PortQ[1] => Digital functions -> Enabled */

         /*  Paso 10: Para uso de interrupci贸n, configurar la sensibilidad (GPIOIS), el evento (GPIOIBE y GPIOIEV), limpiar la bandera de interrupci贸n (GPIOICR) y desenmascarar la interrupci贸n (GPIOIM) */
         GPIO_PORTQ_AHB_IS_R &= ~(GPIO_PIN_1);         /*  PortQ[1] => Interrupt sense -> Edge-sensitive */
         GPIO_PORTQ_AHB_IBE_R &= ~(GPIO_PIN_1);        /*  PortQ[1] => Interrupt both edges -> Controlled by the GPIOIEV register */
         GPIO_PORTQ_AHB_IEV_R &= ~(GPIO_PIN_1);        /*  PortQ[1] => Interrupt event -> Falling edge triggers an interrupt */
         GPIO_PORTQ_AHB_ICR_R |= (GPIO_PIN_1);         /*  PortQ[1] => Interrupt is cleared (recomendado) */
         GPIO_PORTQ_AHB_IM_R |= (GPIO_PIN_1);          /*  PortQ[1] => Interrupt mask -> Unmasked */

         //Confuguraci髇 de interrupci髇
         /*  Configurar el nivel de prioridad de la interrupci贸n (PRIn) */
         NVIC_PRI21_R = (NVIC_PRI21_R & ~NVIC_PRI21_INT85_M) | (1 << NVIC_PRI21_INT85_S);    /*  PortQ => Interrupt priority -> 1 */

         /*  Habilitar la interrupci贸n (ENn) */
         NVIC_EN2_R |= (1 << (85 - 64));
 }
 void GPIO_PortM_Init(void) {
     /*  Paso 1: Habilitar la se帽al de reloj del GPIO (RCGCGPIO) y esperar a que se estabilice (PRGPIO) */
         SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R11;       /*  PortM => Habilitar y proveer de se帽al de reloj */
         while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R11)) {}    /*  PortM => Esperar a que se estabilice la se帽al de reloj */

         /*  Paso 2: Configurar la direcci贸n del GPIO (GPIODIR) */
         GPIO_PORTM_DIR_R &= ~(GPIO_PIN_6);   /*  PortM[6] => Data direction -> Input */

         /*  Paso 8: Configurar como Open Drain (GPIOODR) o las resistencias de Pull-Up (GPIOPUR) / Pull-Down (GPIOPDR) */
         GPIO_PORTM_AHB_PUR_R |= (GPIO_PIN_6);   /*  PortM[6] => Pull-Up resistors -> Enabled */

         /*  Paso 9: Configurar las funciones digitales del GPIO (GPIODEN) */
         GPIO_PORTM_DEN_R |= (GPIO_PIN_6);    /*  PortM[6] => Digital functions -> Enabled */

         /*  Paso 10: Para uso de interrupci贸n, configurar la sensibilidad (GPIOIS), el evento (GPIOIBE y GPIOIEV), limpiar la bandera de interrupci贸n (GPIOICR) y desenmascarar la interrupci贸n (GPIOIM) */
         GPIO_PORTM_AHB_IS_R &= ~(GPIO_PIN_6);        /*  PortM[6] => Interrupt sense -> Edge-sensitive */
         GPIO_PORTM_AHB_IBE_R &= ~(GPIO_PIN_6);       /*  PortM[6] => Interrupt both edges -> Controlled by the GPIOIEV register */
         GPIO_PORTM_AHB_IEV_R &= ~(GPIO_PIN_6);       /*  PortM[6] => Interrupt event -> Falling edge triggers an interrupt */
         GPIO_PORTM_AHB_ICR_R |= (GPIO_PIN_6);        /*  PortM[6] => Interrupt is cleared (recomendado) */
         GPIO_PORTM_AHB_IM_R |= (GPIO_PIN_6);         /*  PortM[6] => Interrupt mask -> Unmasked */

         //Configuraci髇 de la interrupcion
         /*  Configurar el nivel de prioridad de la interrupci贸n (PRIn) */
         NVIC_PRI18_R = (NVIC_PRI18_R & ~NVIC_PRI18_INT72_M) | (0 << NVIC_PRI18_INT72_S);    /*  PortM => Interrupt priority -> 0 */

         /*  Habilitar la interrupci贸n (ENn) */
         NVIC_EN2_R |= (1 << (72 - 64));
 }
void GPIO_PortN_Init(void) {

    /*  Paso 1: Habilitar la se帽al de reloj del GPIO (RCGCGPIO) y esperar a que se estabilice (PRGPIO) */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;                                       /*  PortN => Habilitar y proveer de se帽al de reloj */
    while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12)) {}                               /*  PortN => Esperar a que se estabilice la se帽al de reloj */

    /*  Paso 2: Configurar la direcci贸n del GPIO (GPIODIR) */
    GPIO_PORTN_DIR_R |= (GPIO_PIN_1 | GPIO_PIN_0);                                                 /*  PortN[1,0] => Data direction -> Output */

    /*  Paso 9: Configurar las funciones digitales del GPIO (GPIODEN) */
    GPIO_PORTN_DEN_R |= (GPIO_PIN_1 | GPIO_PIN_0);                                                 /*  PortN[1,0] => Digital functions -> Enabled */
}

 void GPIO_PortF_Init(void) {
     /*  Paso 1: Habilitar la se帽al de reloj del GPIO (RCGCGPIO) y esperar a que se estabilice (PRGPIO) */
     SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;                                        /*  PortF => Habilitar y proveer de se帽al de reloj */
     while (!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)) {}                                /*  PortF => Esperar a que se estabilice la se帽al de reloj */

     /*  Paso 2: Configurar la direcci贸n del GPIO (GPIODIR) */
     GPIO_PORTF_AHB_DIR_R |= (GPIO_PIN_4 | GPIO_PIN_0);                              /*  PortF[4,0] => Data direction -> Output */

     /*  Paso 9: Configurar las funciones digitales del GPIO (GPIODEN) */
     GPIO_PORTF_AHB_DEN_R |= (GPIO_PIN_4 | GPIO_PIN_0);                              /*  PortF[4,0] => Digital functions -> Enabled */
 }

//Control de los leds
 #define LED_D1_Toggle()             (GPIO_PORTN_DATA_R ^= GPIO_PIN_1)               /*  Xor del LED D1 (PortN[1]) */
 #define LED_D2_Toggle()             (GPIO_PORTN_DATA_R ^= GPIO_PIN_0)               /*  Xor del LED D2 (PortN[0]) */
 #define LED_D3_Toggle()             (GPIO_PORTF_AHB_DATA_R ^= GPIO_PIN_4)           /*  Xor del LED D3 (PortF[4]) */
 #define LED_D4_Toggle()             (GPIO_PORTF_AHB_DATA_R ^= GPIO_PIN_0)           /*  Xor del LED D4 (PortF[0]) */
 //Auxiliares para apagar y prender los leds
#define LED_D12_On()             (GPIO_PORTN_DATA_R |= (GPIO_PIN_1 | GPIO_PIN_0))
#define LED_D34_On()             (GPIO_PORTF_AHB_DATA_R |= (GPIO_PIN_4 | GPIO_PIN_0))
#define LED_D12_Off()            (GPIO_PORTN_DATA_R &= ~(GPIO_PIN_1 | GPIO_PIN_0))
#define LED_D34_Off()            (GPIO_PORTF_AHB_DATA_R &= ~(GPIO_PIN_4 | GPIO_PIN_0))

 int a=0,b=0,c=0,i=0,LedAux12=0,LedAux34=0,LedAux12_b=0,LedAux34_b=0;

 void GPIO_PortP_Handler(void) {
     LED_D12_Off(); //Se apagan los leds 1 y 2
     LED_D34_Off(); //Se apagan los leds 3 y 4
     while(c<4){
         LED_D1_Toggle();                    /* Conmutaci髇 del LED D1 */
         for (i = 0; i < 1300000; i++) {}     /* Retardo aproximado de 150ms */

         LED_D2_Toggle();                    /* Conmutaci髇 del LED D2 */
         for (i = 0; i < 1300000; i++) {}

         LED_D3_Toggle();                    /* Conmutaci髇 del LED D3 */
         for (i = 0; i < 1300000; i++) {}

         LED_D4_Toggle();                    /* Conmutaci髇 del LED D4 */
         for (i = 0; i < 1300000; i++) {}
         c=c+1;
     }
     GPIO_PORTP_AHB_ICR_R |= (GPIO_PIN_3);                              /*  PortP[3] => Interrupt is cleared */
     c=0;
 }
 void GPIO_PortQ_Handler(void) {
     LedAux12_b=GPIO_PORTN_DATA_R; //Guardamos los estados de los leds al momento de la interrupcion
     LedAux34_b=GPIO_PORTF_AHB_DATA_R; //Estados de los leds antes de la interrupci髇
     LED_D12_Off(); //Se apagan los leds 1 y 2
     LED_D34_Off(); //Se apagan los leds 3 y 4
     while(b<8){
         LED_D4_Toggle();                    /* Conmutaci髇 del LED D4 */
         for (i = 0; i < 700000; i++) {}     /* Retardo aproximado de 500ms */

         LED_D3_Toggle();                    /* Conmutaci髇 del LED D3 */
         for (i = 0; i < 700000; i++) {}

         LED_D2_Toggle();                    /* Conmutaci髇 del LED D2 */
         for (i = 0; i < 700000; i++) {}

         LED_D1_Toggle();                    /* Conmutaci髇 del LED D1 */
         for (i = 0; i < 700000; i++) {}
         b=b+1;
     }
     /*  Limpiar la bandera de interrupci贸n */
     GPIO_PORTQ_AHB_ICR_R |= (GPIO_PIN_1);                              /*  PortQ[1] => Interrupt is cleared */
     GPIO_PORTN_DATA_R=LedAux12_b; //Reestablecemos los estados de los leds al momento de la interrupcion
     GPIO_PORTF_AHB_DATA_R=LedAux34_b;
     b=0;//Restablece la cuenta
  }
 void GPIO_PortM_Handler(void) {
     LedAux12=GPIO_PORTN_DATA_R; //Guardamos los estados de los leds al momento de la interrupcion
     LedAux34=GPIO_PORTF_AHB_DATA_R; //Estados de los leds antes de la interrupci髇
     LED_D12_Off(); //Se apagan los leds 1 y 2
     LED_D34_Off(); //Se apagan los leds 3 y 4
     while(a<16){
         LED_D1_Toggle();                    /* Conmutaci髇 del LED D1 */
         for (i = 0; i < 210000; i++) {}     /* Retardo aproximado de 150ms */

         LED_D2_Toggle();                    /* Conmutaci髇 del LED D2 */
         for (i = 0; i < 210000; i++) {}

         LED_D3_Toggle();                    /* Conmutaci髇 del LED D3 */
         for (i = 0; i < 210000; i++) {}

         LED_D4_Toggle();                    /* Conmutaci髇 del LED D4 */
         for (i = 0; i < 210000; i++) {}
         a=a+1;
     }
     /*  Limpiar la bandera de interrupci贸n */
     GPIO_PORTM_AHB_ICR_R |= (GPIO_PIN_6);                              /*  PortM[6] => Interrupt is cleared */
     GPIO_PORTN_DATA_R=LedAux12; //Reestablecemos los estados de los leds al momento de la interrupcion
     GPIO_PORTF_AHB_DATA_R=LedAux34;
     a=0;//Restablece la cuenta
  }

int main(void) {
    GPIO_PortP_Init();                                                              /*  Inicializaci贸n del GPIO PortP */
    GPIO_PortQ_Init();                                                              /*  Inicializaci贸n del GPIO PortQ */
    GPIO_PortM_Init();                                                              /*  Inicializaci贸n del GPIO PortM */
    GPIO_PortF_Init();                                                              /*  Inicializaci贸n del GPIO PortF */
    GPIO_PortN_Init();                                                              /*  Inicializaci贸n del GPIO PortN */

    while(1) {
    LED_D12_On(); //Se encienden los Leds 1 y 2
    LED_D34_On(); //Se encienden los leds 3 y 4
    for (i = 0; i < 1300000; i++) {}       // Retardo aproximado de 1s
    LED_D12_Off(); //Se apagan los leds 1 y 2
    LED_D34_Off(); //Se apagan los leds 3 y 4
    for (i = 0; i < 1300000; i++) {}       // Retardo aproximado de 1s
    }
}

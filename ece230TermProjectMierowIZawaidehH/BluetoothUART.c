/*
 * BluetoothUart.c
 *
 *  Created on: Feb 17, 2024
 *      Author: zawaidhm
 */


//************************************************
//TODO - sourced from exercise 7x01, make sure this works
void configureCommunication(void);
void configureCommunication(void){
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    /* Configure MCLK/SMCLK source to DCO, with DCO = 12MHz */
    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;

    //configure UART Pins
      UART_Port->SEL0 |= RXD_Pin | TXD_Pin;                // set 2-UART pins as secondary function
      UART_Port->SEL1 &= ~(RXD_Pin | TXD_Pin);

      /* Configure UART
       *  Asynchronous UART mode, 8N1 (8-bit data, no parity, 1 stop bit),
       *  LSB first, SMCLK clock source
       */
      EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
      EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
              EUSCI_A_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
      /* Baud Rate calculation
          * Refer to Section 24.3.10 of Technical Reference manual
          * BRCLK = 12000000, Baud rate = 9600
          * N = fBRCLK / Baud rate = 12000000/9600 = 1250
          * from Technical Reference manual Table 24-5:
          *
          * TODO lookup values for UCOS16 =1, UCBRx=78, UCBRFx=2, and UCBRSx=0 in Table 24-5
          */
         // TODO set clock prescaler in EUSCI_A2 baud rate control register
         EUSCI_A2->BRW=78;
         // TODO configure baud clock modulation in EUSCI_A2 modulation control register
         EUSCI_A2->MCTLW=(0<<8)+(2<<4)+1;


         EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;    // Initialize eUSCI
         EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;        // Clear eUSCI RX interrupt flag
         EUSCI_A2->IE |= EUSCI_A_IE_RXIE;            // Enable USCI_A2 RX interrupt

         // Enable global interrupt
         __enable_irq();


    // Enable eUSCIA2 interrupt in NVIC module
    NVIC->ISER[0] = (1 << EUSCIA2_IRQn );
}


void EUSCIA2_IRQHandler(void){
    // Check if receive flag is set (value ready in RX buffer)
       if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG){
           // Note that reading RX buffer clears the flag
            uint8_t digit = EUSCI_A2->RXBUF;
            digitsReceived++;
            // Echo character back to screen, otherwise user will not be able to
            //  verify what was typed
            while(!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG)); // Wait for TX buffer ready
            EUSCI_A2->TXBUF = digit;                 // Echo character to terminal

       }
}
//*****************************************************

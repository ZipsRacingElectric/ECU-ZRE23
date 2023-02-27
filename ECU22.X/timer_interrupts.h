// Timer Interrupts
//   Author: Derek Dunn
//   Created: 22.02.18
//   Updated: 23.02.16

#ifndef TIMER_INTERRUPTS_H
#define	TIMER_INTERRUPTS_H

#ifdef	__cplusplus
extern "C" {
#endif

// Initialize Timer Interrupts
// - Call to Initialize the Timer Services
// - Initializes Interrupts and Starts Services
void initialize_timer_interrupts();

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_INTERRUPTS_H */


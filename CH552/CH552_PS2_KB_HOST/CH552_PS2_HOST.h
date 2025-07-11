#ifndef _CH552_PS2_HOST_H_
#define _CH552_PS2_HOST_H_

// HINT: For performance reasons this library controls some pins directly. Due to the way the GPIO library works it can overwrite the changes made by this library.
// Do not use the GPIO library to drive any port containing a pin controlled by this library, or YOU WILL REGRET IT!
// Some more details: The pins controlled by this library include INT0 (if port 0 is enabled) and INT1 (if port 1 is enabled), as well as the pins specified below.
// The GPIO library overwrites pin changes made by this library because it needs to keep track of the last value written to a port, and it can only do that if 
// the value is changed by calling a GPIO library function. Keeping track of the previous written value is needed because the output registers are not readable.

// HINT: user options
#define PS2_USE_PORT_0 1
#define PS2_USE_PORT_1 0
#define PS2_P0_USE_BENCHMARK_PIN 1
#define PS2_P0_ISR_BENCHMARK_PIN TIN2

// HINT: The CLK pin for port 0 is always INT0, and the CLK pin for port 1 is always INT1
#define PS2_P0_DATA_PIN	MISO
#define PS2_P1_DATA_PIN	AIN3

// HINT: do not change any defines below this line
#define PS2_S_RX_ENABLED	0
#define PS2_S_COM_DISABLED	1
#define PS2_S_TX_REQUEST	2
#define PS2_S_PORT_DISABLED	3

#if PS2_USE_PORT_0
extern volatile UINT8 ps2_p0_rx_enabled;
extern volatile UINT8 ps2_p0_tx_enabled;
extern volatile UINT8 ps2_p0_shift;
extern volatile UINT8 ps2_p0_rx_error;
extern volatile UINT8 ps2_p0_tx_error;

extern void (*ps2_p0_rx_callback)(void);
extern void (*ps2_p0_tx_callback)(void);

void ps2_host_p0_init(void);
void ps2_p0_set_line_state(UINT8 line_state);
#endif

#if PS2_USE_PORT_1
extern volatile UINT8 ps2_p1_rx_enabled;
extern volatile UINT8 ps2_p1_tx_enabled;
extern volatile UINT8 ps2_p1_shift;
extern volatile UINT8 ps2_p1_rx_error;
extern volatile UINT8 ps2_p1_tx_error;

extern void (*ps2_p1_rx_callback)(void);
extern void (*ps2_p1_tx_callback)(void);

void ps2_host_p1_init(void);
void ps2_p1_set_line_state(UINT8 line_state);
#endif

#endif

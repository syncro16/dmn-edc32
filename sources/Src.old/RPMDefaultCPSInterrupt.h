/* these variables are defined in main.c and updated by interrupt routine */
extern volatile uint16_t rpm_duration;
extern volatile uint8_t rpm_overflow;
extern volatile uint16_t rpm_injection_trigger;
extern volatile uint32_t trigger_last_hit;

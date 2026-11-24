#ifndef DRIVERS_H
#define DRIVERS_H

#define DOOR_HOLD_MS 5000 // Duration to keep the door open in milliseconds
void screen_params_update(bool door_status, bool ambient_ok, bool user_present);
//void init_all(void);


#endif // DRIVERS_H
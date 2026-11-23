#ifndef SERVO_H
#define SERVO_H

#define SERVO_GPIO       7
#define SERVO_CLOSED_US  1000
#define SERVO_OPEN_US    2000

#ifdef __cplusplus
extern "C" {
#endif

void servo_init(void);
void servo_open(void);
void servo_close(void);

#ifdef __cplusplus
}
#endif

#endif
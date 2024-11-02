#ifndef TELEMETRY_H
#define TELEMETRY_h

#define GPIO_COUNT              26
#define TELEMETRY_HDR           0xF1
#define TELEMETRY_TLR           0x1F

const std::uint8_t GPIO_STATUS_LEN  = 30;
const std::uint8_t TIME_STATUS_LEN  = 8;
const std::uint8_t MOTOR_STATUS_LEN = 8;

typedef enum : std::uint8_t 
{
    GPIO_STATUS  = 0x00,
    TIME_STATUS  = 0x01,
    MOTOR_STATUS = 0x02,
    PWM_STATUS   = 0x03
} TELEMETRY_ID;

typedef enum : std::uint8_t
{
    OFF = 0x00,
    IDLE = 0x01,
    FORWARD = 0x02,
    REVERSE = 0x03
} MOTOR_RUNNING_STATE;

typedef struct __attribute__((packed))
{
    std::uint8_t header = TELEMETRY_HDR;
    std::uint8_t len = GPIO_STATUS_LEN;
    TELEMETRY_ID id = GPIO_STATUS;
    std::uint8_t gpio_status[GPIO_COUNT];
    std::uint8_t tlr = TELEMETRY_TLR; 
} GPIOStatus;

typedef struct __attribute__((packed))
{
    std::uint8_t header = TELEMETRY_HDR;
    std::uint8_t len = TIME_STATUS_LEN; 
    TELEMETRY_ID id = TIME_STATUS;
    std::uint32_t t_seconds = 0;
    std::uint8_t tlr = TELEMETRY_TLR;
} TimeStatus;

// TODO: Add speed/RPM calculations in here
typedef struct __attribute__((packed))
{
    std::uint8_t header = TELEMETRY_HDR;
    std::uint8_t len = MOTOR_STATUS_LEN;
    TELEMETRY_ID id = MOTOR_STATUS;
    bool motor_a_on = false;
    MOTOR_RUNNING_STATE motor_a_state = IDLE;
    bool motor_b_on = false;
    MOTOR_RUNNING_STATE motor_b_state = IDLE;
    std::uint8_t tlr = TELEMETRY_TLR;
} MotorStatus;

#endif

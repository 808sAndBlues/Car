#ifndef TELEMETRY_H
#define TELEMETRY_h

#define GPIO_COUNT              26
#define TELEMETRY_HDR           0xF1
#define TELEMETRY_TLR           0x1F

const std::uint8_t GPIO_STATUS_LEN = 30;
const std::uint8_t TIME_STATUS_LEN = 9;

typedef enum : std::uint8_t 
{
    GPIO_STATUS = 0x00,
    TIME_STATUS = 0x01
} TELEMETRY_ID;

typedef struct
{
    std::uint8_t header = TELEMETRY_HDR;
    std::uint8_t len = GPIO_STATUS_LEN;
    TELEMETRY_ID id = GPIO_STATUS;
    std::uint8_t gpio_status[GPIO_COUNT];
    std::uint8_t tlr = TELEMETRY_TLR; 
} GPIOStatus;

typedef struct
{
    std::uint8_t header = TELEMETRY_HDR;
    std::uint8_t len = TIME_STATUS_LEN; 
    TELEMETRY_ID id = TIME_STATUS;
    std::uint32_t t_seconds = 0;
    std::uint8_t tlr = TELEMETRY_TLR;
} TimeStatus;

#endif

#ifndef TELEMETRY_H
#define TELEMETRY_h

#define GPIO_COUNT              26
#define TELEMETRY_HDR           0xF1
#define TELEMETRY_TLR           0x1F

const std::uint8_t GPIO_STATUS_LEN = 30;

typedef enum : std::uint8_t 
{
    GPIO_STATUS = 0x00
} TELEMETRY_ID;

typedef struct
{
    std::uint8_t header = TELEMETRY_HDR;
    std::uint8_t len = GPIO_STATUS_LEN;
    TELEMETRY_ID id = GPIO_STATUS;
    std::uint8_t gpio_status[GPIO_COUNT];
    std::uint8_t tlr = TELEMETRY_TLR; 
} GPIOStatus;

#endif

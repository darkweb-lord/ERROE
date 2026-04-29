# MCP79412 Firmware Development Documentation

## Scope
This document is a firmware-focused reference for the Microchip MCP79412 battery-backed I2C real-time clock/calendar device, intended for implementation with the PIC24FV32KA304 MCU and similar Microchip controllers [web:35][web:50]. The MCP79412 combines RTCC registers, 64 bytes of battery-backed SRAM, 1 Kbit EEPROM, and a protected 64-bit EUI-64 area accessed over an I2C-compatible two-wire interface [web:4][web:34].

## Device overview
The MCP79412 is part of the MCP7941X family and provides calendar timekeeping with battery switchover so time can continue running when the main supply is removed [web:35][web:34]. The device exposes one I2C address space for RTCC registers and SRAM and a second I2C address space for EEPROM and protected ID access [web:4].

For firmware work, the device can be treated as four logical blocks [web:4]:
- RTCC timekeeping and control registers at 0x00 to 0x1F [web:4].
- Battery-backed user SRAM at 0x20 to 0x5F [web:4].
- EEPROM array at 0x00 to 0x7F through the EEPROM slave address space [web:4].
- Protected unique ID area at 0xF0 to 0xF7 through the EEPROM slave address space [web:4][web:44].

## Why this IC is used
This IC is useful when firmware must keep date and time across resets or power loss, store a small amount of retained data, log power-fail events, or retrieve a factory-programmed unique EUI-64 identifier [web:34][web:4]. In embedded products, common use cases include event timestamping, maintenance logging, backup counters, and deriving a globally unique network or product identity [web:34][web:44].

## Hardware requirements
A working MCP79412 design needs the primary supply on VCC, ground on VSS, SDA and SCL connected to the host I2C bus, and a 32.768 kHz watch crystal connected across X1 and X2 so the RTCC oscillator can run [web:4]. If time retention during main power loss is required, a backup source such as a coin cell must be connected to VBAT and battery backup must also be enabled in firmware through the VBATEN bit [web:4].

The standard support components are listed below [web:4][web:45]:
- Pull-up resistor on SDA.
- Pull-up resistor on SCL.
- 32.768 kHz crystal with load characteristics matched to the board design.
- Optional pull-up on MFP if alarm or square-wave output is used.
- Optional backup battery on VBAT for retention during VCC loss.

## PIC24FV32KA304 firmware context
The PIC24FV32KA304 family includes an I2C peripheral with dedicated control, status, receive, transmit, address, mask, and baud-rate registers, and it supports standard 100 kHz and 400 kHz bus operation [web:45][web:50]. For MCP79412 firmware, the MCU normally acts as the I2C master and uses Start, Repeated Start, byte transmit, ACK/NACK receive control, and Stop operations to move register data [web:45].

A practical driver on PIC24FV32KA304 typically contains these layers [web:45]:
- I2C initialization.
- Low-level Start, Restart, Stop, write-byte, and read-byte primitives.
- MCP79412 register read and write helpers.
- BCD conversion helpers.
- High-level functions for set time, get time, alarm configuration, SRAM access, EEPROM access, and EUI-64 read.

## I2C slave addresses
The MCP79412 uses two fixed I2C control code groups [web:4]. The RTCC and SRAM block uses control byte pattern `1101111x`, while the EEPROM and protected ID block uses `1010111x` [web:4].

In 8-bit address form used by many bare-metal drivers, the addresses are [web:4]:
- RTCC/SRAM write: `0xDE` [web:4].
- RTCC/SRAM read: `0xDF` [web:4].
- EEPROM/ID write: `0xAE` [web:4].
- EEPROM/ID read: `0xAF` [web:4].

In 7-bit form used by some software stacks, the corresponding values are [web:4]:
- RTCC/SRAM: `0x6F` [web:4].
- EEPROM/ID: `0x57` [web:4].

## Memory and register map

### RTCC register block
The RTCC register block occupies addresses 0x00 to 0x1F in the RTCC/SRAM space [web:4]. These registers include current time, control, oscillator trim, unlock, alarms, and power-fail timestamp registers [web:4][web:44].

| Address | Register | Main purpose |
|---|---|---|
| 0x00 | RTCSEC | Seconds register and ST oscillator start bit [web:4] |
| 0x01 | RTCMIN | Minutes register [web:4] |
| 0x02 | RTCHOUR | Hours register, 12/24 hour control [web:4] |
| 0x03 | RTCWKDAY | Weekday, OSCRUN, PWRFAIL, VBATEN [web:4] |
| 0x04 | RTCDATE | Day-of-month [web:4] |
| 0x05 | RTCMTH | Month and leap-year flag [web:4] |
| 0x06 | RTCYEAR | Year value 00 to 99 [web:4] |
| 0x07 | CONTROL | Alarm, square-wave, output control [web:4][web:44] |
| 0x08 | OSCTRIM | Oscillator digital trimming [web:4] |
| 0x09 | EEUNLOCK | Protected ID unlock sequencing [web:44] |
| 0x0A | ALM0SEC | Alarm 0 seconds [web:44] |
| 0x0B | ALM0MIN | Alarm 0 minutes [web:44] |
| 0x0C | ALM0HOUR | Alarm 0 hours [web:44] |
| 0x0D | ALM0WKDAY | Alarm 0 mask, flag, polarity [web:44] |
| 0x0E | ALM0DATE | Alarm 0 date [web:44] |
| 0x0F | ALM0MTH | Alarm 0 month [web:44] |
| 0x10 | Reserved/Alarm area | Family map continuation; check part datasheet when used [web:44] |
| 0x11 | ALM1SEC | Alarm 1 seconds [web:44] |
| 0x12 | ALM1MIN | Alarm 1 minutes [web:44] |
| 0x13 | ALM1HOUR | Alarm 1 hours [web:44] |
| 0x14 | ALM1WKDAY | Alarm 1 mask, flag, polarity/match bits [web:44] |
| 0x15 | ALM1DATE | Alarm 1 date [web:44] |
| 0x16 | ALM1MTH | Alarm 1 month [web:44] |
| 0x18 | PWRDNMIN | Power-down minutes timestamp [web:44] |
| 0x19 | PWRDNHOUR | Power-down hours timestamp [web:44] |
| 0x1A | PWRDNDATE | Power-down date timestamp [web:44] |
| 0x1B | PWRDNMTH | Power-down month and weekday timestamp [web:44] |
| 0x1C | PWRUPMIN | Power-up minutes timestamp [web:44] |
| 0x1D | PWRUPHOUR | Power-up hours timestamp [web:44] |
| 0x1E | PWRUPDATE | Power-up date timestamp [web:44] |
| 0x1F | PWRUPMTH | Power-up month and weekday timestamp [web:44] |

### SRAM block
The battery-backed SRAM occupies addresses 0x20 to 0x5F and is accessed through the RTCC/SRAM I2C address space [web:4]. This area is suitable for small retained values such as boot counters, product state, or quick nonvolatile-like scratch data that must survive VCC loss while VBAT is present [web:4].

### EEPROM block
The EEPROM array occupies addresses 0x00 to 0x7F in the EEPROM address space and supports normal nonvolatile data storage separate from the SRAM block [web:4][web:44]. The datasheet notes array protection options for none, upper quarter, upper half, or all of the array through protection control bits [web:44].

### Protected EUI-64 block
The unique ID location is stored at EEPROM addresses 0xF0 to 0xF7 and can be read at any time, while writes are blocked unless a defined unlock sequence is executed [web:44]. The MCP79412 variant is the family member that carries a factory-programmed EUI-64 MAC address in this protected region [web:34][web:35].

## Time data format
The calendar and time values are stored as BCD, not pure binary, so firmware must convert decimal values before writing and after reading [web:4]. For example, decimal 45 is stored as hexadecimal 0x45 in BCD representation, not 0x2D [web:4].

Useful helper functions are shown below.

```c
static uint8_t dec_to_bcd(uint8_t d)
{
    return (uint8_t)(((d / 10U) << 4) | (d % 10U));
}

static uint8_t bcd_to_dec(uint8_t b)
{
    return (uint8_t)(((b >> 4) * 10U) + (b & 0x0FU));
}
```

## Critical control bits
A few bits are essential for successful firmware bring-up [web:4][web:44]:
- `ST` in register 0x00 bit 7 starts the oscillator when set [web:4].
- `OSCRUN` in register 0x03 bit 5 indicates the oscillator is running [web:4].
- `PWRFAIL` in register 0x03 bit 4 indicates a power-fail event was detected [web:4].
- `VBATEN` in register 0x03 bit 3 enables battery backup operation [web:4].
- `ALM0EN` and `ALM1EN` in CONTROL register enable alarms [web:44].
- `SQWEN` in CONTROL register enables square-wave output on MFP [web:44].

## Step-by-step bring-up sequence
This is the safest basic initialization flow for firmware [web:4]:
1. Initialize the PIC24 I2C peripheral for standard or fast-mode bus timing [web:45].
2. Confirm the MCP79412 acknowledges the RTCC/SRAM device address [web:4].
3. Stop the oscillator by clearing `ST` before writing a full new date/time set [web:4].
4. Program seconds, minutes, hours, weekday, date, month, and year in BCD [web:4].
5. Set `VBATEN` in the weekday register if battery backup is used [web:4].
6. Restart the oscillator by setting `ST` in the seconds register [web:4].
7. Read back `OSCRUN` to confirm the oscillator has started [web:4].

## PIC24FV32KA304 bare-metal I2C template
The MCU-side I2C module exposes the operations needed for all MCP79412 transactions through the I2C control and status registers defined in the PIC24 family reference material [web:45]. The following code is a generic bare-metal example structure for PIC24FV32KA304-style firmware and should be adapted to the exact oscillator frequency and pin mapping used in the project [web:45][web:50].

```c
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define FCY 16000000UL
#define I2C_BRG_100KHZ 157U

static void i2c1_wait_idle(void)
{
    while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN ||
           I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT)
    {
    }
}

void i2c1_init(void)
{
    I2C1CON = 0;
    I2C1BRG = I2C_BRG_100KHZ;
    I2C1CONbits.I2CEN = 1;
}

bool i2c1_start(void)
{
    i2c1_wait_idle();
    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN);
    return true;
}

bool i2c1_restart(void)
{
    i2c1_wait_idle();
    I2C1CONbits.RSEN = 1;
    while (I2C1CONbits.RSEN);
    return true;
}

bool i2c1_stop(void)
{
    i2c1_wait_idle();
    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN);
    return true;
}

bool i2c1_write(uint8_t data)
{
    i2c1_wait_idle();
    I2C1TRN = data;
    while (I2C1STATbits.TRSTAT);
    return (I2C1STATbits.ACKSTAT == 0U);
}

uint8_t i2c1_read(bool ack)
{
    uint8_t data;
    i2c1_wait_idle();
    I2C1CONbits.RCEN = 1;
    while (!I2C1STATbits.RBF);
    data = I2C1RCV;
    i2c1_wait_idle();
    I2C1CONbits.ACKDT = ack ? 0U : 1U;
    I2C1CONbits.ACKEN = 1;
    while (I2C1CONbits.ACKEN);
    return data;
}
```

## MCP79412 low-level definitions

```c
#define MCP79412_RTCC_ADDR_7B   0x6FU
#define MCP79412_EE_ADDR_7B     0x57U

#define MCP79412_RTCC_WR        0xDEU
#define MCP79412_RTCC_RD        0xDFU
#define MCP79412_EE_WR          0xAEU
#define MCP79412_EE_RD          0xAFU

#define MCP79412_REG_RTCSEC     0x00U
#define MCP79412_REG_RTCMIN     0x01U
#define MCP79412_REG_RTCHOUR    0x02U
#define MCP79412_REG_RTCWKDAY   0x03U
#define MCP79412_REG_RTCDATE    0x04U
#define MCP79412_REG_RTCMTH     0x05U
#define MCP79412_REG_RTCYEAR    0x06U
#define MCP79412_REG_CONTROL    0x07U
#define MCP79412_REG_OSCTRIM    0x08U
#define MCP79412_REG_EEUNLOCK   0x09U

#define MCP79412_BIT_ST         0x80U
#define MCP79412_BIT_VBATEN     0x08U
#define MCP79412_BIT_OSCRUN     0x20U
#define MCP79412_BIT_PWRFAIL    0x10U

typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t wkday;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} mcp79412_time_t;
```

## Generic register access functions

```c
bool mcp79412_write_reg(uint8_t reg, uint8_t value)
{
    if (!i2c1_start()) return false;
    if (!i2c1_write(MCP79412_RTCC_WR)) { i2c1_stop(); return false; }
    if (!i2c1_write(reg))              { i2c1_stop(); return false; }
    if (!i2c1_write(value))            { i2c1_stop(); return false; }
    i2c1_stop();
    return true;
}

bool mcp79412_read_reg(uint8_t reg, uint8_t *value)
{
    if (value == 0) return false;
    if (!i2c1_start()) return false;
    if (!i2c1_write(MCP79412_RTCC_WR)) { i2c1_stop(); return false; }
    if (!i2c1_write(reg))              { i2c1_stop(); return false; }
    if (!i2c1_restart())               { i2c1_stop(); return false; }
    if (!i2c1_write(MCP79412_RTCC_RD)) { i2c1_stop(); return false; }
    *value = i2c1_read(false);
    i2c1_stop();
    return true;
}

bool mcp79412_read_regs(uint8_t start_reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    if ((buf == 0) || (len == 0U)) return false;
    if (!i2c1_start()) return false;
    if (!i2c1_write(MCP79412_RTCC_WR)) { i2c1_stop(); return false; }
    if (!i2c1_write(start_reg))        { i2c1_stop(); return false; }
    if (!i2c1_restart())               { i2c1_stop(); return false; }
    if (!i2c1_write(MCP79412_RTCC_RD)) { i2c1_stop(); return false; }
    for (i = 0; i < len; i++)
    {
        buf[i] = i2c1_read(i < (uint8_t)(len - 1U));
    }
    i2c1_stop();
    return true;
}

bool mcp79412_write_regs(uint8_t start_reg, const uint8_t *buf, uint8_t len)
{
    uint8_t i;
    if ((buf == 0) || (len == 0U)) return false;
    if (!i2c1_start()) return false;
    if (!i2c1_write(MCP79412_RTCC_WR)) { i2c1_stop(); return false; }
    if (!i2c1_write(start_reg))        { i2c1_stop(); return false; }
    for (i = 0; i < len; i++)
    {
        if (!i2c1_write(buf[i])) { i2c1_stop(); return false; }
    }
    i2c1_stop();
    return true;
}
```

## Example 1: Detect device presence
This is the first check to perform during board bring-up [web:4].

```c
bool mcp79412_ping_rtcc(void)
{
    bool ok;
    i2c1_start();
    ok = i2c1_write(MCP79412_RTCC_WR);
    i2c1_stop();
    return ok;
}

bool mcp79412_ping_eeprom(void)
{
    bool ok;
    i2c1_start();
    ok = i2c1_write(MCP79412_EE_WR);
    i2c1_stop();
    return ok;
}
```

## Example 2: Set time and date
The datasheet-defined method is to clear `ST`, write the calendar registers, set `VBATEN` if needed, and then set `ST` again to start the oscillator [web:4].

```c
bool mcp79412_set_datetime(const mcp79412_time_t *t, bool enable_battery)
{
    uint8_t buf[7];
    if (t == 0) return false;

    buf[0] = dec_to_bcd(t->sec) & 0x7FU;
    buf[1] = dec_to_bcd(t->min) & 0x7FU;
    buf[2] = dec_to_bcd(t->hour) & 0x3FU;
    buf[3] = (uint8_t)((t->wkday & 0x07U) | (enable_battery ? MCP79412_BIT_VBATEN : 0U));
    buf[4] = dec_to_bcd(t->date) & 0x3FU;
    buf[5] = dec_to_bcd(t->month) & 0x1FU;
    buf[6] = dec_to_bcd(t->year);

    if (!mcp79412_write_regs(MCP79412_REG_RTCSEC, buf, 7U)) return false;
    return mcp79412_write_reg(MCP79412_REG_RTCSEC, (uint8_t)(buf[0] | MCP79412_BIT_ST));
}
```

Example usage for 27-Apr-2026 Monday 10:30:00 is shown below.

```c
mcp79412_time_t rtc = {0, 30, 10, 1, 27, 4, 26};
mcp79412_set_datetime(&rtc, true);
```

## Example 3: Read current time and date
Read all seven time registers in one burst, then remove control bits before BCD conversion [web:4].

```c
bool mcp79412_get_datetime(mcp79412_time_t *t)
{
    uint8_t raw[7];
    if (t == 0) return false;
    if (!mcp79412_read_regs(MCP79412_REG_RTCSEC, raw, 7U)) return false;

    t->sec   = bcd_to_dec(raw[0] & 0x7FU);
    t->min   = bcd_to_dec(raw[1] & 0x7FU);
    t->hour  = bcd_to_dec(raw[2] & 0x3FU);
    t->wkday = raw[3] & 0x07U;
    t->date  = bcd_to_dec(raw[4] & 0x3FU);
    t->month = bcd_to_dec(raw[5] & 0x1FU);
    t->year  = bcd_to_dec(raw[6]);
    return true;
}
```

## Example 4: Enable battery backup only
Battery switchover is controlled through the `VBATEN` bit in RTCWKDAY, so firmware can enable it without rewriting all date/time fields if needed [web:4].

```c
bool mcp79412_enable_battery_backup(void)
{
    uint8_t wkday;
    if (!mcp79412_read_reg(MCP79412_REG_RTCWKDAY, &wkday)) return false;
    wkday |= MCP79412_BIT_VBATEN;
    return mcp79412_write_reg(MCP79412_REG_RTCWKDAY, wkday);
}
```

## Example 5: Check oscillator running state
The `OSCRUN` bit provides firmware confirmation that the crystal oscillator is active [web:4].

```c
bool mcp79412_is_oscillator_running(bool *running)
{
    uint8_t wkday;
    if (running == 0) return false;
    if (!mcp79412_read_reg(MCP79412_REG_RTCWKDAY, &wkday)) return false;
    *running = ((wkday & MCP79412_BIT_OSCRUN) != 0U);
    return true;
}
```

## Example 6: Detect power-fail event
The MCP79412 records whether a power failure occurred through the `PWRFAIL` flag and also stores power-up and power-down timestamps in dedicated registers [web:4][web:44].

```c
bool mcp79412_has_power_failed(bool *flag)
{
    uint8_t wkday;
    if (flag == 0) return false;
    if (!mcp79412_read_reg(MCP79412_REG_RTCWKDAY, &wkday)) return false;
    *flag = ((wkday & MCP79412_BIT_PWRFAIL) != 0U);
    return true;
}
```

## Example 7: Read power-fail timestamps
These registers are useful for maintenance logs and field diagnostics because they indicate when power went down and when it returned [web:44].

```c
typedef struct
{
    uint8_t min;
    uint8_t hour;
    uint8_t date;
    uint8_t month;
    uint8_t wkday;
} mcp79412_pwrts_t;

bool mcp79412_get_powerdown_timestamp(mcp79412_pwrts_t *ts)
{
    uint8_t raw[4];
    if (ts == 0) return false;
    if (!mcp79412_read_regs(0x18U, raw, 4U)) return false;
    ts->min   = bcd_to_dec(raw[0] & 0x7FU);
    ts->hour  = bcd_to_dec(raw[1] & 0x3FU);
    ts->date  = bcd_to_dec(raw[2] & 0x3FU);
    ts->month = bcd_to_dec(raw[3] & 0x1FU);
    ts->wkday = (raw[3] >> 5) & 0x07U;
    return true;
}

bool mcp79412_get_powerup_timestamp(mcp79412_pwrts_t *ts)
{
    uint8_t raw[4];
    if (ts == 0) return false;
    if (!mcp79412_read_regs(0x1CU, raw, 4U)) return false;
    ts->min   = bcd_to_dec(raw[0] & 0x7FU);
    ts->hour  = bcd_to_dec(raw[1] & 0x3FU);
    ts->date  = bcd_to_dec(raw[2] & 0x3FU);
    ts->month = bcd_to_dec(raw[3] & 0x1FU);
    ts->wkday = (raw[3] >> 5) & 0x07U;
    return true;
}
```

## Example 8: SRAM read and write
SRAM is accessed in the RTCC/SRAM address space beginning at 0x20 and supports sequential access for application data [web:4].

```c
bool mcp79412_sram_write(uint8_t offset, const uint8_t *data, uint8_t len)
{
    if ((data == 0) || ((uint16_t)offset + len > 64U)) return false;
    return mcp79412_write_regs((uint8_t)(0x20U + offset), data, len);
}

bool mcp79412_sram_read(uint8_t offset, uint8_t *data, uint8_t len)
{
    if ((data == 0) || ((uint16_t)offset + len > 64U)) return false;
    return mcp79412_read_regs((uint8_t)(0x20U + offset), data, len);
}
```

Example retained-data usage.

```c
uint8_t stamp[4] = {0x12, 0x34, 0x56, 0x78};
mcp79412_sram_write(0, stamp, 4);
mcp79412_sram_read(0, stamp, 4);
```

## Example 9: EEPROM byte read and write
EEPROM uses the separate EEPROM control address space and is intended for nonvolatile data storage [web:4]. Normal EEPROM writes must allow internal write-cycle completion before the next access, typically by acknowledge polling or delay based on the datasheet timing model [web:4].

```c
bool mcp79412_eeprom_write_byte(uint8_t addr, uint8_t value)
{
    if (!i2c1_start()) return false;
    if (!i2c1_write(MCP79412_EE_WR)) { i2c1_stop(); return false; }
    if (!i2c1_write(addr))           { i2c1_stop(); return false; }
    if (!i2c1_write(value))          { i2c1_stop(); return false; }
    i2c1_stop();
    __delay_ms(10);
    return true;
}

bool mcp79412_eeprom_read_byte(uint8_t addr, uint8_t *value)
{
    if (value == 0) return false;
    if (!i2c1_start()) return false;
    if (!i2c1_write(MCP79412_EE_WR)) { i2c1_stop(); return false; }
    if (!i2c1_write(addr))           { i2c1_stop(); return false; }
    if (!i2c1_restart())             { i2c1_stop(); return false; }
    if (!i2c1_write(MCP79412_EE_RD)) { i2c1_stop(); return false; }
    *value = i2c1_read(false);
    i2c1_stop();
    return true;
}
```

## Example 10: Read EUI-64 MAC address
The MCP79412 protected ID area stores the unique 64-bit identifier at addresses 0xF0 to 0xF7 in the EEPROM address space, and that location is readable at any time [web:44][web:34]. This is one of the key reasons to choose the MCP79412 specifically instead of another family member [web:35][web:34].

```c
bool mcp79412_read_eui64(uint8_t mac[8])
{
    uint8_t i;
    if (mac == 0) return false;

    if (!i2c1_start()) return false;
    if (!i2c1_write(MCP79412_EE_WR)) { i2c1_stop(); return false; }
    if (!i2c1_write(0xF0U))          { i2c1_stop(); return false; }
    if (!i2c1_restart())             { i2c1_stop(); return false; }
    if (!i2c1_write(MCP79412_EE_RD)) { i2c1_stop(); return false; }

    for (i = 0; i < 8U; i++)
    {
        mac[i] = i2c1_read(i < 7U);
    }

    i2c1_stop();
    return true;
}
```

Formatting helper.

```c
void format_eui64_string(const uint8_t mac[8], char *out)
{
    sprintf(out, "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
            mac[0], mac[1], mac[2], mac[3],
            mac[4], mac[5], mac[6], mac[7]);
}
```

## Example 11: Alarm configuration
The MCP79412 includes alarm registers for two alarms and control bits that enable alarm match generation on the MFP output [web:44]. The following example shows a simple Alarm 0 setup pattern, while the exact mask bits should be selected according to the desired compare mode from the datasheet table [web:44].

```c
bool mcp79412_alarm0_set_minsec(uint8_t minute, uint8_t second)
{
    uint8_t ctrl;
    uint8_t alm[6] = {0};

    alm[0] = dec_to_bcd(second) & 0x7FU;
    alm[1] = dec_to_bcd(minute) & 0x7FU;
    alm[2] = 0x00U;
    alm[3] = 0x10U;
    alm[4] = 0x01U;
    alm[5] = 0x01U;

    if (!mcp79412_write_regs(0x0AU, alm, 6U)) return false;
    if (!mcp79412_read_reg(MCP79412_REG_CONTROL, &ctrl)) return false;
    ctrl |= 0x10U;
    return mcp79412_write_reg(MCP79412_REG_CONTROL, ctrl);
}
```

## Example 12: Square-wave output setup
The control register also manages square-wave output on the MFP pin, which can be useful for periodic wake or validation during board test [web:44]. The exact frequency selection is encoded in the square-wave frequency bits of the control register [web:44].

```c
bool mcp79412_enable_squarewave(uint8_t sqw_bits)
{
    uint8_t ctrl;
    if (!mcp79412_read_reg(MCP79412_REG_CONTROL, &ctrl)) return false;
    ctrl &= (uint8_t)~0x43U;
    ctrl |= 0x40U;
    ctrl |= (sqw_bits & 0x03U);
    return mcp79412_write_reg(MCP79412_REG_CONTROL, ctrl);
}
```

## Example 13: Oscillator trim write
The OSCTRIM register allows digital calibration adjustment to compensate oscillator drift and improve time accuracy [web:34][web:44]. Use this only after a measured calibration procedure because incorrect trim values directly affect timekeeping rate [web:34].

```c
bool mcp79412_set_trim(bool negative, uint8_t value7bit)
{
    uint8_t reg = (uint8_t)(value7bit & 0x7FU);
    if (negative)
    {
        reg |= 0x80U;
    }
    return mcp79412_write_reg(MCP79412_REG_OSCTRIM, reg);
}
```

## Example 14: Protected ID unlock concept
The protected unique ID region can be read freely, but writing requires an unlock sequence using the EEUNLOCK register and the EEPROM space defined by the datasheet [web:44]. In most firmware projects using MCP79412, the EUI-64 should be treated as read-only because the distinguishing value of this specific part is the factory-programmed identifier [web:34][web:44].

## Register masks for common fields
These masks are useful in production firmware because several registers mix data bits and control bits [web:4].

```c
#define MCP79412_MASK_SEC      0x7FU
#define MCP79412_MASK_MIN      0x7FU
#define MCP79412_MASK_HOUR24   0x3FU
#define MCP79412_MASK_WKDAY    0x07U
#define MCP79412_MASK_DATE     0x3FU
#define MCP79412_MASK_MONTH    0x1FU
```

## Recommended validation tests
A solid firmware validation plan should cover the following items [web:4][web:44][web:45]:
- Device acknowledge test on both I2C address spaces [web:4].
- Full set and get round-trip for time/date values [web:4].
- Oscillator start confirmation by reading `OSCRUN` [web:4].
- Battery switchover test with VBAT present and VCC removed [web:4].
- SRAM retention test across VCC loss [web:4].
- EEPROM read/write retention test across reset [web:4].
- EUI-64 read verification from 0xF0 to 0xF7 [web:44].
- Power-fail flag and timestamp capture verification [web:44].

## Common mistakes
These are the most common firmware integration issues [web:4][web:45]:
- Using binary values directly instead of BCD when writing time registers [web:4].
- Forgetting to set `ST`, which leaves the oscillator stopped [web:4].
- Forgetting to set `VBATEN`, which prevents backup switchover operation [web:4].
- Reading time registers without masking control bits before BCD conversion [web:4].
- Using the RTCC I2C address for EEPROM access or the EEPROM address for RTCC access [web:4].
- Omitting I2C pull-ups or using an invalid bus timing configuration on the MCU side [web:45].

## Quick reference

### Core addresses
- RTCC/SRAM 7-bit address: `0x6F` [web:4].
- EEPROM/ID 7-bit address: `0x57` [web:4].
- Unique ID range: `0xF0` to `0xF7` in EEPROM space [web:44].
- SRAM range: `0x20` to `0x5F` in RTCC/SRAM space [web:4].

### Must-do firmware actions
- Initialize PIC24 I2C correctly [web:45].
- Write BCD values only [web:4].
- Set `VBATEN` when backup battery is used [web:4].
- Set `ST` after writing the date and time [web:4].
- Read back and verify critical registers during bring-up [web:4].

## Source basis
This document is based on the official Microchip product information for MCP79412, the MCP7941X datasheet family material, and Microchip PIC24 I2C reference material used to align the firmware examples with PIC24FV32KA304-class peripherals [web:35][web:4][web:45][web:50].

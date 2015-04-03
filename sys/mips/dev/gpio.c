/*
 * GPIO driver for pic32.
 *
 * Copyright (C) 2014 Serge Vakulenko, <serge@vak.ru>
 * Copyright (C) 2015 Jaume Olivé, <jolive@iberoxarxa.com>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/spi.h>
#include <sys/systm.h>

#include <mips/dev/device.h>
#include <machine/pic32mz.h>
#include <machine/pic32_gpio.h>

/*
 * To enable debug output, uncomment the following line.
 */
#define PRINTDBG printf
#ifndef PRINTDBG
#   define PRINTDBG(...) /*empty*/
#endif

static const char pin_name[16] = "?ABCDEFGHJK?????";

int gpio_input_map1(int pin)
{
    switch (pin) {
    case RP('D',2):  return 0;
    case RP('G',8):  return 1;
    case RP('F',4):  return 2;
    case RP('D',10): return 3;
    case RP('F',1):  return 4;
    case RP('B',9):  return 5;
    case RP('B',10): return 6;
    case RP('C',14): return 7;
    case RP('B',5):  return 8;
    case RP('C',1):  return 10;
    case RP('D',14): return 11;
    case RP('G',1):  return 12;
    case RP('A',14): return 13;
    case RP('D',6):  return 14;
    }
    printf("gpio: cannot map peripheral input pin %c%d, group 1\n",
        pin_name[pin>>4], pin & 15);
    return -1;
}

int gpio_input_map2(int pin)
{
    switch (pin) {
    case RP('D',3):  return 0;
    case RP('G',7):  return 1;
    case RP('F',5):  return 2;
    case RP('D',11): return 3;
    case RP('F',0):  return 4;
    case RP('B',1):  return 5;
    case RP('E',5):  return 6;
    case RP('C',13): return 7;
    case RP('B',3):  return 8;
    case RP('C',4):  return 10;
    case RP('D',15): return 11;
    case RP('G',0):  return 12;
    case RP('A',15): return 13;
    case RP('D',7):  return 14;
    }
    printf("gpio: cannot map peripheral input pin %c%d, group 2\n",
        pin_name[pin>>4], pin & 15);
    return -1;
}

int gpio_input_map3(int pin)
{
    switch (pin) {
    case RP('D',9):  return 0;
    case RP('G',6):  return 1;
    case RP('B',8):  return 2;
    case RP('B',15): return 3;
    case RP('D',4):  return 4;
    case RP('B',0):  return 5;
    case RP('E',3):  return 6;
    case RP('B',7):  return 7;
    case RP('F',12): return 9;
    case RP('D',12): return 10;
    case RP('F',8):  return 11;
    case RP('C',3):  return 12;
    case RP('E',9):  return 13;
    }
    printf("gpio: cannot map peripheral input pin %c%d, group 3\n",
        pin_name[pin>>4], pin & 15);
    return -1;
}

int gpio_input_map4(int pin)
{
    switch (pin) {
    case RP('D',1):  return 0;
    case RP('G',9):  return 1;
    case RP('B',14): return 2;
    case RP('D',0):  return 3;
    case RP('B',6):  return 5;
    case RP('D',5):  return 6;
    case RP('B',2):  return 7;
    case RP('F',3):  return 8;
    case RP('F',13): return 9;
    case RP('F',2):  return 11;
    case RP('C',2):  return 12;
    case RP('E',8):  return 13;
    }
    printf("gpio: cannot map peripheral input pin %c%d, group 4\n",
        pin_name[pin>>4], pin & 15);
    return -1;
}

void gpio_set_input(int pin) {
    struct gpioreg *port = (struct gpioreg*) &ANSELA;
    port += (pin >> 4 & 15) - 1;
    port->trisset = (1 << (pin & 15));
    port->anselclr = (1 << (pin & 15));
}

void gpio_set_output(int pin) {
    struct gpioreg *port = (struct gpioreg*) &ANSELA;
    port += (pin >> 4 & 15) - 1;
    port->trisclr = (1 << (pin & 15));
    port->anselclr = (1 << (pin & 15));
}

void gpio_set_analog(int pin) {
    struct gpioreg *port = (struct gpioreg*) &ANSELA;
    port += (pin >> 4 & 15) - 1;
    port->trisset = (1 << (pin & 15));
    port->anselset = (1 << (pin & 15));
}

void gpio_set(int pin) {
    struct gpioreg *port = (struct gpioreg*) &ANSELA;
    port += (pin >> 4 & 15) - 1;
    port->latset = (1 << (pin & 15));
}

void gpio_clr(int pin) {
    struct gpioreg *port = (struct gpioreg*) &ANSELA;
    port += (pin >> 4 & 15) - 1;
    port->latclr = (1 << (pin & 15));
}

int gpio_get(int pin) {
    struct gpioreg *port = (struct gpioreg*) &ANSELA;
    port += (pin >> 4 & 15) - 1;
    return ((port->port & (1 << (pin & 15))) ? 1 : 0);
}

char gpio_portname(int pin) {
    return pin_name[(pin >> 4) & 15];
}

int gpio_pinno(int pin) {
    return pin & 15;
}

#include "gpio.h"
#if NGPIO > 0
#include <machine/gpio.h>

/*
 * Devices:
 *      /dev/porta ... /dev/portk
 *      /dev/confa ... /dev/confk
 *
 * Example:
 *      echo ....oiiid....iiii > /dev/confa
 *      echo ....1...0........ > /dev/porta
 *
 * Write to /dev/confX:
 *      'i' - configure the corresponding port pin as an input;
 *      'o' - configure the corresponding port pin as an output;
 *      'd' - configure the corresponding port pin as an open-drain output;
 *      'x' - deconfigure the corresponding port pin;
 *      '.' - no action.
 *
 * Write to /dev/portX:
 *      '0' - set output pin low;
 *      '1' - set output pin high;
 *      '+' - invert the value of output pin;
 *      '.' - no action.
 *
 * Use ioctl() on any of devices to control pins from the user program.
 *      ioctl(fd, GPIO_PORTA |  GPIO_CONFIN, mask)  - configure as input
 *      ioctl(fd, GPIO_PORTB |  GPIO_CONFOUT,mask)  - configure as output
 *      ioctl(fd, GPIO_PORTC |  GPIO_CONFOD, mask)  - configure as open drain
 *      ioctl(fd, GPIO_PORTD |  GPIO_DECONF, mask)  - deconfigure
 *      ioctl(fd, GPIO_PORTE |  GPIO_STORE,  val)   - set values of all pins
 *      ioctl(fd, GPIO_PORTF |  GPIO_SET,    mask)  - set to 1 by mask
 *      ioctl(fd, GPIO_PORTG |  GPIO_CLEAR,  mask)  - set to 0 by mask
 *      ioctl(fd, GPIO_PORT(0)| GPIO_INVERT, mask)  - invert by mask
 * val= ioctl(fd, GPIO_PORT(1)| GPIO_POLL,   0)     - get input values
 *
 * Several operations can be combined in one call.
 * For example, to toggle pin A2 high thew low, and get value
 * of all PORTA pins:
 * val = ioctl(fd, GPIO_PORTA | GPIO_SET | GPIO_CLEAR | GPIO_POLL, 1<<3);
 */
#undef NGPIO
#define NGPIO           10              /* Ports A, B, C, D, E, F, G, H, J, K */
#define NPINS           16              /* Number of pins per port */

#define MINOR_CONF      0x40            /* Minor mask: /dev/confX */
#define MINOR_UNIT      0x0f            /* Minor mask: unit number */

/*
 * Mask of configured pins, default empty.
 */
u_int gpio_confmask [NGPIO];

static void
gpio_print(dev, buf)
    dev_t dev;
    char *buf;
{
    u_int unit = minor(dev) & MINOR_UNIT;
    register struct gpioreg *reg = unit + (struct gpioreg*) &ANSELA;
    register u_int mask, conf, tris;
    register char c;

    conf = gpio_confmask [unit];
    tris = reg->tris;
    if (minor(dev) & MINOR_CONF) {
        /* /dev/confX device: port configuration mask */
        u_int odc = reg->odc;
        for (mask=1<<(NPINS-1); mask; mask>>=1) {
            if (! (conf & mask))
                c = '-';
            else if (tris & mask)
                c = 'i';
            else
                c = (odc & mask) ? 'd' : 'o';
            *buf++ = c;
        }
    } else {
        /* /dev/portX device: port value mask */
        u_int lat = reg->lat;
        u_int port = reg->port;
        for (mask=1<<(NPINS-1); mask; mask>>=1) {
            if (! (conf & mask))
                c = '-';
            else if (tris & mask)
                c = (port & mask) ? '1' : '0';
            else
                c = (lat & mask) ? '1' : '0';
            *buf++ = c;
        }
    }
    *buf++ = '\n';
    *buf = 0;
}

static void
gpio_parse(dev, buf)
    dev_t dev;
    char *buf;
{
    u_int unit = minor(dev) & MINOR_UNIT;
    register struct gpioreg *reg = unit + (struct gpioreg*) &ANSELA;
    register u_int mask;
    register char c;

    if (minor(dev) & MINOR_CONF) {
        /* /dev/confX device: port configuration mask */
        for (mask=1<<(NPINS-1); mask; mask>>=1) {
            c = *buf++;
            if (c <= ' ' || c > '~')
                break;
            if (c == 'x' || c == 'X')
                gpio_confmask [unit] &= ~mask;
            else if (c == 'i' || c == 'I')
                reg->trisset = mask;
            else if (c == 'o' || c == 'O') {
                reg->odcclr = mask;
                reg->trisclr = mask;
            } else if (c == 'd' || c == 'D') {
                reg->odcset = mask;
                reg->trisclr = mask;
            }
        }
    } else {
        /* /dev/portX device: port value mask */
        u_int conf = gpio_confmask [unit];
        u_int tris = reg->tris;
        for (mask=1<<(NPINS-1); mask; mask>>=1) {
            c = *buf++;
            if (c <= ' ' || c > '~')
                break;
            if (! (conf & mask) || (tris & mask))
                continue;
            if (c == '0')
                reg->latclr = mask;
            else
                reg->latset = mask;
        }
    }
}

/*
 * Open /dev/portX or /dev/confX device.
 */
int gpioopen(dev_t dev, int flag, int mode)
{
    int unit = minor(dev) & MINOR_UNIT;

    if (unit >= NGPIO)
        return ENXIO;
    if (curproc->p_ucred->cr_uid != 0)
        return EPERM;
    return 0;
}

/*
 * Close /dev/portX or /dev/confX device.
 */
int gpioclose(dev_t dev, int flag, int mode)
{
    return 0;
}

int gpioread(dev_t dev, struct uio *uio, int flag)
{
    register struct iovec *iov;
    register u_int cnt = NPINS + 1;
    char buf [20];

    /* I/o size should be large enough. */
    iov = uio->uio_iov;
    if (iov->iov_len < cnt)
        return EIO;

    /* Read only cnt bytes. */
    if (uio->uio_offset >= cnt)
        return 0;
    cnt -= uio->uio_offset;

    /* Print port status to buffer. */
    gpio_print(dev, buf);
    //PRINTDBG("gpioread -> %s", buf);

    bcopy(buf + uio->uio_offset, iov->iov_base, cnt);
    iov->iov_base += cnt;
    iov->iov_len -= cnt;
    uio->uio_resid -= cnt;
    uio->uio_offset += cnt;
    return 0;
}

int gpiowrite(dev_t dev, struct uio *uio, int flag)
{
    register struct iovec *iov = uio->uio_iov;
    register u_int cnt = NPINS;
    char buf [20];

    /* I/o size should be large enough. */
    if (iov->iov_len < cnt)
        return EIO;

    bcopy(iov->iov_base, buf, cnt);
    iov->iov_base += cnt;
    iov->iov_len -= cnt;
    uio->uio_resid -= cnt;
    uio->uio_offset += cnt;

    PRINTDBG("gpiowrite('%s')\n", buf);
    gpio_parse(dev, buf);
    return 0;
}

/*
 * Commands:
 * GPIO_CONFIN  - configure as input
 * GPIO_CONFOUT - configure as output
 * GPIO_CONFOD  - configure as open drain
 * GPIO_STORE   - store all outputs
 * GPIO_SET     - set to 1 by mask
 * GPIO_CLEAR   - set to 0 by mask
 * GPIO_INVERT  - invert by mask
 * GPIO_POLL    - poll
 *
 * Use GPIO_PORT(n) to set port number.
 */
int gpioioctl(dev_t dev, u_int cmd, caddr_t addr, int flag)
{
    register u_int unit, mask, value;
    register struct gpioreg *reg;

    PRINTDBG("gpioioctl(cmd=%08x, addr=%08x, flag=%d)\n", cmd, addr, flag);
    unit = cmd & 0xff;
    cmd &= ~0xff;

    if ((cmd & (IOC_INOUT | IOC_VOID)) != IOC_VOID ||
        ((cmd >> 8) & 0xff) != 'g')
        return EINVAL;
    if (unit >= NGPIO)
        return ENXIO;

    reg = unit + (struct gpioreg*) &ANSELA;
    mask = (u_int) addr & gpio_confmask [unit];

    if (cmd & GPIO_COMMAND & GPIO_CONFIN) {
        /* configure as input */
        PRINTDBG("TRIS%cSET %p := %04x\n", unit+'A', &reg->trisset, mask);
        reg->trisset = mask;
    }
    if (cmd & GPIO_COMMAND & (GPIO_CONFOUT | GPIO_CONFOD)) {
        if (cmd & GPIO_COMMAND & GPIO_CONFOUT) {
            /* configure as output */
            PRINTDBG("ODC%cCLR %p := %04x\n", unit+'A', &reg->odcclr, mask);
            reg->odcclr = mask;
        } else {
            /* configure as open drain */
            PRINTDBG("ODC%cSET %p := %04x\n", unit+'A', &reg->odcset, mask);
            reg->odcset = mask;
        }
        PRINTDBG("TRIS%cCLR %p := %04x\n", unit+'A', &reg->trisclr, mask);
        reg->trisclr = mask;
    }
    if (cmd & GPIO_COMMAND & GPIO_STORE) {
        /* store all outputs */
        value = reg->lat;
        PRINTDBG("LAT%c %p -> %04x\n", unit+'A', &reg->lat, value);
        value &= ~gpio_confmask [unit];
        value |= mask;
        PRINTDBG("LAT%c %p := %04x\n", unit+'A', &reg->lat, value);
        reg->lat = value;
    }
    if (cmd & GPIO_COMMAND & GPIO_SET) {
        /* set to 1 by mask */
        PRINTDBG("LAT%cSET %p := %04x\n", unit+'A', &reg->latset, mask);
        reg->latset = mask;
    }
    if (cmd & GPIO_COMMAND & GPIO_CLEAR) {
        /* set to 0 by mask */
        PRINTDBG("LAT%cCLR %p := %04x\n", unit+'A', &reg->latclr, mask);
        reg->latclr = mask;
    }
    if (cmd & GPIO_COMMAND & GPIO_INVERT) {
        /* invert by mask */
        PRINTDBG("LAT%cINV %p := %04x\n", unit+'A', &reg->latinv, mask);
        reg->latinv = mask;
    }
    if (cmd & GPIO_COMMAND & GPIO_POLL) {
        /* send current pin values to user */
        value = reg->port;
        PRINTDBG("PORT%c %p -> %04x\n", unit+'A', &reg->port, value);
        curproc->p_md.md_rval[0] = value;
    }
    return 0;
}

static int
gpioprobe(config)
    struct mips_ctlr *config;
{
    int unit = config->mips_unit;
    int flags = config->mips_flags;
    char buf [20];

    if (unit < 0 || unit >= NGPIO)
        return 0;

    gpio_confmask[unit] = flags;

    gpio_print(unit | MINOR_CONF, buf);
    printf("gpio%u at port%c, pins %s\n", unit,
        unit + (unit<8 ? 'A' : 'B'), buf);
    return 1;
}

struct driver gpiodriver = {
    "gpio", gpioprobe,
};
#endif /* NGPIO */

/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>

/* Driver configuration */
#include "ti_drivers_config.h"
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/Timer.h>

void UART_init()
{
}

void GPIO_init()
{
}



/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    /* Toggle an LED */
    GPIO_toggle(CONFIG_GPIO_LED_0);
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_1.
 *  This may not be used for all boards.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn1(uint_least8_t index)
{

}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED and button pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /* Install Button callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);

    /*
     *  If more than one input pin is available for your device, interrupts
     *  will be enabled on CONFIG_GPIO_BUTTON1.
     */
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1)
    {
        /* Configure BUTTON1 pin */
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

        /* Install Button callback */
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }

    return (NULL);
}

// Init the driver
UART_init();

// I2C Global Variables
static const struct {
       uint8_t address;
          uint8_t resultReg;
char *id;
       } sensors[3] = {
       { 0x48, 0x0000, "11X" },
       { 0x49, 0x0000, "116" },
       { 0x41, 0x0001, "006" }
};
       uint8_t txBuffer[1];
       uint8_t rxBuffer[2];
I2C_Transaction i2cTransaction;

// Driver Handles - Global variables
I2C_Handle i2c;

// Make sure you call initUART() before calling this function.
void initI2C(void)
{
    int8_t i, found;
I2C_Params i2cParams;
    DISPLAY(snprintf(output, 64, "Initializing I2C Driver - "))
// Init the driver
I2C_init();
// Configure the driver
I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
// Open the driver
i2c = I2C_open(CONFIG_I2C_0, &i2cParams);
    if (i2c == NULL)
{
           DISPLAY(snprintf(output, 64, "Failed\n\r"))
        while (1);
}
        DISPLAY(snprintf(output, 32, "Passed\n\r"))

/* Common I2C transaction setup */
i2cTransaction.writeBuf = txBuffer;
i2cTransaction.writeCount = 1;
i2cTransaction.readBuf = rxBuffer;
i2cTransaction.readCount = 0;
found = false;
for (i=0; i<3; ++i)
{
i2cTransaction.slaveAddress = sensors[i].address;
    txBuffer[0] = sensors[i].resultReg;
    DISPLAY(snprintf(output, 64, "Is this %s? ", sensors[i].id))
if (I2C_transfer(i2c, &i2cTransaction))
{
    DISPLAY(snprintf(output, 64, "Found\n\r"))
        found = true;
break;
}
    DISPLAY(snprintf(output, 64, "No\n\r"))
}
if(found)
{
    DISPLAY(snprintf(output, 64, "Detected TMP%s I2C address:
                     %x\n\r", sensors[i].id, i2cTransaction.slaveAddress))
}
else
{
    DISPLAY(snprintf(output, 64, "<%02d,%02d,%d,%04d>\n\r", temperature, setpoint, heat, seconds))
}
}
int16_t readTemp(void)
{
Int j;
int16_t temperature = 0;
i2cTransaction.readCount = 2;
if (I2C_transfer(i2c, &i2cTransaction))
{
/*
* Extract degrees C from the received data;
* see TMP sensor datasheet
*/
temperature = (rxBuffer[0] << 8) | (rxBuffer[1]);
temperature *= 0.0078125;
/*
* If the MSB is set '1', then we have a 2's complement
* negative value which needs to be sign extended
*/
if (rxBuffer[0] & 0x80)
{
temperature |= 0xF000;
}
}
else
{
DISPLAY(snprintf(output, 64, "Error reading temperature sensor
(%d)\n\r",i2cTransaction.status))
DISPLAY(snprintf(output, 64, "Please power cycle your board by
unplugging USB and plugging back in.\n\r"))
}
return temperature;
}

#define DISPLAY(x) UART_write(uart, &output, x);
// UART Global Variables
    Char output[64];
    Int bytesToSend;

    // Driver Handles - Global variables
UART_Handle uart;
void initUART(void)
{
UART_Params uartParams;

// Configure the driver
UART_Params_init(&uartParams);
uartParams.writeDataMode = UART_DATA_BINARY;
uartParams.readDataMode = UART_DATA_BINARY;
uartParams.readReturnMode = UART_RETURN_FULL;
uartParams.baudRate = 115200;

// Open the driver
uart = UART_open(CONFIG_UART_0, &uartParams);
if (uart == NULL) {

    /* UART_open() failed */
while (1);
}
}

// Driver Handles - Global variables
Timer_Handle timer0;
volatile unsigned char TimerFlag = 0;
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    TimerFlag = 1;
}
void initTimer(void)
{
Timer_Params params;

// Init the driver
Timer_init();

// Configure the driver
Timer_Params_init(&params);
params.period = 200;
params.periodUnits = Timer_PERIOD_US;
params.timerMode = Timer_CONTINUOUS_CALLBACK;
params.timerCallback = timerCallback;

// Open the driver
timer0 = Timer_open(CONFIG_TIMER_0, &params);
if (timer0 == NULL) {

    /* Failed to initialized timer */
while (1) {}
}
if (Timer_start(timer0) == Timer_STATUS_ERROR) {

    /* Failed to start timer */
while (1) {}
}
}

// Driver Handles - Global variables
Timer_Handle timer0;
volatile unsigned char TimerFlag = 0;
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
TimerFlag = 1;
}

int main()
{
    TemperatureSensor_init();

    //Check Buttons Every 200ms
    while(1)
    {
        static uint32_t buttonCounter = 0;
        if(buttonCounter >= 200)
        {
            readButtons();
            buttonCounter = 0;
        }

        //Read Temperature Every 500ms
        static uint32_t temperatureCounter = 0;
        if(temperatureCounter >=500)
        {
            readTemperature();
            temperatureCounter = 0;
        }

        updateLED();

        //Report to Server Every 1 Second
        static uint32_t reportCounter = 0;
        if(reportCounter >= 1000)
        {
            sendDataToServer();
            reportCounter = 0;
        }
    }

    return 0;
}
    if(currentTemperature > temperatureSetPoint)
    {
        GPIO_write(Board_LED0, 0);
    }

    float temperature = TemperatureSensor_getCurrentTemperature();

    void increaseSetPoint
    {
        temperatureSetPoint += 1.0;
    }
    void decreaseSetPoint
    {
        temperatureSetPoint -= 1.0;
    }
    void delay
    {
    }

    //Task Struct
    typedef struct task {
        int state; //Tasks's current state
        unsigned long period; //Task period
        unsigned long elapsedTime; //Time elapsed since last task tick
        int (*TickFct)(int); //Task tick function
    } task;

    //Creating New tasks
    task BL_task;
    task TL_task;

    //Setting Task Fields
    BL_task.state = BL_SMSart; //Indicates initial state
    BL_task.period = 500; //Task should be called every 500 seconds
    BL_task.elapsedTime = 500; //Time since last tick, causes program to start

    //Setting a Task Function field
    BL_task.TickFct = &TickFct_Blinked;

    //Calling a Tasks Tick Function
    BL_task.state = BL_task.TickFct(BL_task.state);

    return state;
    }
        }
    }
}

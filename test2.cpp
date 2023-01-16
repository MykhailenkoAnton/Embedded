// 2. Implement the software I2C master protocol on any microcontroller: Atmega, STM32, ESP32. No need to implement 
// the arbitration.ʼ


#include <common.h>
#include <rcc.h>
#include <gpio.h>
#include <i2c.h>
#include <vic.h>



static void my_delay_1()
{
   int i = 72e6/2/4;

   while( i > 0 )
     {
        i--;
        __asm__( "nop" );
     }
}

#define MYSLAVE_ADDRESS 0x32

#define MYSLAVE_SET_REG 0x01

#define MYSLAVE_GET_ADD_RESULT 0x02
#define MYSLAVE_GET_SUB_RESULT 0x03
#define MYSLAVE_GET_MUL_RESULT 0x04


static void i2c_slave_init(uint8_t ownaddress)
{
   rcc_periph_clock_enable(RCC_GPIOB);
   rcc_periph_clock_enable(RCC_I2C1);

   nvic_enable_irq(NVIC_I2C1_EV_IRQ);

   gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                 GPIO_I2C1_SDA); 
   gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                 GPIO_I2C1_SCL); 

   i2c_reset(I2C1);
   i2c_peripheral_disable(I2C1);

   i2c_set_speed(I2C1, i2c_speed_sm_100k, I2C_CR2_FREQ_36MHZ);
   i2c_set_own_7bit_slave_address(I2C1, ownaddress);
   i2c_enable_interrupt(I2C1, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);
   i2c_peripheral_enable(I2C1);

   i2c_enable_ack(I2C1);
}

volatile uint8_t *read_p;
volatile uint8_t *write_p;
volatile uint8_t writing;
volatile uint8_t reading;

volatile uint8_t buf[3];
volatile uint16_t val;


uint32_t sr1, sr2;

sr1 = I2C_SR1(I2C1);

if (sr1 & I2C_SR1_ADDR)
    {
    reading = 0;
    read_p = buf;
    write_p = ((volatile uint8_t *)(&val) + 1);
    writing = 2;
    sr2 = I2C_SR2(I2C1);
    (void) sr2;
    }

else if (sr1 & I2C_SR1_RxNE)
    {
    if (reading > 3)
        return;
    *read_p++ = i2c_get_data(I2C1);
    reading++;
    }
else if ((sr1 & I2C_SR1_TxE) && !(sr1 & I2C_SR1_BTF))
    {
    i2c_send_data(I2C1, *write_p--);
    writing--;
    }
else if (sr1 & I2C_SR1_STOPF)
    {
    i2c_peripheral_enable(I2C1);

    if (buf[0] == MYSLAVE_GET_ADD_RESULT)
        val = buf[1] + buf[2];
    else if (buf[0] == MYSLAVE_GET_SUB_RESULT)
        val = buf[1] - buf[2];
    else if (buf[0] == MYSLAVE_GET_MUL_RESULT)
        val = buf[1] * buf[2];
    }
else if (sr1 & I2C_SR1_AF)
    {
    I2C_SR1(I2C1) &= ~(I2C_SR1_AF);
    }


int main( )
{
   rcc_clock_setup_in_hse_8mhz_out_72mhz();

   rcc_periph_clock_enable(RCC_GPIOC);
   gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                 GPIO13);
   gpio_set(GPIOC, GPIO13);

   i2c_slave_init(MYSLAVE_ADDRESS);

   while( 1 )
     {
        gpio_toggle(GPIOC, GPIO13);
        my_delay_1();
     }

    return 0;
}
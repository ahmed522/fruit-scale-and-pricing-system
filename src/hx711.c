<<<<<<< HEAD
/******************************************************************************
 *
 * HX711 library for AVR ATmega328P - Implementation
 * With Proteus simulation support
 * Optimized for 1MHz clock frequency
 *
 ******************************************************************************/

#include "hx711.h"

/*---------------------------------------------------------------------------------
 * STATIC VARIABLES
 *---------------------------------------------------------------------------------*/
static uint8 g_gain = 1;            // Gain pulses (1=128, 3=64, 2=32)
static sint32 g_offset = 0;         // Tare offset
static double g_scale = 1.0;        // Scale factor
static uint8 g_gainValue = 128;     // Actual gain value for getgain()


/*---------------------------------------------------------------------------------
 * PRIVATE MACROS (for real hardware)
 *---------------------------------------------------------------------------------*/
#define HX711_SCK_HIGH()    SET_BIT(HX711_SCK_PORT, HX711_SCK_PINNUM)
#define HX711_SCK_LOW()     CLEAR_BIT(HX711_SCK_PORT, HX711_SCK_PINNUM)
#define HX711_DOUT_READ()   ((HX711_DOUT_PIN >> HX711_DOUT_PINNUM) & 0x01)
#define HX711_PULSE_DELAY_US 2


/*---------------------------------------------------------------------------------
 * PRIVATE FUNCTIONS (for real hardware)
 *---------------------------------------------------------------------------------*/
static uint8 shiftIn_MSB(void)
{
    uint8 value = 0;
    uint8 i;

    for(i = 0; i < 8; i++)
    {
        HX711_SCK_HIGH();
        _delay_us(HX711_PULSE_DELAY_US);
        value |= (HX711_DOUT_READ() << (7 - i));
        HX711_SCK_LOW();
        _delay_us(HX711_PULSE_DELAY_US);
    }

    return value;
}

/*---------------------------------------------------------------------------------
 * PUBLIC FUNCTION IMPLEMENTATIONS
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
void hx711_init(uint8 gain, double scale, sint32 offset)
{
    /* Store scale and offset */
    g_scale = scale;
    g_offset = offset;

    /* Set gain */
    switch(gain)
    {
        case HX711_GAINCHANNELA128:
            g_gain = 1;
            g_gainValue = 128;
            break;
        case HX711_GAINCHANNELA64:
            g_gain = 3;
            g_gainValue = 64;
            break;
        case HX711_GAINCHANNELB32:
            g_gain = 2;
            g_gainValue = 32;
            break;
        default:
            g_gain = 1;
            g_gainValue = 128;
            break;
    }

    /* Configure hardware pins */
    SET_BIT(HX711_SCK_DDR, HX711_SCK_PINNUM);
    HX711_SCK_LOW();

    CLEAR_BIT(HX711_DOUT_DDR, HX711_DOUT_PINNUM);
    SET_BIT(HX711_DOUT_PORT, HX711_DOUT_PINNUM);

    _delay_ms(200);

}

/*---------------------------------------------------------------------------------*/
uint8 hx711_isready(void)
{
    return (HX711_DOUT_READ() == 0) ? 1 : 0;

}

/*---------------------------------------------------------------------------------*/
sint32 hx711_read(void)
{
    uint8 data[3] = {0, 0, 0};
    uint8 filler = 0x00;
    uint32 value = 0;
    uint8 i;

    /* Wait for ready */
    while(!hx711_isready())
    {
        _delay_us(10);
    }

    /* Disable interrupts */
    cli();

    /* Read 24 bits */
    data[2] = shiftIn_MSB();
    data[1] = shiftIn_MSB();
    data[0] = shiftIn_MSB();

    /* Set gain for next reading */
    for(i = 0; i < g_gain; i++)
    {
        HX711_SCK_HIGH();
        _delay_us(HX711_PULSE_DELAY_US);
        HX711_SCK_LOW();
        _delay_us(HX711_PULSE_DELAY_US);
    }

    /* Re-enable interrupts */
    sei();

    /* Sign extension */
    if(data[2] & 0x80) {
        filler = 0xFF;
    }

    /* Construct 32-bit value */
    value = ((uint32)filler << 24) |
            ((uint32)data[2] << 16) |
            ((uint32)data[1] << 8) |
            ((uint32)data[0]);

    return (sint32)value;

}

/*---------------------------------------------------------------------------------*/
sint32 hx711_readaverage(uint8 times)
{
    sint64 sum = 0;
    uint8 i;

    for(i = 0; i < times; i++)
    {
        sum += (sint64)hx711_read();
        _delay_ms(5);
    }

    return (sint32)(sum / times);
}

/*---------------------------------------------------------------------------------*/
double hx711_getweight(void)
{
    sint32 raw = hx711_readaverage(1);
    double weight = (double)(raw - g_offset) / g_scale;

    /* Ensure non-negative */
    if(weight < 0.0) {
        weight = 0.0;
    }

    return weight;
}

/*---------------------------------------------------------------------------------*/
void hx711_calibrate1setoffset(void)
{
    /* Tare - set offset to current average reading */
    g_offset = hx711_readaverage(10);

}

/*---------------------------------------------------------------------------------*/
void hx711_calibrate2setscale(double knownWeight)
{
    /* Calculate scale factor based on known weight */
    sint32 rawValue = hx711_readaverage(10);
    g_scale = (double)(rawValue - g_offset) / knownWeight;


}

/*---------------------------------------------------------------------------------*/
void hx711_taretozero(void)
{
    /* Same as calibrate1setoffset */
    hx711_calibrate1setoffset();
}

/*---------------------------------------------------------------------------------*/
void hx711_setscale(double scale)
{
    g_scale = scale;
}

/*---------------------------------------------------------------------------------*/
double hx711_getscale(void)
{
    return g_scale;
}

/*---------------------------------------------------------------------------------*/
void hx711_setoffset(sint32 offset)
{
    g_offset = offset;


}

/*---------------------------------------------------------------------------------*/
sint32 hx711_getoffset(void)
{
    return g_offset;
}

/*---------------------------------------------------------------------------------*/
uint8 hx711_getgain(void)
{
    return g_gainValue;
}

/*---------------------------------------------------------------------------------*/
void hx711_powerdown(void)
{
    HX711_SCK_LOW();
    _delay_us(2);
    HX711_SCK_HIGH();
    _delay_us(70);
}

/*---------------------------------------------------------------------------------*/
void hx711_powerup(void)
{
    HX711_SCK_LOW();
    _delay_us(2);

}
=======
/******************************************************************************
 * HX711 library for AVR ATmega328P - Implementation
 * With optional Proteus / software simulation support
 * Optimized for ~1 MHz CPU clock
 *
 ******************************************************************************/

#include "hx711.h"

/*---------------------------------------------------------------------------------
 * STATIC VARIABLES
 *--------------------------------------------------------------------------------*/
static uint8  g_gain       = 1;      /* pulses count: 1=128, 3=64, 2=32        */
static sint32 g_offset     = 0;      /* tare offset                             */
static double g_scale      = 1.0;    /* scale factor (counts per kg)           */
static uint8  g_gainValue  = 128;    /* actual gain value for getgain()        */

/* Simulation state - uses counter instead of millis() */
static uint8  g_simulationEnabled = HX711_SIMULATION_ENABLED_DEFAULT;
static uint8  g_simPatternActive  = 0;
static uint32 g_simTicksMs        = 0;  /* incremented on each simulated read */

/*---------------------------------------------------------------------------------
 * PRIVATE MACROS (REAL HARDWARE)
 *--------------------------------------------------------------------------------*/
#define HX711_SCK_HIGH()  SET_BIT(HX711_SCK_PORT, HX711_SCK_PINNUM)
#define HX711_SCK_LOW()   CLEAR_BIT(HX711_SCK_PORT, HX711_SCK_PINNUM)
#define HX711_DOUT_READ() ((HX711_DOUT_PIN >> HX711_DOUT_PINNUM) & 0x01)

#define HX711_PULSE_DELAY_US   2

/*---------------------------------------------------------------------------------
 * PRIVATE FUNCTION PROTOTYPES
 *--------------------------------------------------------------------------------*/
static uint8  shiftIn_MSB(void);
static float32 hx711_read_simulated(void);
static float32 sim_weight_to_raw(double kg);

/*---------------------------------------------------------------------------------
 * PRIVATE FUNCTIONS
 *--------------------------------------------------------------------------------*/

/*
 * Shift in 8 bits MSB-first from HX711 DOUT, clocked by SCK.
 */
static uint8 shiftIn_MSB(void)
{
    uint8 value = 0;
    uint8 i;

    for(i = 0; i < 8; i++)
    {
        HX711_SCK_HIGH();
        _delay_us(HX711_PULSE_DELAY_US);

        value |= (HX711_DOUT_READ() << (7 - i));

        HX711_SCK_LOW();
        _delay_us(HX711_PULSE_DELAY_US);
    }

    return value;
}

/*
 * Convert desired weight in kg to a synthetic raw reading, using
 * current scale and offset so that hx711_getweight() returns ~kg.
 */
static float32 sim_weight_to_raw(double kg)
{
    /* raw = offset + scale * kg */
    double raw = (double)g_offset + (g_scale * kg);
    if(raw > 2147483647.0)
        raw = 2147483647.0;
    else if(raw < -2147483648.0)
        raw = -2147483648.0;

    return raw;
}

static float32 hx711_read_simulated(void)
{
    double targetKg;
    uint32 elapsed;

    if(!g_simPatternActive)
    {
        g_simPatternActive = 1;
        g_simTicksMs       = 0;
    }

    elapsed = g_simTicksMs;

    if(elapsed < 2000U)
    {
        /* Phase 1: 1 kg with small noise ±0.005 kg */
        double noise;
        uint8 slot = (uint8)((elapsed / 100U) % 11U);  /* 0-10 pattern */

        /* Create smooth noise pattern from -0.005 to +0.005 kg */
        switch(slot)
        {
            case 0:  noise = -0.005; break;
            case 1:  noise = -0.004; break;
            case 2:  noise = -0.003; break;
            case 3:  noise = -0.002; break;
            case 4:  noise = -0.001; break;
            case 5:  noise =  0.000; break;
            case 6:  noise =  0.001; break;
            case 7:  noise =  0.002; break;
            case 8:  noise =  0.003; break;
            case 9:  noise =  0.004; break;
            case 10: noise =  0.005; break;
            default: noise =  0.000; break;
        }

        targetKg = 1.0 + noise;
    }
    else if(elapsed < 5000U)
    {
    	/* Phase 1: 1 kg with small noise ±0.005 kg */
    	        double noise;
    	        uint8 slot = (uint8)((elapsed / 100U) % 11U);  /* 0-10 pattern */

    	        /* Create smooth noise pattern from -0.005 to +0.005 kg */
    	        switch(slot)
    	        {
    	            case 0:  noise = -0.005; break;
    	            case 1:  noise = -0.004; break;
    	            case 2:  noise = -0.003; break;
    	            case 3:  noise = -0.002; break;
    	            case 4:  noise = -0.001; break;
    	            case 5:  noise =  0.000; break;
    	            case 6:  noise =  0.001; break;
    	            case 7:  noise =  0.002; break;
    	            case 8:  noise =  0.003; break;
    	            case 9:  noise =  0.004; break;
    	            case 10: noise =  0.005; break;
    	            default: noise =  0.000; break;
    	        }

        /* Phase 2: 2 kg */
        targetKg = 2.0+noise;
    }
    else
    {
    	/* Phase 1: 1 kg with small noise ±0.005 kg */
    	        double noise;
    	        uint8 slot = (uint8)((elapsed / 100U) % 11U);  /* 0-10 pattern */

    	        /* Create smooth noise pattern from -0.005 to +0.005 kg */
    	        switch(slot)
    	        {
    	            case 0:  noise = -0.005; break;
    	            case 1:  noise = -0.004; break;
    	            case 2:  noise = -0.003; break;
    	            case 3:  noise = -0.002; break;
    	            case 4:  noise = -0.001; break;
    	            case 5:  noise =  0.000; break;
    	            case 6:  noise =  0.001; break;
    	            case 7:  noise =  0.002; break;
    	            case 8:  noise =  0.003; break;
    	            case 9:  noise =  0.004; break;
    	            case 10: noise =  0.005; break;
    	            default: noise =  0.000; break;
    	        }

        /* Phase 3: back to stable 1 kg */
        targetKg = 1.0+noise;
    }

    /* Advance simulation "time" by 100 ms per read */
    g_simTicksMs += 100U;

    return sim_weight_to_raw(targetKg);
}

/*---------------------------------------------------------------------------------
 * PUBLIC FUNCTION IMPLEMENTATIONS
 *--------------------------------------------------------------------------------*/

/*
 * Initialize HX711 driver.
 * gain  : HX711_GAINCHANNELA128, HX711_GAINCHANNELA64, HX711_GAINCHANNELB32
 * scale : scale factor (counts per kg)
 * offset: tare offset (raw value at 0 kg)
 */
void hx711_init(uint8 gain, double scale, sint32 offset)
{
    /* Store scale and offset */
    g_scale  = (scale  == 0.0) ? 1.0 : scale;
    g_offset = offset;

    /* Set gain pulses and value */
    switch(gain)
    {
        case HX711_GAINCHANNELA128:
            g_gain      = 1;
            g_gainValue = 128;
            break;

        case HX711_GAINCHANNELA64:
            g_gain      = 3;
            g_gainValue = 64;
            break;

        case HX711_GAINCHANNELB32:
            g_gain      = 2;
            g_gainValue = 32;
            break;

        default:
            g_gain      = 1;
            g_gainValue = 128;
            break;
    }

    /* Configure pins */
    SET_BIT(HX711_SCK_DDR, HX711_SCK_PINNUM);    /* SCK as output  */
    HX711_SCK_LOW();

    CLEAR_BIT(HX711_DOUT_DDR, HX711_DOUT_PINNUM);/* DOUT as input  */
    SET_BIT(HX711_DOUT_PORT, HX711_DOUT_PINNUM); /* pull-up enable */

    _delay_ms(200);

    /* Reset simulation state */
    g_simPatternActive = 0;
    g_simTicksMs       = 0;
}

/*
 * Check if HX711 is ready (DOUT low).
 */
uint8 hx711_isready(void)
{
    return (HX711_DOUT_READ() == 0) ? 1U : 0U;
}

/*
 * Read a single raw value from HX711 or from simulation.
 */
float32 hx711_read(void)
{
    /* Simulation branch */
    if(g_simulationEnabled)
    {
        return hx711_read_simulated();
    }

    /* Real hardware branch */
    uint8  data[3] = {0, 0, 0};
    uint8  filler  = 0x00;
    uint32 value   = 0;
    uint8  i;

    /* Wait for ready */
    while(!hx711_isready())
    {
        _delay_us(10);
    }

    /* Critical section */
    cli();

    /* Read 24 bits MSB-first */
    data[2] = shiftIn_MSB();
    data[1] = shiftIn_MSB();
    data[0] = shiftIn_MSB();

    /* Set gain for next reading (1–3 pulses) */
    for(i = 0; i < g_gain; i++)
    {
        HX711_SCK_HIGH();
        _delay_us(HX711_PULSE_DELAY_US);
        HX711_SCK_LOW();
        _delay_us(HX711_PULSE_DELAY_US);
    }

    /* End critical section */
    sei();

    /* Sign extension */
    if(data[2] & 0x80)
    {
        filler = 0xFF;
    }

    /* Build 32-bit signed value */
    value = ((uint32)filler << 24) |
            ((uint32)data[2] << 16) |
            ((uint32)data[1] << 8)  |
            ((uint32)data[0]);

    return (sint32)value;
}

/*
 * Average multiple raw HX711 readings.
 */
float32 hx711_readaverage(uint8 times)
{
    float64 sum = 0;
    uint8  i;

    if(times == 0U)
        times = 1U;

    for(i = 0; i < times; i++)
    {
        sum += (float64)hx711_read();
        _delay_ms(5);
    }

    return (float32)(sum / (sint64)times);
}

/*
 * Return calibrated weight in kilograms.
 */
double hx711_getweight(void)
{
    float32 raw     = hx711_readaverage(1);
    double weight  = 0.0;

    if(g_scale != 0.0)
    {
        weight = (double)(raw - g_offset) / g_scale;
    }

    if(weight < 0.0)
        weight = 0.0;

    return weight;
}

/*
 * Calibration step 1: set tare offset to current average reading.
 */
void hx711_calibrate1setoffset(void)
{
    g_offset = hx711_readaverage(10);
}

/*
 * Calibration step 2: compute scale factor using known weight (kg).
 */
void hx711_calibrate2setscale(double knownWeight)
{
    if(knownWeight <= 0.0)
    {
        return;
    }

    sint32 rawValue = hx711_readaverage(10);
    g_scale = (double)(rawValue - g_offset) / knownWeight;

    if(g_scale == 0.0)
    {
        g_scale = 1.0;
    }
}

/*
 * Tare to zero: wrapper around calibrate1setoffset().
 */
void hx711_taretozero(void)
{
    hx711_calibrate1setoffset();
}

/*
 * Set scale factor.
 */
void hx711_setscale(double scale)
{
    if(scale != 0.0)
    {
        g_scale = scale;
    }
}

/*
 * Get scale factor.
 */
double hx711_getscale(void)
{
    return g_scale;
}

/*
 * Set offset.
 */
void hx711_setoffset(sint32 offset)
{
    g_offset = offset;
}

/*
 * Get offset.
 */
sint32 hx711_getoffset(void)
{
    return g_offset;
}

/*
 * Get gain (actual numeric gain value).
 */
uint8 hx711_getgain(void)
{
    return g_gainValue;
}

/*
 * Power down HX711.
 */
void hx711_powerdown(void)
{
    HX711_SCK_LOW();
    _delay_us(2);
    HX711_SCK_HIGH();
    _delay_us(70);
}

/*
 * Power up HX711.
 */
void hx711_powerup(void)
{
    HX711_SCK_LOW();
    _delay_us(2);
}

/*---------------------------------------------------------------------------------
 * SIMULATION CONTROL
 *--------------------------------------------------------------------------------*/

/*
 * Globally enable simulation.
 */
void hx711_enableSimulation(void)
{
    g_simulationEnabled = 1;
    g_simPatternActive  = 0;
    g_simTicksMs        = 0;
}

/*
 * Globally disable simulation.
 */
void hx711_disableSimulation(void)
{
    g_simulationEnabled = 0;
    g_simPatternActive  = 0;
    g_simTicksMs        = 0;
}

/*
 * Explicitly restart the simulation pattern timeline.
 * Call this when entering the weighing state.
 */
void hx711_startSimulationPattern(void)
{
    g_simPatternActive  = 0;
    g_simTicksMs        = 0;
    g_simulationEnabled = 1;
}

/*
 * Stop the simulation pattern (optional).
 * Call this when leaving the weighing state.
 */
void hx711_stopSimulationPattern(void)
{
    g_simPatternActive  = 0;
    g_simTicksMs        = 0;
    g_simulationEnabled = 0;
}
>>>>>>> edc6387c1d585a5baaf460e549f88eece77f0548

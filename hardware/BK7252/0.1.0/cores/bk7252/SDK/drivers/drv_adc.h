#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include <rtdef.h>
//#ifndef RT_USING_ADC
//#define RT_USING_ADC
//#endif
#ifdef RT_USING_ADC

void rt_hw_adc_init(void);
int drv_adc_init(void);

#endif
#endif /* __DRV_ADC_H__ */

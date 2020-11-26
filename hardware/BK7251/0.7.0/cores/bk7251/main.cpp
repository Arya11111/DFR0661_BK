/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#define ARDUINO_MAIN
#include "rtthread.h"
#include "Arduino.h"

/*
#include "audio.h"
#include "signal.h"
#include "audio_pub.h"
#include "audio_device.h"
#include "dfs.h"
#include "hostapd.h"*/
#include "drv_flash.h"
//#include <dfs.h>
//#include <dfs_fs.h>
#include "easyflash.h"
#include "ku.h"
//#include "target_util_pub.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <dfs.h>
#include <dfs_fs.h>
#include "ate_app.h"
#include "shell.h"
#include "app.h"
#include "drv_gpio.h"
//#include "drv_i2s.h"
#include "drv_iic.h"
//#include "drv_pm.h"
#include "fal.h"
#include "drv_usbd.h"
#include "drv_uart.h"
#include "drv_wdt.h"
#include "drv_adc.h"
#include "drv_spi.h"
/////#include "drv_sdio_sd.h"
#include <dfs.h>
#include <dfs_romfs.h>
#include <drv_sys_ctrl.h>
/////#include <dfs_elm.h>
#include "audio_pub.h"
#include "shell.h"
#include "drv_wlan.h"
#ifdef __cplusplus
}
#endif
// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }
//extern int main(int argc, char **argv);
extern const struct romfs_dirent romfs_root;
#define DFS_ROMFS_ROOT          (&romfs_root) 
/*
 * \brief Main entry point of Arduino application
 */
 //extern int main(int argc, char **argv);
 
 
 static int wlan_app_init(void)
{
    /* init ate mode check. */
    ate_app_init();

    if (get_ate_mode_state())
    {
        rt_kprintf("\r\n\r\nEnter automatic test mode...\r\n\r\n");

        finsh_set_echo(0);
        finsh_set_prompt("#");

        ate_start();
    }
    else
    {
        rt_kprintf("Enter normal mode...\r\n\r\n");
        app_start();
        //user_app_start();
    }

    return 0;
}
 extern rt_err_t rt_audio_codec_hw_init(void);
//extern rt_err_t rt_audio_codec_hw_init(void);

 int main(){
    drv_iic2_init();
    rt_hw_wdt_init();
    drv_adc_init();
    rt_hw_spi_device_init();
    rt_audio_adc_hw_init();
    dfs_init();
    rt_hw_gpio_init();
    PWMInit();
    ////////rt_hw_sdcard_init();//SD卡初始化
    ///////elm_init(); //文件系统初始化
    //rt_hw_sys_ctrl_init();
    dfs_romfs_init();
    finsh_system_init();
	beken_wlan_hw_init();
    //beken_flash_init();
    //rt_hw_flash_disk_init();
    //rt_hw_gpio_init();
    //rt_i2s_hw_init();
    
    //drv_pm_init();
    //fal_init();
    
    //rt_hw_adc_init();
    
    
    //开启GPIO口
    
    
    
    //drv_pwm_init();
    if (dfs_mount(RT_NULL, "/", "rom", 0, (const void *)DFS_ROMFS_ROOT) == 0)
    {
        rt_kprintf("ROMFS File System initialized!\n");
    }else
    {
        rt_kprintf("ROMFS File System initialized Failed!\n");
    }
	// rt_device_t sd = rt_device_find("sd0");
	// const char *name;
    // name = dfs_filesystem_get_mounted_path(sd);
	// if(name == NULL) rt_kprintf("no  mounit!\r\n");
    // if(dfs_mount("sd0", "/sd", "elm", 0, 0) == 0)
        // rt_kprintf("SD File System initialized!\n");
    // else
        // rt_kprintf("SD File System initialzation failed!\n");
	// name = dfs_filesystem_get_mounted_path(sd);
	// if(name != NULL) rt_kprintf("%s \r\n", name);
#ifdef PKG_USING_EASYFLASH
    extern EfErrCode easyflash_init(void);
    easyflash_init();
#endif
    
    wlan_app_init();
    rt_audio_codec_hw_init();
    rt_usbd_vcom_class_register();
    saradc_work_create(20);
    //rt_hw_uart_init();
    //cdc_echo();
    rt_usbd_init();
    
    //rt_audio_codec_hw_init();
    
    setup();
    for(;;){
        loop();
    }
 }

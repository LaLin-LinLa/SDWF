#include "bsp_systick.h"
#include "bsp_gpio.h"
#include "drv_lcd.h"
#include "lvgl.h" 
#include "lv_port_disp_template.h"

void lv_ex_label(void)
{
		char* github_addr = "https://github.com/";
		lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_recolor(label, true);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    lv_obj_set_width(label, 120);
    lv_label_set_text_fmt(label, "#ff0000 Github: %s#", github_addr);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 10);
	
    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_recolor(label2, true);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    lv_obj_set_width(label2, 120);
    lv_label_set_text_fmt(label2, "#ff0000 Hello# #0000ff world !123456789#");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, -10);
}


/**
	* @brief  系统初始化
	* @retval None
	*/
void System_Init(void)
{	 
	bsp_systick_Init(100);
	bsp_gpio_init();
	lv_init();
	lv_port_disp_init();
	_delay_ms(2000);
	lv_ex_label();
}

/**
	* @brief  系统任务循环
	* @retval None
	*/
void System_Loop(void)
{	 
	lv_task_handler();
	//_delay_us(1);
}

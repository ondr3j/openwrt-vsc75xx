#include <linux/module.h>
#include <linux/kernel.h>
#include <net/session_limit.h>
#include <net/rate_limit.h>

int ip_bind_mac_mode;
unsigned long draytek_syslog_flag;

// session limit
void (*set_session_limit_data_from_config)(void *dataIn);
void (*set_lan_info_session_from_config)(u_int32_t ipaddr, u_int32_t netmask, u_int32_t ifindex, u_int32_t ipaddr2, u_int32_t netmask2);
void (*get_session_limit_monitor_data_out)(void *dataOut);
void (*set_auto_rate_limit_data_from_rate)(int enable, int session_max);
void (*set_session_limit_debug)(int flag);
// rate limit
void (*set_rate_limit_data_from_config)(void *dataIn) = NULL;
void (*set_rate_limit_block_from_config)(u_int32_t ip, int block) = NULL;
void (*set_lan_info_from_config)(u_int32_t ipaddr, u_int32_t netmask, u_int32_t ifindex, u_int32_t ipaddr2, u_int32_t netmask2) = NULL;
void (*get_rate_limit_monitor_data_out)(void *dataOut) = NULL;
void (*update_auto_rate_limit_from_session)(u_int32_t ip, int a) = NULL;
void (*set_rate_limit_debug)(int flag);
// appe
void (*reset_appe_counter_from_config)(int param_num);
void (*exchange_appe_counter)(int param_num);
void (*get_appe_counter_data_out)(void *dataOut);
// hso driver interface setup
void (*set_hso_driver_inf_num)(int param);
// usbserial driver dual3G mode parameter
void (*set_usbserial_driver_dual3G)(int param);
void (*reload_blk_from_file)(void);
void draytek_get_fw_type (void *param) ;

int USR_VLAN[4] = {3, 4, 5, 6};
extern int vigor_wan_type;
extern int vigor_wan_get_ip;
extern char vigor_device_name_new[];

EXPORT_SYMBOL(set_session_limit_data_from_config);
EXPORT_SYMBOL(set_lan_info_session_from_config);
EXPORT_SYMBOL(get_session_limit_monitor_data_out);
EXPORT_SYMBOL(set_auto_rate_limit_data_from_rate);
EXPORT_SYMBOL(set_session_limit_debug);
EXPORT_SYMBOL(set_rate_limit_data_from_config);
EXPORT_SYMBOL(set_rate_limit_block_from_config);
EXPORT_SYMBOL(set_lan_info_from_config);
EXPORT_SYMBOL(get_rate_limit_monitor_data_out);
EXPORT_SYMBOL(update_auto_rate_limit_from_session);
EXPORT_SYMBOL(set_rate_limit_debug);
EXPORT_SYMBOL(reload_blk_from_file);

EXPORT_SYMBOL(USR_VLAN);

EXPORT_SYMBOL(reset_appe_counter_from_config);
EXPORT_SYMBOL(exchange_appe_counter);
EXPORT_SYMBOL(get_appe_counter_data_out);
EXPORT_SYMBOL(set_hso_driver_inf_num);
EXPORT_SYMBOL(set_usbserial_driver_dual3G);


asmlinkage long sys_dray_syscall(unsigned __user mode, int option, void *param, int parameter_num)
{
	u_int32_t *p1;
	if (option == 0)
		ip_bind_mac_mode = mode;

	switch (option) {
	case 1:
		if (set_session_limit_data_from_config)
			set_session_limit_data_from_config(param);
		break;
	case 2:
		if (set_rate_limit_data_from_config)
			set_rate_limit_data_from_config(param);
		break;
	case 3:
		if (set_lan_info_session_from_config && set_lan_info_from_config)
			if (param != NULL) {
				p1 = (u_int32_t *)param;
				set_lan_info_session_from_config(p1[0], p1[1], p1[2], p1[3], p1[4]);
				set_lan_info_from_config(p1[0], p1[1], p1[2], p1[3], p1[4]);
			}
		break;
	case 4:
		if (set_rate_limit_block_from_config)
			if (param != NULL) {
				p1 = (u_int32_t *)param;
				set_rate_limit_block_from_config(p1[0], parameter_num);
			}
		break;
	case 5:
		if (get_session_limit_monitor_data_out)
			get_session_limit_monitor_data_out(param);
		break;
	case 6:
		if (get_rate_limit_monitor_data_out)
			get_rate_limit_monitor_data_out(param);
		break;
	case 7:
		// syscall(352, 0, 7, (void *)&data, 1);
		if(param){
			int *ptr = (int *)param, i;
			for(i=0; i<4; i++){
				if(*(ptr + i) != 0){
					USR_VLAN[i] = *(ptr + i);
				}
			}
		}
		break;
    case 8://dray syslog flag   
        if (param != NULL) {
	        draytek_syslog_flag = *(u_int32_t *)param;
		}
        break;
	case 9:
		if(param)
			draytek_get_fw_type((unsigned char *)param);
		break;  
	case 10:
		if (set_session_limit_debug)
			set_session_limit_debug(parameter_num);
		break;
	case 11:
		if (set_rate_limit_debug)
			set_rate_limit_debug(parameter_num);
		break;
	case 12:
		if (reset_appe_counter_from_config)
			reset_appe_counter_from_config(parameter_num);
		break;
	case 13:
		if (get_appe_counter_data_out)
			get_appe_counter_data_out(param);
		break;
	case 14:
		if(set_hso_driver_inf_num){
			set_hso_driver_inf_num(parameter_num);
		}
		break;
	case 15:
		if (exchange_appe_counter)
			exchange_appe_counter(parameter_num);
		break;
	case 16:
		if(set_usbserial_driver_dual3G)
			set_usbserial_driver_dual3G(parameter_num);
		break;
#if 1 // auto-detection
	case 17: // Set wan type for Vigor ARP keep alive data system call command
    	if(parameter_num){
			vigor_wan_type = (int)parameter_num;
			//printk("Kernel space: The wan mode is %d\n", vigor_wan_type);
    	}
		break;
     case 18: // Check wan IP for Vigor ARP keep alive data system call command
    	if(parameter_num){
			vigor_wan_get_ip = (int)parameter_num;
			//printk("Kernel space: The status wan get ip is %d\n", vigor_wan_get_ip);
    	}
		break;
	 case 19: // Check device name for Vigor ARP  data system call command
    	if(param){
			strncpy(vigor_device_name_new, (char *)param, 31);
			vigor_device_name_new[31]='\0';
			//printk("Kernel space: The host name is %s\n", vigor_device_name_new);
    	}
		break;
#endif	
	case 20:	// reload url block page from file
		if (reload_blk_from_file)
			reload_blk_from_file();
		break;
	}
	return 0;
}

EXPORT_SYMBOL_GPL(ip_bind_mac_mode);
EXPORT_SYMBOL_GPL(draytek_syslog_flag);


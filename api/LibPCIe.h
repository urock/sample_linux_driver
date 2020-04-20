/**
 * @file   LibPCIe.c
 * @Author urock (yarumyantsev@gmail.com)
 * @date   03.11.2015
 * @brief  Main header for Library Rosta PCIe
 *
 * 
	Rosta PCIe Library contains functions and structures for low level access to pcie devices 
	with rosta legacy pcie driver (rosta_drv) and new xdma driver. 
	
	Library also supports device disable and pcie rescan routines used for FPGA reconfiguration.
 */

#ifndef LIB_PCIE_H
#define LIB_PCIE_H

#include "sys-include.h"
#include "../driver/driver_exports.h"

#define MAX_PCIE_DEVICES 	  64
#define MAX_DEVICES_TO_LIST   100

#ifdef __cplusplus
#   define API extern "C"
#else
#   define API
#endif


enum device_type {ROSTA_DEV, XDMA_DEV}; 




/**
* @brief brief
*/
struct parent_pci
{
unsigned parent_phys_domain;
unsigned parent_phys_bus;
unsigned parent_phys_device;
unsigned parent_phys_function;
};

/**
* @brief brief
*/
struct pci_device
{
	enum device_type dtype; 
	
	int major_num; /**< driver numbers */
	int minor_num; /**< driver numbers */
	int version; 
	unsigned vendor_id, device_id; /**< Set during locate */
	unsigned subsys_vendor_id, subsys_device_id;
	unsigned phys_domain, phys_bus, phys_device, phys_function;
	struct   parent_pci parent[100];
	int 	 num_parent;
	unsigned instance;

	int intfd; 				/**< Sdevice handle, set in rosta_pcie_open_device */
	
	
	// for xdma devices
	void *map_base_user;					// for user access

	int fd_user; 					// minor  0
	int fd_control; 				// minor  1
	int fd_events; 					// minor  2
	int fd_bypass; 					// minor  3
	
	int fd_h2c[4];					// host to card dma tranfers channels: minors 32, 33, 34, 35
	int fd_c2h[4];					// card to host dma tranfers channels: minors 64, 65, 66, 67	
};



API int rosta_pcie_find_devices(struct pci_device *pd, int verbose);

API int rosta_pcie_open_device(struct pci_device *p);
API int rosta_pcie_close_device(struct pci_device *p);

API int rosta_pcie_disable_device(struct pci_device *p);
API int rosta_pcie_rescan_bus(void);

API int rosta_pcie_print_device_info(struct pci_device *p);


API int rosta_pcie_write_reg_legacy(int dev, unsigned int bar, unsigned int offset, unsigned int data);
API int rosta_pcie_read_reg_legacy(int dev, unsigned int bar, unsigned int offset, unsigned int* data);
API int rosta_pcie_read_dma_legacy(int dev, unsigned int *buf, unsigned int len_bytes);
API int rosta_pcie_write_dma_legacy(int dev, unsigned int *buf, unsigned int len_bytes);

API int rosta_pcie_set_bits_legacy(int dev, unsigned int bar, unsigned int offset,
unsigned int *reg_val, unsigned int set_bits);

API int rosta_pcie_reset_bits_legacy(int dev, unsigned int bar, unsigned int offset,
unsigned int *reg_val, unsigned int reset_bits);




// macro defines
#define FATAL do { fprintf(stderr, "Error in func %s, at line %d, file %s (%d) [%s]\n",__func__, __LINE__, __FILE__, errno, strerror(errno)); return errno; } while(0)
	
#define do_and_test(xx,yy) \
	do { \
		r = xx yy; \
		if (r < 0) \
		{ \
			printf(# xx # yy ": error %d\n",r); \
			return r; \
		} \
	} while (0)


#endif // LIB_PCIE_H


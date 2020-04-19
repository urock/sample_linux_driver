 

#ifndef __PCIE_DEFINES_H
#define __PCIE_DEFINES_H


#define DRIVER_NAME "rosta_drv"

#define DEVICE_NAME "rosta_dev"


typedef struct {
	unsigned short device_id;
	unsigned short vendor_id;
} rsp5xx_device_info_t, *rsp5xx_device_info_p;


typedef struct {
	unsigned int bar;
	unsigned int offset;
	unsigned int data;
} rsp5xx_reg32_io_t, *rsp5xx_reg32_io_p;

typedef struct {
	unsigned int dword1;
	unsigned int dword2;
} two_dwords_t, *two_dwords_p;

typedef struct {
	unsigned int id;
	unsigned int addr;
	unsigned int size;
} rsp5xx_bar_info_t, *rsp5xx_bar_info_p;

typedef struct {
	unsigned long long dest;
	unsigned long long src;
	unsigned long long size_read;
	unsigned long long size_write;
} RostaPCIe_DualDmaTransfer_t, *RostaPCIe_DualDmaTransfer_p;

typedef struct {

	unsigned long src;
	unsigned long dest;
	unsigned long size;

} rsp_p2p_dma_t, *rsp_p2p_dma_p;


//IOCTL stuff
#define RSP_DRV_IOCTL_BASE 'r'
#define RSP_DEV_INFO 		_IOR  (RSP_DRV_IOCTL_BASE, 0, rsp5xx_device_info_p)
#define RSP_BAR_INFO 		_IOWR (RSP_DRV_IOCTL_BASE, 1, rsp5xx_bar_info_p)
#define RSP_REG32_READ 		_IOWR (RSP_DRV_IOCTL_BASE, 2, rsp5xx_reg32_io_p)
#define RSP_REG32_WRITE 	_IOW  (RSP_DRV_IOCTL_BASE, 3, rsp5xx_reg32_io_p)
#define RSP_SET_PACKET_LEN 	_IOW  (RSP_DRV_IOCTL_BASE,10, two_dwords_p)					// obsolete
#define RSP_DUAL_DMA		_IOWR (RSP_DRV_IOCTL_BASE,13, RostaPCIe_DualDmaTransfer_p)	// obsolete
#define RSP_P2P_DMA			_IOWR (RSP_DRV_IOCTL_BASE,14, rsp_p2p_dma_p)				// obsolete


#define DMA_STATUS_OK		1
#define DMA_STATUS_ERROR	-1
#define DMA_STATUS_TIMEOUT	-2


#endif //__PCIE_DEFINES_H
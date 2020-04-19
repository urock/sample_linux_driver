/**
 * @file   rosta_drv.c
 * @Author urock (yarumyantsev@gmail.com)
 * @date   03.11.2015
 * @brief  Wrapper functions for Rosta legacy driver for pcie devices (rosta_drv)
 *
 *  */
 
#include "LibPCIe.h"

static char temp[256];

int rosta_pcie_print_device_info(struct pci_device *p)
{
	int i;
	char *t = temp;
	
	t += sprintf(t, "%20s: 0x%04x\n","Vendor", p->vendor_id);
	t += sprintf(t, "%20s: 0x%04x\n","Device", p->device_id);
	t += sprintf(t, "%20s: 0x%04x\n","Subsystem Vendor", p->subsys_vendor_id);
	t += sprintf(t, "%20s: 0x%04x\n","Subsystem Device", p->subsys_device_id);
	t += sprintf(t, "%20s: %04x:%02x:%02x.%d\n","Location", p->phys_domain, p->phys_bus, p->phys_device, p->phys_function);
	// t += sprintf(t, "%20s: %04x:%02x:%02x.%d\n","Parent", p->parent_phys_domain, p->parent_phys_bus, p->parent_phys_device, p->parent_phys_function);
	
	// t += sprintf(t, "%20s: %d\n","Config", p->config.length);
	
	// for (i=0; i<6; i++)
	// {
	// 	t += sprintf(t, "%20s: %d base: 0x%8x length:%d\n","Resource", i, p->regions[i].base,  p->regions[i].length);
	// }
	
	puts(temp);
	
	return 0;
}

/**
	/brief Opens Rosta device special file
	
	If no /dev/rsp5xx special files exist in system function creates them with 777 access permisions.
	So that all host programs could be launched without sudo prefix

	After system startup this function should be called with root permisions

*/

int rosta_pcie_open_device(struct pci_device *p)
{

	struct stat buffer;
	int r, fd;
	int major, minor;
	char cmd_str[100];

	major = p->major;
	minor = p->minor;
	
	if (p->dtype != ROSTA_DEV) {
		fprintf(stderr,"%s: wrong pointer to pci_device\n", __func__);
		return -1; 
	}	
	
	
	sprintf(temp,"/dev/rsp5xx%d",p->instance);
	
	r = stat(temp, &buffer);
	
	if (r != 0)
	{
		// Node does not exists, create it
		r = mknod(temp, 0777 | S_IFCHR,  makedev(major,minor + p->instance));
		
		sprintf(cmd_str,"chmod 777 %s",temp);
		//printf("%s\n",cmd_str);
		if (system(cmd_str) <0)
			return -1;
			
		
		
	}
	else
	{
		r = 0;
		
		// Node exists, check parameters
		if (major != major(buffer.st_rdev))
		{
			r = 1;
		}
		
		if ((minor + p->instance) != minor(buffer.st_rdev))
		{
			r = 1;
		}
		
		if (r != 0)
		{
			// Node is wrong, delete and recreate
			unlink(temp); // delete name or file from the system
			r = mknod(temp, 0777 | S_IFCHR,  makedev(major,minor + p->instance));
			
			sprintf(cmd_str,"chmod 777 %s",temp);
			//printf("%s\n",cmd_str);
			if (system(cmd_str) <0)
				return -1;			
		}
	}

	if (r != 0)
	{
		return -1;
	}
	
	fd = open(temp,O_RDWR);
	
	if (fd < 0)
	{
		return -4;
	}
	
	p->intfd = fd;
	
	return 0;
}


int rosta_pcie_close_device(struct pci_device *p)
{
	if (p->dtype == ROSTA_DEV)
		close(p->intfd);	
	else {
		fprintf(stderr,"%s: wrong pointer to pci_device\n",__func__);
		return -1; 
	}
	
	return 0; 
}



int rosta_pcie_write_reg_legacy(int dev, unsigned int bar, unsigned int offset, unsigned int data) 
{
	rsp5xx_reg32_io_t reg32_io;

	reg32_io.bar = bar;
	reg32_io.offset = offset;
	reg32_io.data = data;

	return ioctl(dev, RSP_REG32_WRITE, &reg32_io);
}



int rosta_pcie_read_reg_legacy(int dev, unsigned int bar, unsigned int offset, unsigned int* data) 
{
	int ret;
	rsp5xx_reg32_io_t reg32_io;

	reg32_io.bar = bar;
	reg32_io.offset = offset;
	reg32_io.data = 0;

	ret = ioctl(dev, RSP_REG32_READ, &reg32_io);
	*data = reg32_io.data;
	return ret;
}


int rosta_pcie_set_bits_legacy(int dev, unsigned int bar, unsigned int offset,
unsigned int *reg_val, unsigned int set_bits)
{

	rosta_pcie_read_reg_legacy(dev,bar,offset,reg_val);

	*reg_val |=  set_bits;

	rosta_pcie_write_reg_legacy(dev,bar,offset,*reg_val);

	return 0;

}

int rosta_pcie_reset_bits_legacy(int dev, unsigned int bar, unsigned int offset,
unsigned int *reg_val, unsigned int reset_bits)
{

	rosta_pcie_read_reg_legacy(dev,bar,offset,reg_val);

	*reg_val &=  ~reset_bits;

	rosta_pcie_write_reg_legacy(dev,bar,offset,*reg_val);

	return 0;
}


int rosta_pcie_read_dma_legacy(int dev, unsigned int *buf, unsigned int len_bytes) 
{
	return read(dev, (void*) buf, len_bytes);
}

int rosta_pcie_write_dma_legacy(int dev, unsigned int *buf, unsigned int len_bytes) 
{

	return write(dev, (void*) buf, len_bytes);
}



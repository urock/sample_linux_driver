/**
 * @file   power.c
 * @Author 
 * @date   
 * @brief  Brief description of file.
 *
 * Detailed description of file.
 */
 
#include "LibPCIe.h"
#define TMPLEN 256

static const char *devices_path = "/sys/bus/pci/devices";
static char temp[TMPLEN];



int rosta_pcie_disable_device(struct pci_device *p)
{
	int r;
	int fd;


	sprintf(temp,"%s/%04x:%02x:%02x.%d/remove", devices_path, 
	p->phys_domain, p->phys_bus, p->phys_device, p->phys_function
	);


	fd = open(temp, O_WRONLY|O_APPEND);

	if (fd < 0)
	{
		printf("[[%s]]\n",temp); perror("OPEN:");
		return -1;
	}

	sprintf(temp,"1\n");
	
	r = write(fd, temp, 2);

	close(fd);
	
	if (r != 2)
	{
		return -2;
	}
	
	usleep(1000);
	
	return 0;
}


// call rosta_pcie_find_devices after this function
int rosta_pcie_rescan_bus(void)
{
	int r;
	int fd;
	
	sprintf(temp,"%s","/sys/bus/pci/rescan");

	fd = open(temp, O_WRONLY|O_APPEND);

	if (fd < 0)
	{
		return -1;
	}

	sprintf(temp,"1\n");
	
	r = write(fd, temp, 2);

	close(fd);
	
	if (r != 2)
	{
		return -2;
	}
	
	return 0;	
}


// this function does not seem to work
int rosta_pcie_enable_device(struct pci_device *p)
{
	int r;
	int fd;

	sprintf(temp,"%s/%04x:%02x:%02x.%d/rescan", devices_path, 
	p->parent[0].parent_phys_domain, p->parent[0].parent_phys_bus, p->parent[0].parent_phys_device, p->parent[0].parent_phys_function
	);

	fd = open(temp, O_WRONLY|O_APPEND);

	if (fd < 0)
	{
		return -1;
	}

	sprintf(temp,"1\n");
	
	r = write(fd, temp, 2);

	close(fd);
	
	if (r != 2)
	{
		return -2;
	}
	
	return 0;
}





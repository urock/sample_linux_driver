/**
 * @file   locate.c
 * @Author urock
 * @date   03.11.2015
 * @brief  LibPCIe functions to locate pcie devices 
		- legacy rosta_sdr
		- new xdma
 *
 * 
 */

#include "LibPCIe.h"



static const char *rosta_driver_path = "/sys/bus/pci/drivers/" DRIVER_NAME;


#define TMPLEN 256
static char temp[TMPLEN];


// internal prototypes
int parsefile(const char *fname, const char *format, ...);
int pcid_device_list_start(void);
int pcid_device_list_next(struct pci_device *p);
int pcid_device_list_end(void);




// INTERNALS

static int readfile(const char *fname)
{
	int fd, r;
	
	fd = open(fname, O_RDONLY);
	
	if (fd < 0)
	{
		return -1;
	}
	
	r = read(fd, temp, TMPLEN);
	//printf("Read = %s\n",temp); 	
	
	close(fd);
	
	return r;
}



int parsefile(const char *fname, const char *format, ...)
{
	int r;
	va_list ap;
	
	va_start(ap, format);
	
	r = readfile(fname);
	
	if (r < 0) return r;
	
	r = vsscanf(temp,format,ap);
	
	va_end(ap);
	
	return r;
}


// get rosta_drv driver numbers
int get_driver_numbers(int *major_num, int *minor_num, int *version)
{
	struct stat buffer; 
	int r;
	

	*major_num = -1;
	*minor_num = -1;
	*version = -1;
	
	r = stat(rosta_driver_path, &buffer);
	
	if (r != 0)
	{
		return -1;
	}
	
	if (!S_ISDIR(buffer.st_mode))
	{
		return -2;
	}
	
	sprintf(temp, "%s/%s%s", rosta_driver_path, DRIVER_NAME,"_cdev");
	
	r = parsefile(temp,"%d %d %d", major_num, minor_num, version);
	
	if (r < 0)
	{
		return -4;
	}
	
	if (*major_num < 0 || *major_num > 255) return -6;
	if (*minor_num < 0 || *minor_num > 255) return -7;
	
	//	printf("Found device, major %d minor %d\n", *major, *minor);
	
	return 0;
}




static DIR *device_dir = NULL;

int pcid_device_list_start(void)
{	
	device_dir = opendir(rosta_driver_path);
	
	if (device_dir == NULL)
	{
		return -1;
	}

	return 0;
}



int pcid_device_list_next(struct pci_device *p)
{
	int r, i, num=0;
	struct dirent *filep;
	char *q1;
	unsigned domain;
	unsigned bus;
	unsigned device;
	unsigned function;


	for(i=0;i<TMPLEN; i++)
	temp[i]=0;


	while (1)
	{
		filep = readdir(device_dir);
		
		if (filep == NULL)
		{
			closedir(device_dir);
			device_dir = NULL;
			return 0;
		}

		// Get our physical device information from the directory name
		// and throw away any non-conforming directories.
		
		r = sscanf(filep->d_name, "%04x:%02x:%02x.%d",
		&p->phys_domain, &p->phys_bus, &p->phys_device, &p->phys_function);
		
		if (r != 4)
		{
			continue;
		}

		// Parse the link information to locate our parent

		sprintf(temp,"%s/%s", rosta_driver_path, filep->d_name);


		r = readlink(temp, temp, TMPLEN);

		if (r < 1)
		{
			return -10;
		}
		
		q1 = temp + r;
		

		while (*q1 != '/') q1--;
		*q1 = '\0';
		r=4;
		while (r == 4){
			while (*q1 != '/') q1--;
			*q1 = '\0';
			q1++;
			r = sscanf(q1, "%04x:%02x:%02x.%d",
			&domain, &bus, &device, &function);
			

			if (r == 4)
			{
				
				p->parent[num].parent_phys_domain=domain;
				p->parent[num].parent_phys_bus=bus;
				p->parent[num].parent_phys_device=device; 
				p->parent[num].parent_phys_function=function;
				num++;
				p->num_parent=num;
				
			}

		}
		// Get standard PCI device indentity values from the stanard PCI subsystem files

		sprintf(temp,"%s/%s/%s", rosta_driver_path, filep->d_name, "vendor");
		
		r = parsefile(temp,"0x%04x", &p->vendor_id);
		
		if (r < 0)
		{
			return -1;
		}

		sprintf(temp,"%s/%s/%s", rosta_driver_path, filep->d_name, "subsystem_vendor");
		
		r = parsefile(temp,"0x%04x", &p->subsys_vendor_id);
		
		if (r < 0)
		{
			return -2;
		}

		sprintf(temp,"%s/%s/%s", rosta_driver_path, filep->d_name, "device");
		
		r = parsefile(temp,"0x%04x", &p->device_id);
		
		if (r < 0)
		{
			return -3;
		}

		sprintf(temp,"%s/%s/%s", rosta_driver_path, filep->d_name, "subsystem_device");
		
		r = parsefile(temp,"0x%04x", &p->subsys_device_id);
		
		if (r < 0)
		{
			return -4;
		}

		sprintf(temp,"%s/%s/%s%s", rosta_driver_path, filep->d_name, DEVICE_NAME, "_instance");
		
		r = parsefile(temp,"%d", &p->instance);
		
		if (r < 0)
		{
			return -5;
		}

		
		return 1;
	}
}


int pcid_device_list_end(void)
{
	if (device_dir != NULL)
	{
		closedir(device_dir);
		device_dir = NULL;
	}
	
	return 0;
}	

// Externals


/********************************************************************************
*	Searches for all rosta pcie devices
*
*	struct pci_device *pd - pointer to array of pci_device structs
	int verbose - 			1 - enable output printf, 0 - disable printf
*
*
*  Returns number of found devices, < 0 on error
*
********************************************************************************/
int rosta_pcie_find_devices(struct pci_device *pd, int verbose)
{
	int ret_val;
	int r, k;
	struct pci_device pd_temp;
	int major_num, minor_num, version;


	if (get_driver_numbers(&major_num, &minor_num, &version) < 0) {
		fprintf(stderr, "%s: Error getting driver numbers for rosta_dev (rosta_dev not loaded?)\n",__func__);
		return -1;
	}



	k = 0;
	if (pcid_device_list_start() < 0) {
		fprintf(stderr, "%s: Error in pcid_device_list_start (rosta_dev not loaded?)\n",__func__);
		return -1;		
	}
	do {
		ret_val = pcid_device_list_next(&pd_temp);
		if (ret_val < 0) {
			printf("Error in %s: pcid_device_list_next failed!\n",__func__);
			return ret_val;
		}

		if (ret_val == 1) {
			if (verbose == 1) 
				printf("Found device # %d\t %04x:%02x:%02x.%d %04x:%04x\n", k,
				pd_temp.phys_domain, pd_temp.phys_bus, pd_temp.phys_device,
				pd_temp.phys_function, pd_temp.vendor_id, pd_temp.device_id);
				

			pd_temp.major_num = major_num;
			pd_temp.minor_num = minor_num;
			pd_temp.version = version; 
			
			pd_temp.dtype = ROSTA_DEV;

			pd[k] = pd_temp;
			
			
			k++;
		}
	} while (ret_val == 1);

	do_and_test(pcid_device_list_end,());

	

	return k;
}








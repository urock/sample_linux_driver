
#include "LibPCIe.h"

int bar_test(int dev);
int read_reg_test(int dev);



int main(int argc, char *argv[])
{
	int dev;
	int r, k, ret_val, i;
//	struct pci_device pd_temp; 
	unsigned int val32;
	char ch; 
	int num; 

	struct pci_device pd[20];


	printf("Searching for devices... \n\n");

	k = rosta_pcie_find_devices(pd,1);
	if (k < 0) {
		printf("Error searching for devices!\n");
		return 0;
	}

	if (!k)	printf("No device found\n");
	else printf("\nFound %d devices\n",k);


	while (1) {
		
	printf("\nChoose device (0 - %d) or type:\n", k-1);
	printf("\t\t\t 's' - to show info on all devices\n");
	printf("\t\t\t 'r' - to rescan pci bus\n");
	printf("\t\t\t 'q' - to exit\n");

	scanf("%s",&ch); 

	switch (ch) {
		case 'q': // exit
			return 0;

		case 's': // show info
			for (i = 0; i < k; i++) {
				printf("Device # %d, instance %d:::\n",i,pd[i].instance); 
				do_and_test(rosta_pcie_open_device,(&pd[i]));
				rosta_pcie_print_device_info(&pd[i]);
				rosta_pcie_close_device(&pd[i]);
			}			
			
			break;

		case 'r': // rescan bus
			do_and_test(rosta_pcie_rescan_bus,()); 
			sleep(1); 
			k = rosta_pcie_find_devices(pd,1);
			if (k < 0) {
				printf("Error searching for devices!\n");
				return 0;
			}

			if (!k)	printf("No device found\n");
			else printf("Found %d devices\n",k); 
			break; 
		}

	num  = (int)strtol(&ch,(char **)NULL,10); 

	// check if entered value is correct
	if (!(num >= 0 && num <= (k-1)) || ((num == 0)&&(ch!='0'))) {
		//printf("Not suppotred value!\n");
		continue;
	}
	// end check

	printf("You entered %d\n",num); 


		while (1) {
		printf("Choose action: \n");
		printf("\t 1 - Test BARs\n");
		printf("\t 2 - Remove device from the system\n");
		printf("\t 3 - Rescan Bus\n");		
		printf("\t 4 - Read reg test\n");
		printf("\t q - to exit to device select menu\n");
		

		scanf("%s",&ch);

		if (ch == 'q')
			break; 		
		else if (ch == '1') {
			printf("Running BARs test\n");
			do_and_test(rosta_pcie_open_device,(&pd[num]));
			ret_val = bar_test(pd[num].intfd);
			if (ret_val < 0) 
				printf("Error running BARs test\n");
			rosta_pcie_close_device(&pd[num]);

			}
		else 	if (ch == '2') { // remove device
			
			do_and_test(rosta_pcie_disable_device,(&pd[num])); 
			sleep(1); 
			printf("Remove device ok!\n"); 

			k = rosta_pcie_find_devices(pd,1);
			if (k < 0) {
				printf("Error searching for devices!\n");
				return 0;
			}

			if (!k)	printf("No device found\n");
			else printf("Found %d devices\n",k); 
			break; 

			}

		else 	if (ch == '3') { // rescan bus 
			
			do_and_test(rosta_pcie_rescan_bus,()); 
			sleep(1); 

			k = rosta_pcie_find_devices(pd,1);
			if (k < 0) {
				printf("Error searching for devices!\n");
				return 0;
			}

			if (!k)	printf("No device found\n");
			else printf("Found %d devices\n",k); 
			break; 

			}

		else if (ch == '4') {
			printf("Running Read Reg test\n");
			do_and_test(rosta_pcie_open_device,(&pd[num]));
			ret_val = read_reg_test(pd[num].intfd);
			if (ret_val < 0) 
				printf("Error running BARs test\n");
			rosta_pcie_close_device(&pd[num]);

			}
		 

		}
	}

	return 0;
}



int read_reg_test(int dev)
{
	int ret_val=0, bar=0;
	unsigned int addr=0x110, wr_val = 0x123, rd_val = 0;	

	int i, t=1000;

	//release user reset
	// rosta_pcie_write_reg_legacy(dev, CNTRL_BAR, USER_CNTRL_REG, 1);

	ret_val = rosta_pcie_write_reg_legacy(dev, bar, addr, wr_val);
	if (ret_val < 0) {
		printf("read_reg_test: Error writting to BAR[%d] at offset = %x!\n", bar, addr);
		return -1;
	}		

	for(i=0;i<t;i++) {
		
		ret_val = rosta_pcie_read_reg_legacy(dev, bar, addr, &rd_val);
		if (ret_val < 0) {
			printf("read_reg_test: Error at i = %d reading from BAR[%d] at offset = %x!\n", i, bar, addr);
			return -1;
		}	

		if (rd_val != wr_val) {
			// rosta_pcie_write_reg_legacy(dev, CNTRL_BAR, 0x150, 1);		
			// rosta_pcie_write_reg_legacy(dev, CNTRL_BAR, 0x150, 0);				
			printf("read_reg_test: i = %d, rd_val = 0x%x\n",i,rd_val); 
			return -3; 
			
		}
	}

	return 0; 
}


int bar_test(int dev) {
	int ret_val, bar;
	unsigned int addr, value;
	char ch;

	printf("\n\t :::::::::::TESTING REGS::::::::::: \n");
	printf("\t\tEnter BAR num in decimal: ");
	scanf("%d", &bar);

	while (1) {
		printf("Choose action: \n\t 1 - Write reg \n\t 2 - Read reg 3 - Exit\n");

		scanf("%s", &ch);

		if (ch == '3')
			return 0;

		printf("Enter reg address in HEX: ");
		scanf("%x", &addr);

		if (ch == '1') {
			printf("\nEnter value to write in HEX: ");
			scanf("%x", &value);
			ret_val = rosta_pcie_write_reg_legacy(dev, bar, addr, value);
			if (ret_val < 0) {
				printf("Error writting to BAR[%d] at offset = %x!\n", bar, addr);
				return -1;
			}

		}

		if (ch == '2') {
			printf("\nReg value:");
			ret_val = rosta_pcie_read_reg_legacy(dev, bar, addr, &value);
			if (ret_val < 0) {
				printf("Error reading from BAR[%d] at offset = %x!\n", bar,
						addr);
				return -1;
			}
			printf("%x\n", value);
		}


	}

	return 0;
}


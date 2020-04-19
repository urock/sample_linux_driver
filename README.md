# Sample linux pcie driver and simple device test app

Требует cmake для сборки


## Инструкиця по сборке

0. Скачать репозиторий

```
git clone git@github.com:urock/sample_linux_driver.git pcie_driver
```

1. Компиляция драйвера

```
cd pcie_driver/driver
make
```

На выходе должен быть создан драйвер `rosta_drv.ko` 

2. Компиляция библиотеки и теста

```
cd pcie_driver
mkdir build
cd build
cmake ..
make
```

На выходе внутри директории `build` должен появиться файл `device_test`

3. Запуск теста

```
cd pcie_driver/driver  
sudo insmod rosta_drv.ko
cd pcie_driver/build
./device_test
```


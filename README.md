# Sample linux pcie driver and simple device test app

Требует cmake для сборки


## Инструкиця по сборке

1. Компиляция драйвера

```
cd driver
make
```

На выходе должен быть создан драйвер `rosta_drv.ko` 

2. Компиляция библиотеки и теста

```
mkdir build
cd build
cmake ..
make
```

На выходе внутри директории `build` должен появиться файл `device_test`

3. Запуск теста

```
cd driver  
sudo insmod rosta_drv.ko
cd ../build
./device_test
```


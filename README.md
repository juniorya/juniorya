# STM32F107 Delta Robot CNC Controller

Это bare-metal проект под STM32F107 (72 МГц) для управления дельта-роботом через EtherCAT сервоприводы по профилю CiA-402. Контроллер реализует ядро ЧПУ, планировщик траекторий с look-ahead и S-кривыми, EtherCAT-мастер с синхронизацией DC (Sync0), а также модуль дельта-кинематики. Код организован без RTOS (суперлуп + прерывания Sync0).

## Структура проекта

```
core/       – состояния ЧПУ, главный цикл, обработка команд
planner/    – планировщик движений (look-ahead, S-curve)
kinematics/ – дельта-кинематика, Якобиан и workspace-проверки
motion/     – выдача сетпойнтов в EtherCAT по режиму CSP/CST/CSV
ethcat/     – EtherCAT мастер (SOEM-подобный API), DC sync, PDO/SDO
cia402/     – реализация профиля CiA-402 и state-machine
gcode/      – парсер G-кода (G0/1/2/3/4/20/21/90/91, M3/4/5/17/18/112)
drivers/    – STM32F1 ETH MAC/PHY, GPIO, UART (изоляция от HAL)
board/      – конфигурация платы, клоки, MAC/PHY, delta_cfg_t
utils/      – fixed-point Q16.16, CORDIC-тригонометрия, матрицы, CRC, таймер
tests/      – хостовые unit-тесты (x86)
```

## Сборка

### Хостовые unit-тесты

```bash
cmake -S . -B build -DHOST_TESTS=ON
cmake --build build --target tests_host
./build/tests_host
```

### Прошивка STM32 (arm-none-eabi)

```bash
cmake -S . -B build-stm32 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm-none-eabi.cmake
cmake --build build-stm32 --target firmware_stm32
```

Полученный ELF (`firmware_stm32.elf`) прошивается через ST-Link Utility / STM32CubeProgrammer.

## EtherCAT

* Поддерживаются три привода (оси A/B/C) с верификацией VendorId/ProductCode.
* Sync0 = 1 кГц (опционально 2 кГц через `board/config.h`).
* PDO-карта (пример):
  * **RxPDO** – Controlword (0x6040), Target Position (0x607A), Target Velocity (0x60FF), Target Torque (0x6071), Modes of Operation (0x6060).
  * **TxPDO** – Statusword (0x6041), Position Actual Value (0x6064), Velocity Actual Value (0x606C), Torque Actual Value (0x6077), Modes of Operation Display (0x6061), EMCY code.
* При инициализации по CoE задаются лимиты V/A/J (0x6081/0x6083/0x607F), параметры homing и масштаб энкодера.
* DC синхронизация – выравнивание локального таймера MCU по смещению DC и коррекция дрейфа.

## CiA-402

* Полный state-machine: Not Ready → Operation Enabled, обработка Fault/Quick Stop/Fault Reset.
* Режимы CSP (основной), CST и CSV.
* Интерфейс `cia402_axis_command()` подаёт сетпойнты и формирует Controlword, статусы читаются через `cia402_axis_update()`.

## Планировщик

* Очередь на 128 строк G-кода, look-ahead на ≥16 сегментов, сглаживание по углам.
* S-кривая на каждом тике (1 кГц) с деградацией до трапеций при нехватке буфера.
* Ограничения V/A/J задаются в `board/config.h`.

## Консоль и команды

UART 115200 бод: приём G-кода, сервисные команды `$H`, `$X`, `$ECAT?`. Ответы в формате `ok`/`error:<код>`.

## Самотесты ($SELFTEST)

* Круг XY (R = 50 мм) и квадрат 100×100 мм с отчётом по максимальному отклонению.
* EtherCAT цикл: счётчик пропусков Sync0.

## Параметры Sync0/DC

Настраиваются в `board/config.h` (период, смещение, список приводов). Flash-память может использоваться для хранения параметров (wear-leveling).

## Типовой запуск

1. Сконфигурировать MAC/PHY (RMII) в CubeMX, собрать прошивку.
2. Поднять EtherCAT сеть, убедиться в `ok` от команды `$ECAT?`.
3. Перейти в Operation Enabled (`M17`), отправить G1 X/Y/Z F... для движения.
4. Аварийный стоп (`M112` или аппаратный E-stop) → Quick Stop + disable < 2 мс.

## Лицензия

MIT.

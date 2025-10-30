Сборка и развёртывание
======================

Базовый сценарий включает запуск ``./configure`` для выбора целевой платформы,
затем ``make`` для сборки артефактов и, при необходимости, ``make install`` для
установки под выбранный ``PREFIX``. Все вспомогательные утилиты (IDE,
симулятор, тесты) собираются вместе с прошивкой и размещаются в ``build/bin``.

Host / QEMU
-----------

Для настольной отладки используйте:

.. code-block:: bash

   ./configure --target-os=host --enable-brachisto
   make
   make check
   ./build/bin/cnc_simulator

Уберите ``--enable-brachisto``, если достаточно классических сплайнов. Команда
``make docs`` формирует HTML/PDF документацию в ``build/docs``.
Для QEMU-эмуляции MIPS64 достаточно сменить целевую ОС и, при необходимости,
указать кросс-компилятор:

.. code-block:: bash

   ./configure --target-os=qemu --cc="clang --target=mips64el-linux-gnuabi64"
   make
   make qemu-selftest

Скрипт ``make qemu-selftest`` собирает автономный бинарник
``tests/qemu_selftest`` и запускает его в ``qemu-mips64el`` при наличии
эмулятора.

QNX / VxWorks / ОСРВ «Багет»
---------------------------

Кросс-компиляция выполняется тем же ``configure``. Пример для QNX 7.x:

.. code-block:: bash

   ./configure --target-os=qnx --cc=qcc --extra-cflags='-Vgcc_nto64'
   make

Для ОСРВ «Багет» при необходимости задайте ``--endian=be``. Дополнительные
заголовки и библиотеки BSP подключаются через ``--extra-cppflags`` и
``--extra-ldflags``. Установка выполняется командой ``make install`` (поддержан
``DESTDIR``).

Параметры запуска
-----------------

* ``cnc_firmware`` — основной бинарник прошивки с разделением RT/aux процессов;
* ``cnc_ide`` — пакетный экспорт проектов ``.cncp`` и конфигурация TinyML;
* ``cnc_simulator`` — визуализация и имитация траекторий без аппаратуры.

Опции командной строки и сетевые настройки EtherCAT/OPC UA описаны в разделах
``EtherCAT + CiA-402`` и ``OPC UA модель``.

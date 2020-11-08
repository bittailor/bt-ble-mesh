build:

`west build -p auto -b particle_xenon`

clean build and flash:
`rm -rf build && west build -p auto -b particle_xenon && west flash`

erase:

``` bash
cd /Users/bittailor/.particle/toolchains/openocd/0.11.2-adhoc6ea4372.0

bin/openocd -f interface/cmsis-dap.cfg -f target/nrf52-particle.cfg \
-c "adapter_khz 1000" \
-c "transport select swd" \
-c "init" \
-c "reset halt" \
-c "flash erase_address 0x000f8000 0x00008000" \
-c "exit"

bin/openocd -f interface/cmsis-dap.cfg -f target/nrf52-particle.cfg \
-c  "adapter_khz 1000" \
-c "transport select swd" \
-c "init" \
-c "reset halt" \
-c "flash erase_address 0x0000C000 0x00067000" \
-c "exit"

bin/openocd -f interface/cmsis-dap.cfg -f target/nrf52-particle.cfg \
-c  "adapter_khz 1000" \
-c "transport select swd" \
-c "init" \
-c "reset halt" \
-c "flash erase_address 0x00073000 0x00067000" \
-c "exit"
```

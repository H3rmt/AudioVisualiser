call_recipe := just_executable() + " --justfile=\"" + justfile() + "\""

build env="wired":
    platformio run -e {{ env }}

upload *args:
    platformio run -e wired -t upload {{ args }}

monitor device="auto":
    #!/usr/bin/env bash
    if [ "{{ device }}" = "auto" ]; then
      device=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -n1);
      [ -n "$device" ] || { echo "No serial device found"; exit 1; };
    else
      device="{{ device }}";
    fi;
    exec minicom -D "$device" -b 115200

monitor-pio:
    platformio device monitor -e wired -b 115200

setup:
    picotool partition create main.json ./tmp/pt.uf2
    picotool load ./tmp/pt.uf2
    picotool reboot -u

test:
    pio test -e native -vv

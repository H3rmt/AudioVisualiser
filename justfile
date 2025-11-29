call_recipe := just_executable() + " --justfile=\"" + justfile() + "\""

build env="dfu":
    platformio run -e {{ env }}

upload-whired port: (build "dfu")
    #!/usr/bin/env bash
    platformio run -e dfu -t nobuild -t upload --upload-port {{ port }}

upload-whireless:
    #!/usr/bin/env bash
    [ -f .env ] && source .env
    export PLATFORMIO_UPLOAD_FLAGS="--auth='${OTA_PASSWORD}'"
    {{ call_recipe }} build "ota"
    platformio run -e ota -t nobuild -t upload --upload-port ${ESP_HOSTNAME}

upload:
    #!/usr/bin/env bash
    device=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -n1) || true;
    if [ -n "$device" ]; then
      echo "Uploading via wired connection to $device"
      {{ call_recipe }} upload-whired "$device"
    else
      echo "No wired device found, uploading via wireless"
      {{ call_recipe }} upload-whireless
    fi

monitor device="auto":
    #!/usr/bin/env bash
    if [ "{{ device }}" = "auto" ]; then
      device=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -n1);
      [ -n "$device" ] || { echo "No serial device found"; exit 1; };
    else
      device="{{ device }}";
    fi;
    exec minicom -D "$device" -b 115200

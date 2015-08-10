CFLAGS += -std=c99 -Wall -Wextra -pedantic -O3 -g \
-DSENSOR_PATH='"/sys/bus/i2c/drivers/al3000a/2-001c/show_lux"' \
-DBRIGHTNESS_CONTROL='"/sys/class/backlight/pwm-backlight/brightness"' \
-DHISTORY_DEPTH=5 \
-DBRIGHTNESS_MIN=10 \
-DBRIGHTNESS_MAX=255 \
-D_X_OPEN_SOURCE=500
 
bin/autoBrightness : src/autoBrightness.o
	$(CC) $(CFLAGS) -static -o $@ $< -lm

PHONY: clean

clean:
	rm src/autoBrightness.o bin/autoBrightness


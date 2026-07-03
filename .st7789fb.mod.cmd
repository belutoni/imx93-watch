savedcmd_drivers/lkss/lab5/st7789fb.mod := printf '%s\n'   st7789fb.o | awk '!x[$$0]++ { print("drivers/lkss/lab5/"$$0) }' > drivers/lkss/lab5/st7789fb.mod

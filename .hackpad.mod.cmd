savedcmd_drivers/lkss/lab5/hackpad.mod := printf '%s\n'   hackpad.o | awk '!x[$$0]++ { print("drivers/lkss/lab5/"$$0) }' > drivers/lkss/lab5/hackpad.mod

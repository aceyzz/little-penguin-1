obj-m := fortytwo.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

MOD   := fortytwo
DEV   := /dev/fortytwo
LOGIN := cedmulle

GREEN  := \033[1;32m
RED    := \033[1;31m
YELLOW := \033[1;33m
BLUE   := \033[1;34m
RESET  := \033[0m

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

test: all
	@printf '%b\n' "$(YELLOW)[!] Nettoyage de l'historique 'dmesg'$(RESET)"
	@dmesg -c >/dev/null 2>&1 || true
	@printf '\n'
	@printf '%b\n' "$(YELLOW)[!] Pré-nettoyage éventuel...$(RESET)"
	@sudo rmmod $(MOD) >/dev/null 2>&1 || true
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Insertion du module...$(RESET)"
	@sudo insmod $(MOD).ko || { printf '%b\n' "$(RED)[-] Échec insmod$(RESET)"; exit 1; }
	@sleep 1
	@dmesg -T | tail -n 1
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Vérification du device...$(RESET)"
	@ls -l $(DEV) || { printf '%b\n' "$(RED)[-] $(DEV) introuvable$(RESET)"; sudo rmmod $(MOD); exit 1; }
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Lecture du device :$(RESET)"
	@cat $(DEV) || { printf '%b\n' "$(RED)[-] Lecture échouée$(RESET)"; sudo rmmod $(MOD); exit 1; }
	@printf '\n'
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Écriture correcte :$(RESET)"
	@echo "$(LOGIN)" | sudo tee $(DEV) >/dev/null && printf '%b\n' "$(GREEN)[OK] écriture valide$(RESET)"
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Écriture incorrecte :$(RESET)"
	@echo "wronglogin" | sudo tee $(DEV) >/dev/null || printf '%b\n' "$(GREEN)[OK] écriture refusée$(RESET)"
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Suppression du module...$(RESET)"
	@sudo rmmod $(MOD) || { printf '%b\n' "$(RED)[-] Échec rmmod$(RESET)"; exit 1; }
	@sleep 1
	@dmesg -T | tail -n 1
	@printf '\n'

test-log: all
	@echo "[STEP 0] Reset dmesg"
	@echo "COMMAND : dmesg -c"
	@echo "EXPECTED: clear kernel ring buffer (no specific output)"
	@out="$$(dmesg -c 2>&1 || true)"; rc=$$?; echo "OBTAINED: $$out"; echo "RC      : $$rc"; echo "RESULT  : PASS"; echo
	@echo "[STEP 1] Pre-unload module (if loaded)"
	@echo "COMMAND : rmmod $(MOD)"
	@echo "EXPECTED: module removed or not present"
	@out="$$(sudo rmmod $(MOD) 2>&1 || true)"; rc=$$?; echo "OBTAINED: $$out"; echo "RC      : $$rc"; if [ $$rc -eq 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : PASS (not loaded)"; fi; echo
	@echo "[STEP 2] Insert module"
	@echo "COMMAND : insmod $(MOD).ko"
	@echo "EXPECTED: RC=0 and a registration message in dmesg"
	@out="$$(sudo insmod $(MOD).ko 2>&1)"; rc=$$?; kmsg="$$(dmesg | tail -n 1)"; echo "OBTAINED: $$out"; echo "KMSG    : $$kmsg"; echo "RC      : $$rc"; if [ $$rc -eq 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 3] Check device node"
	@echo "COMMAND : ls -l $(DEV)"
	@echo "EXPECTED: $(DEV) exists (char device), RC=0"
	@out="$$(ls -l $(DEV) 2>&1)"; rc=$$?; echo "OBTAINED: $$out"; echo "RC      : $$rc"; if [ $$rc -eq 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 4] Read #1 (fresh offset) -> must return login"
	@echo "COMMAND : cat $(DEV)"
	@echo "EXPECTED: stdout == '$(LOGIN)', RC=0"
	@got="$$(cat $(DEV) 2>&1)"; rc=$$?; echo "OBTAINED: $$got"; echo "RC      : $$rc"; if [ $$rc -eq 0 ] && [ "$$got" = "$(LOGIN)" ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 5] Read #2 on SAME FD (EOF) -> must be empty"
	@LEN=$$(printf "%s" "$(LOGIN)" | wc -c | tr -d ' '); \
	echo "COMMAND : exec 3<$(DEV); dd <&3 bs=$$LEN count=1; dd <&3 bs=$$LEN count=1; exec 3<&-"; \
	echo "EXPECTED: first dd outputs '$(LOGIN)', second dd outputs '' (empty), RC=0"; \
	exec 3<$(DEV); \
	first=$$(dd bs=$$LEN count=1 <&3 2>/dev/null | cat); \
	second=$$(dd bs=$$LEN count=1 <&3 2>/dev/null | cat); \
	rc=$$?; exec 3<&-; \
	echo "OBTAINED: first='$$first' second='$$second'"; echo "RC      : $$rc"; \
	if [ "$$first" = "$(LOGIN)" ] && [ -z "$$second" ] && [ $$rc -eq 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 6] Valid write (exact, no newline)"
	@echo "COMMAND : printf '%s' '$(LOGIN)' > $(DEV)"
	@echo "EXPECTED: RC=0"
	@sh -c 'printf "%s" "$(LOGIN)" > $(DEV)'; rc=$$?; echo "OBTAINED: (no stdout)"; echo "RC      : $$rc"; if [ $$rc -eq 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 7] Valid write (login + newline)"
	@echo "COMMAND : printf '%s\n' '$(LOGIN)' > $(DEV)"
	@echo "EXPECTED: RC=0"
	@sh -c 'printf "%s\n" "$(LOGIN)" > $(DEV)'; rc=$$?; echo "OBTAINED: (no stdout)"; echo "RC      : $$rc"; if [ $$rc -eq 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 8] Invalid write (same length, wrong content)"
	@echo "COMMAND : bad=\$$(printf '%s' '$(LOGIN)' | tr 'a-z' 'n-za-m'); printf '%s' \"\$$bad\" > $(DEV)"
	@echo "EXPECTED: RC!=0 (EINVAL)"
	@bad="$$(printf '%s' '$(LOGIN)' | tr 'a-z' 'n-za-m')"; printf "INFO    : altered='%s'\n" "$$bad"; printf "%s" "$$bad" > $(DEV); rc=$$?; echo "OBTAINED: (no stdout)"; echo "RC      : $$rc"; if [ $$rc -ne 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 9] Invalid write (wrong length: too short)"
	@echo "COMMAND : printf 'ced' > $(DEV)"
	@echo "EXPECTED: RC!=0 (EINVAL)"
	@sh -c 'printf "ced" > $(DEV)'; rc=$$?; echo "OBTAINED: (no stdout)"; echo "RC      : $$rc"; if [ $$rc -ne 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 10] Read again with fixed size -> must return login"
	@echo "COMMAND : dd if=$(DEV) bs=<LOGIN_LEN> count=1 | cat"
	@echo "EXPECTED: stdout == '$(LOGIN)', RC=0"
	@LEN=$$(printf "%s" "$(LOGIN)" | wc -c | tr -d ' '); got="$$(dd if=$(DEV) bs=$$LEN count=1 2>/dev/null | cat)"; rc=$$?; echo "INFO    : LOGIN_LEN=$$LEN"; echo "OBTAINED: $$got"; echo "RC      : $$rc"; if [ $$rc -eq 0 ] && [ "$$got" = "$(LOGIN)" ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[STEP 11] Remove module"
	@echo "COMMAND : rmmod $(MOD)"
	@echo "EXPECTED: RC=0 and a deregistration message in dmesg"
	@out="$$(sudo rmmod $(MOD) 2>&1)"; rc=$$?; kmsg="$$(dmesg | tail -n 1)"; echo "OBTAINED: $$out"; echo "KMSG    : $$kmsg"; echo "RC      : $$rc"; if [ $$rc -eq 0 ]; then echo "RESULT  : PASS"; else echo "RESULT  : FAIL"; fi; echo
	@echo "[SUMMARY] Finished all tests"

obj-m := debugfs.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

MOD      := debugfs
DBGROOT  := /sys/kernel/debug
DIR      := $(DBGROOT)/fortytwo
ID       := $(DIR)/id
JIFFIES  := $(DIR)/jiffies
FOO      := $(DIR)/foo

LOGIN    := cedmulle

NONROOT  := $(shell id -u nobody >/dev/null 2>&1 && echo nobody || echo false)

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
	@printf '%b\n' "$(YELLOW)[!] Reset dmesg$(RESET)"
	@dmesg -c >/dev/null 2>&1 || true
	@printf '\n'
	@printf '%b\n' "$(YELLOW)[!] Vérification/montage de debugfs$(RESET)"
	@grep -qsE 'debugfs .* $(DBGROOT) ' /proc/mounts || sudo mount -t debugfs none $(DBGROOT)
	@ls -ld $(DBGROOT)
	@printf '\n'
	@printf '%b\n' "$(YELLOW)[!] Pré-nettoyage éventuel$(RESET)"
	@sudo rmmod $(MOD) >/dev/null 2>&1 || true
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Insertion du module$(RESET)"
	@sudo insmod $(MOD).ko || { printf '%b\n' "$(RED)[-] Échec insmod$(RESET)"; exit 1; }
	@sleep 1
	@dmesg -T | tail -n 1
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Vérifier répertoire et permissions$(RESET)"
	@ls -ld $(DIR) || { printf '%b\n' "$(RED)[-] $(DIR) introuvable$(RESET)"; sudo rmmod $(MOD); exit 1; }
	@sudo chmod 755 $(DIR) || true
	@ls -l $(DIR)
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Vérifier existence et modes des fichiers$(RESET)"
	@ls -l $(ID) $(JIFFIES) $(FOO)
	@printf '  - id mode:      '; stat -c %a $(ID)
	@printf '  - jiffies mode: '; stat -c %a $(JIFFIES)
	@printf '  - foo mode:     '; stat -c %a $(FOO)
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Tests 'id' (lecture/écriture/EOF)$(RESET)"
	@got="$$(cat $(ID))"; if [ "$$got" = "$(LOGIN)" ]; then printf '%b\n' "$(GREEN)[OK] lecture id$(RESET)"; else printf '%b (%s)\n' "$(RED)[KO] lecture id" "$$got$(RESET)"; fi
	@printf "$(LOGIN)" >| $(ID) && printf '%b\n' "$(GREEN)[OK] écriture id sans \\n$(RESET)" || printf '%b\n' "$(RED)[KO] écriture id sans \\n$(RESET)"
	@printf "$(LOGIN)\n" >| $(ID) && printf '%b\n' "$(GREEN)[OK] écriture id avec \\n$(RESET)" || printf '%b\n' "$(RED)[KO] écriture id avec \\n$(RESET)"
	@sh -c 'printf "wrong" >| $(ID)' >/dev/null 2>&1 && printf '%b\n' "$(RED)[KO] écriture invalide acceptée$(RESET)" || printf '%b\n' "$(GREEN)[OK] écriture invalide rejetée$(RESET)"
	@LEN=$$(printf "%s" "$(LOGIN)" | wc -c | tr -d ' '); \
	  exec 3<$(ID); first=$$(dd bs=$$LEN count=1 <&3 2>/dev/null | cat); second=$$(dd bs=$$LEN count=1 <&3 2>/dev/null | cat); exec 3<&-; \
	  if [ "$$first" = "$(LOGIN)" ] && [ -z "$$second" ]; then printf '%b\n' "$(GREEN)[OK] EOF sur même FD$(RESET)"; else printf '%b (first=%s second=%s)\n' "$(RED)[KO] EOF" "$$first" "$$second$(RESET)"; fi
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Tests 'jiffies' (lecture, progression)$(RESET)"
	@j1="$$(cat $(JIFFIES))"; sleep 1; j2="$$(cat $(JIFFIES))"; \
	  printf '  j1=%s  j2=%s\n' "$$j1" "$$j2"; \
	  case "$$j1$$j2" in (*[!0-9]*) printf '%b\n' "$(RED)[KO] valeur non numérique$(RESET)";; esac; \
	  if [ $${j2:-0} -gt $${j1:-0} ]; then printf '%b\n' "$(GREEN)[OK] jiffies augmente$(RESET)"; else printf '%b\n' "$(RED)[KO] jiffies n’augmente pas$(RESET)"; fi
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Tests 'foo' (root OK, non-root refusé, limites)$(RESET)"
	@echo "hello world" | sudo tee $(FOO) >/dev/null && printf '%b\n' "$(GREEN)[OK] écriture root$(RESET)" || printf '%b\n' "$(RED)[KO] écriture root$(RESET)"
	@got="$$(cat $(FOO))"; if [ "$$got" = "hello world" ]; then printf '%b\n' "$(GREEN)[OK] lecture foo$(RESET)"; else printf '%b (%s)\n' "$(RED)[KO] lecture foo" "$$got$(RESET)"; fi
	@if [ "$(NONROOT)" = "false" ]; then \
		printf '%b\n' "$(YELLOW)[!] utilisateur 'nobody' introuvable, saute test non-root$(RESET)"; \
	else \
		sudo -u $(NONROOT) sh -c 'printf X > $(FOO)' >/dev/null 2>&1 && printf '%b\n' "$(RED)[KO] écriture non-root acceptée$(RESET)" || printf '%b\n' "$(GREEN)[OK] écriture non-root refusée$(RESET)"; \
	fi
	@head -c 4096 /dev/zero | tr '\0' 'A' | sudo tee $(FOO) >/dev/null && printf '%b\n' "$(GREEN)[OK] write 4096 bytes$(RESET)" || printf '%b\n' "$(RED)[KO] write 4096 bytes$(RESET)"
	@sz="$$(wc -c < $(FOO))"; printf '  taille lue=%s\n' "$$sz"; [ "$$sz" -eq 4096 ] && printf '%b\n' "$(GREEN)[OK] taille conforme$(RESET)" || printf '%b\n' "$(RED)[KO] taille inattendue$(RESET)"
	@head -c 4097 /dev/zero | tr '\0' 'B' | sudo tee $(FOO) >/dev/null && printf '%b\n' "$(RED)[KO] write >PAGE_SIZE accepté$(RESET)" || printf '%b\n' "$(GREEN)[OK] write >PAGE_SIZE refusé$(RESET)"
	@printf '\n'
	@printf '%b\n' "$(BLUE)[+] Suppression du module et vérif cleanup$(RESET)"
	@sudo rmmod $(MOD) || { printf '%b\n' "$(RED)[-] Échec rmmod$(RESET)"; exit 1; }
	@sleep 1
	@dmesg -T | tail -n 2
	@ls -ld $(DIR) >/dev/null 2>&1 && printf '%b\n' "$(RED)[KO] répertoire existe encore$(RESET)" || printf '%b\n' "$(GREEN)[OK] répertoire supprimé$(RESET)"
	@printf '\n'

test-log: all
	@echo "[STEP 0] Reset dmesg"
	@out="$$(dmesg -c 2>&1 || true)"; rc=$$?; echo "OBTAINED: $$out"; echo "RC: $$rc"; echo
	@echo "[STEP 1] Mount debugfs if needed"
	@mount_ok="$$(grep -qsE 'debugfs .* $(DBGROOT) ' /proc/mounts && echo yes || echo no)"; \
	if [ "$$mount_ok" = "no" ]; then out="$$(sudo mount -t debugfs none $(DBGROOT) 2>&1)"; rc=$$?; echo "MOUNT: $$out"; echo "RC: $$rc"; else echo "MOUNT: already mounted"; echo "RC: 0"; fi; echo
	@echo "[STEP 2] Pre-unload module"
	@out="$$(sudo rmmod $(MOD) 2>&1 || true)"; rc=$$?; echo "OBTAINED: $$out"; echo "RC: $$rc"; echo
	@echo "[STEP 3] Insert module"
	@out="$$(sudo insmod $(MOD).ko 2>&1)"; rc=$$?; kmsg="$$(dmesg | tail -n 1)"; echo "OBTAINED: $$out"; echo "KMSG: $$kmsg"; echo "RC: $$rc"; echo
	@echo "[STEP 4] Check directory"
	@out="$$(ls -ld $(DIR) 2>&1)"; rc=$$?; echo "OBTAINED: $$out"; echo "RC: $$rc"; echo
	@sudo chmod 755 $(DIR) || true
	@echo "[STEP 5] Check files and modes"
	@for f in $(ID) $(JIFFIES) $(FOO); do out="$$(ls -l $$f 2>&1)"; rc=$$?; echo "$$f => $$out / RC=$$rc"; done; \
	echo "ID mode=$$(stat -c %a $(ID)) JIFFIES mode=$$(stat -c %a $(JIFFIES)) FOO mode=$$(stat -c %a $(FOO))"; echo
	@echo "[STEP 6] id: read"
	@got="$$(cat $(ID) 2>&1)"; rc=$$?; echo "OBTAINED: $$got"; echo "RC: $$rc"; if [ "$$got" = "$(LOGIN)" ]; then echo "RESULT: PASS"; else echo "RESULT: FAIL"; fi; echo
	@echo "[STEP 7] id: write valid (no NL)"
	@sh -c 'printf "%s" "$(LOGIN)" >| $(ID)'; rc=$$?; echo "RC: $$rc"; [ $$rc -eq 0 ] && echo "RESULT: PASS" || echo "RESULT: FAIL"; echo
	@echo "[STEP 8] id: write valid (with NL)"
	@sh -c 'printf "%s\n" "$(LOGIN)" >| $(ID)'; rc=$$?; echo "RC: $$rc"; [ $$rc -eq 0 ] && echo "RESULT: PASS" || echo "RESULT: FAIL"; echo
	@echo "[STEP 9] id: write invalid content"
	@sh -c 'printf "wrong" >| $(ID)'; rc=$$?; echo "RC: $$rc"; [ $$rc -ne 0 ] && echo "RESULT: PASS" || echo "RESULT: FAIL"; echo
	@echo "[STEP 10] id: EOF on same FD"
	@LEN=$$(printf "%s" "$(LOGIN)" | wc -c | tr -d ' '); \
	exec 3<$(ID); first=$$(dd bs=$$LEN count=1 <&3 2>/dev/null | cat); second=$$(dd bs=$$LEN count=1 <&3 2>/dev/null | cat); rc=$$?; exec 3<&-; \
	echo "first='$$first' second='$$second' RC=$$rc"; \
	if [ "$$first" = "$(LOGIN)" ] && [ -z "$$second" ] && [ $$rc -eq 0 ]; then echo "RESULT: PASS"; else echo "RESULT: FAIL"; fi; echo
	@echo "[STEP 11] jiffies: read twice and compare"
	@j1="$$(cat $(JIFFIES) 2>/dev/null)"; sleep 1; j2="$$(cat $(JIFFIES) 2>/dev/null)"; echo "j1=$$j1 j2=$$j2"; \
	case "$$j1$$j2" in (*[!0-9]*) echo "RESULT: FAIL (non-numeric)";; *) if [ $${j2:-0} -gt $${j1:-0} ]; then echo "RESULT: PASS"; else echo "RESULT: FAIL"; fi;; esac; echo
	@echo "[STEP 12] foo: write as root and read back"
	@out="$$(echo 'hello world' | sudo tee $(FOO) 2>&1 >/dev/null)"; rc=$$?; echo "WRITE RC: $$rc"; got="$$(cat $(FOO) 2>&1)"; echo "READ: $$got"; [ "$$got" = "hello world" ] && echo "RESULT: PASS" || echo "RESULT: FAIL"; echo
	@echo "[STEP 13] foo: non-root write denied"
	@if [ "$(NONROOT)" = "false" ]; then echo "SKIP: nobody not found"; echo; else \
		out="$$(sudo -u $(NONROOT) sh -c 'printf X > $(FOO)' 2>&1)"; rc=$$?; echo "OBTAINED: $$out"; echo "RC: $$rc"; [ $$rc -ne 0 ] && echo "RESULT: PASS" || echo "RESULT: FAIL"; echo; fi
	@echo "[STEP 14] foo: PAGE_SIZE boundary"
	@out="$$(head -c 4096 /dev/zero | tr '\0' 'A' | sudo tee $(FOO) >/dev/null 2>&1)"; rc=$$?; echo "WRITE 4096 RC: $$rc"; sz="$$(wc -c < $(FOO))"; echo "SIZE READ: $$sz"; [ "$$sz" -eq 4096 ] && echo "RESULT: PASS" || echo "RESULT: FAIL"; echo
	@out="$$(head -c 4097 /dev/zero | tr '\0' 'B' | sudo tee $(FOO) >/dev/null 2>&1)"; rc=$$?; echo "WRITE 4097 RC: $$rc"; [ $$rc -ne 0 ] && echo "RESULT: PASS" || echo "RESULT: FAIL"; echo
	@echo "[STEP 15] Remove module and verify cleanup"
	@out="$$(sudo rmmod $(MOD) 2>&1)"; rc=$$?; echo "RMMOD: $$out"; echo "RC: $$rc"; ls -ld $(DIR) >/dev/null 2>&1 && echo "CLEANUP: FAIL (dir exists)" || echo "CLEANUP: PASS"; echo

